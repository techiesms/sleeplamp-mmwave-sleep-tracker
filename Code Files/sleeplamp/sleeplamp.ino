/**
 * SleepLamp — contactless sleep lamp  | arduino-esp32 core 3.1+
 * ESP32-S3 + C1001 mmWave radar + DHT11 + NeoPixel ring + TTP223 touch + Matter
 *
 *  Architecture (deliberate):
 *   - setup() brings up WiFi + web FIRST. The radar is initialised inside
 *     sensorTask (core 0) with retries — a dead/booting radar can NEVER block
 *     the dashboard. radarOk reports link health live.
 *   - sensorTask owns the UART exclusively (init, recovery, recalibration).
 *   - 1-min sleep-session ring on-device -> hypnogram survives page refreshes.
 *
 *  Board: ESP32S3 Dev Module. Partition: "Huge APP (3MB No OTA/1MB SPIFFS)".
 *  Dashboard: http://sleeplamp.local
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "config.h"
#include "types.h"

// ---- global definitions (declared extern in types.h) ----
SensorData              g;
LightState              light;
AlarmState              alarmCfg;
WebServer               server(80);
ShubhSensor             hu(&Serial1);
SemaphoreHandle_t       mux;
char                    timeStr[6] = "--:--";
bool                    timeOk = false;
volatile bool           g_sensorReset = false;
volatile bool           radarOk = false;
volatile bool           g_endSession = false;
volatile bool           g_alarmStop = false;
volatile bool           g_lightDirty = false;
SleepLive               live;
NightReport             lastReport;
SessSample              sessBuf[SESS_MAX];
int                     sessN = 0, sessStart = 0;

static void printTelemetry() {
  SensorData d; LightState L; AlarmState A; SleepLive lv; NightReport R; char tb[6];
  xSemaphoreTake(mux, portMAX_DELAY);
  d=g; L=light; A=alarmCfg; lv=live; R=lastReport; memcpy(tb,timeStr,6);
  xSemaphoreGive(mux);
  const char* kState[] = {"Deep","Light","Awake","None"};
  const char* kMot[]   = {"None","Still","Active"};
  Serial.println("------------------------------------------------");
  Serial.printf("radar:%s  time:%s  presence:%d  bed:%s  radarState:%s  motion:%s  range:%d\n",
                radarOk?"OK":"DOWN", tb, d.presence, d.inBed?"in":"out",
                kState[d.sleepState&3], kMot[d.motion>2?0:d.motion], d.movingRange);
  Serial.printf("HR:%d bpm  resp:%d rpm  breathState:%d\n",
                d.heartRate, d.breathRate, d.breathState);
  Serial.printf("SLEEP[engine] %s  stage:%s  since:%s  bed:%dm asleep:%dm (deep %d/light %d/awake %d)\n",
                lv.active?"SESSION":"idle", kState[lv.stage&3], lv.since,
                lv.bedMin, lv.sleepMin, lv.deepMin, lv.lightMin, lv.awakeMin);
  Serial.printf("SLEEP[engine] onset:%dm  wakes:%d  turns:%d  liveScore:%d   lastReport:%s score:%d\n",
                lv.onsetMin, lv.wakes, lv.turns, lv.score,
                R.valid?R.when:"none", R.score);
  Serial.printf("ENV temp:%d C  humidity:%d %%  (valid:%d)\n", d.tempC, d.humidity, d.envValid);
  Serial.printf("LIGHT mode:%d rgb(%d,%d,%d) bright:%d%%   ALARM %02d:%02d en:%d firing:%d   heap:%lu\n",
                L.mode, L.r, L.g, L.b, L.bright, A.hour, A.minute, A.enabled, A.firing, (unsigned long)ESP.getFreeHeap());
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n===== SleepLamp booting =====");

  // Radar UART: big RX buffer so proactive radar frames never overflow while
  // we're busy serving HTTP. Init itself happens inside sensorTask.
  Serial1.setRxBufferSize(1024);
  Serial1.begin(115200, SERIAL_8N1, RADAR_RX, RADAR_TX);
  Serial.println("[C1001] init runs in background (radar needs ~10-15 s after power-on)");

  lightBegin();
  lightBootTest();
  touchBegin();
  storeBegin();
  settingsBegin();   // restore saved lamp + alarm settings

  WiFi.mode(WIFI_STA); WiFi.setSleep(false); WiFi.setHostname(MDNS_HOST);
  WiFi.setAutoReconnect(true);
  String ws = getWifiSsid(), wp = getWifiPass();
  WiFi.begin(ws.c_str(), wp.c_str());
  Serial.printf("[WiFi] joining \"%s\"...\n", ws.c_str());
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) { delay(350); Serial.print('.'); }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] connected, IP: %s\n", WiFi.localIP().toString().c_str());
    timeBegin();
  } else {
    WiFi.mode(WIFI_AP); WiFi.softAP("SleepLamp-Setup", "sleeplamp123");
    Serial.printf("[WiFi] AP fallback \"SleepLamp-Setup\" IP: %s\n", WiFi.softAPIP().toString().c_str());
  }

  if (MDNS.begin(MDNS_HOST)) {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("[mDNS] http://%s.local\n", MDNS_HOST);
  }

  mux = xSemaphoreCreateMutex();
  server.on("/",            handleRoot);
  server.on("/api/data",    handleData);
  server.on("/api/light",   handleLight);
  server.on("/api/alarm",   handleAlarm);
  server.on("/api/history", handleHistory);
  server.on("/api/session", handleSession);
  server.on("/api/report",  handleReport);
  server.on("/api/export",  handleExport);
  server.on("/api/sensor",  handleSensor);
  server.on("/manifest.json", handleManifest);
  server.on("/wifi",   HTTP_GET,  handleWifiPage);
  server.on("/wifi",   HTTP_POST, handleWifiSave);
  server.on("/update", HTTP_GET,  handleUpdatePage);
  server.on("/update", HTTP_POST, handleUpdateDone, handleUpdateUpload);
  server.on("/api/reset", handleFactoryReset);
  server.begin();
  Serial.println("[HTTP] dashboard up (works even while radar is still booting)");

  // Matter joins the smart-home fabric over the WiFi we just brought up. Skipped
  // in AP-setup mode (no station WiFi yet) — it starts on the next boot once
  // home WiFi is configured.
  if (WiFi.status() == WL_CONNECTED) matterBegin();

  xTaskCreatePinnedToCore(sensorTask, "radar", 8192, nullptr, 2, nullptr, 0);
  xTaskCreatePinnedToCore(envTask,    "env",   4096, nullptr, 1, nullptr, 1);
  Serial.println("[TASKS] radar(core0) + env(core1) started");
}

void loop() {
  server.handleClient();
  handleTouch();                                  // poll the TTP223 every loop (snappy)
  matterLoop();                                   // Matter pairing-status prints
  if (g_lightDirty) {                             // a smart-home app changed the lamp
    g_lightDirty = false; lightApply(); settingsSaveLight();
  }
  static uint32_t tTick = 0, tPrint = 0, tWifi = 0;
  uint32_t now = millis();
  if (now - tTick > 1000) { tTick = now; if (!alarmCheck()) { lightAuto(); lightApply(); } }
  // (history writes happen inside the engine at session end — no polling store)
  if (now - tPrint > SERIAL_PRINT_MS) { tPrint = now; printTelemetry(); }
  if (now - tWifi > 10000) {                      // auto-recover dropped WiFi
    tWifi = now;
    if (WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFi] reconnecting...");
      WiFi.reconnect();
    }
  }
  // AP fallback: keep the setup hotspot up, but quietly retry home WiFi every
  // 5 min — a router that was down at boot no longer strands the lamp in AP
  // mode until someone power-cycles it.
  static uint32_t tSta = 0;
  if (WiFi.getMode() & WIFI_MODE_AP) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("[WiFi] home WiFi joined, IP %s — closing setup hotspot\n",
                    WiFi.localIP().toString().c_str());
      WiFi.mode(WIFI_STA);
      timeBegin();
    } else if (now - tSta > 300000UL) {
      tSta = now;
      String ws = getWifiSsid(), wp = getWifiPass();
      Serial.printf("[WiFi] AP mode: retrying \"%s\" in background\n", ws.c_str());
      WiFi.mode(WIFI_AP_STA);
      WiFi.begin(ws.c_str(), wp.c_str());
    }
  }
  delay(1);
}
