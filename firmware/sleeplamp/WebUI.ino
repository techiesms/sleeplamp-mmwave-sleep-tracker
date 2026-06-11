// SleepLamp · WebUI.ino — HTTP handlers: dashboard, live JSON, light + alarm control
#include "types.h"
#include "config.h"
#include "page.h"

void handleData() {
  SensorData d; LightState L; AlarmState A; SleepLive lv; NightReport R;
  char tbuf[6]; bool tok;
  xSemaphoreTake(mux, portMAX_DELAY);
  d = g; L = light; A = alarmCfg; lv = live; R = lastReport;
  tok = timeOk; memcpy(tbuf, timeStr, sizeof(tbuf));
  xSemaphoreGive(mux);
  int rDeepP  = R.sleepMin ? R.deepMin  * 100 / R.sleepMin : 0;
  int rLightP = R.sleepMin ? R.lightMin * 100 / R.sleepMin : 0;
  String ipS = (WiFi.getMode() & WIFI_MODE_AP) ? WiFi.softAPIP().toString()
                                               : WiFi.localIP().toString();
  static char b[2500];          // only ever called from the loop task
  snprintf(b, sizeof(b),
    "{\"valid\":%d,\"seq\":%lu,\"uptime\":%lu,\"rssi\":%d,\"heap\":%lu,\"fw\":\"" FW_VERSION "\",\"radarOk\":%d,"
    "\"ip\":\"%s\","
    "\"time\":\"%s\",\"timeOk\":%d,"
    "\"presence\":%d,\"motion\":%d,\"movingRange\":%d,\"distance\":%d,"
    "\"heartRate\":%d,\"breathRate\":%d,\"breathState\":%d,"
    "\"inBed\":%d,\"sleepState\":%d,\"quality\":%d,"
    "\"disturbance\":%d,\"rating\":%d,\"abnormal\":%d,"
    "\"cResp\":%d,\"cHeart\":%d,\"cTurn\":%d,\"cLarge\":%d,\"cMinor\":%d,\"cApnea\":%d,"
    "\"myStage\":%d,\"sessOn\":%d,\"since\":\"%s\",\"bedMin\":%d,\"asleepMin\":%d,"
    "\"deepMin\":%d,\"lightMin\":%d,\"awakeMin\":%d,\"onset\":%d,\"wakes\":%d,\"turns\":%d,\"liveScore\":%d,"
    "\"rOk\":%d,\"rWhen\":\"%s\",\"rScore\":%d,\"rBed\":%d,\"rSleep\":%d,\"rDeepP\":%d,\"rLightP\":%d,"
    "\"rAwake\":%d,\"rOnset\":%d,\"rWakes\":%d,\"rHR\":%d,\"rBR\":%d,\"rTurn\":%d,\"rApnea\":%d,"
    "\"envValid\":%d,\"tempC\":%d,\"humidity\":%d,"
    "\"lightMode\":%d,\"lr\":%d,\"lg\":%d,\"lb\":%d,\"lbright\":%d,"
    "\"alOn\":%d,\"alH\":%d,\"alM\":%d,\"alWin\":%d,\"alFire\":%d}",
    d.valid?1:0,(unsigned long)d.seq,(unsigned long)(millis()/1000),(int)WiFi.RSSI(),
    (unsigned long)ESP.getFreeHeap(), radarOk?1:0,
    ipS.c_str(),
    tbuf, tok?1:0,
    d.presence,d.motion,d.movingRange,d.distance,
    d.heartRate,d.breathRate,d.breathState,
    d.inBed,d.sleepState,d.quality,
    d.disturbance,d.rating,d.abnormal,
    d.cResp,d.cHeart,d.cTurn,d.cLarge,d.cMinor,d.cApnea,
    lv.stage,lv.active?1:0,lv.since,lv.bedMin,lv.sleepMin,
    lv.deepMin,lv.lightMin,lv.awakeMin,lv.onsetMin,lv.wakes,lv.turns,lv.score,
    R.valid?1:0,R.when,R.score,R.bedMin,R.sleepMin,rDeepP,rLightP,
    R.awakeMin,R.onsetMin,R.wakes,R.avgHR,R.avgBR,R.turns,R.apnea,
    d.envValid?1:0,d.tempC,d.humidity,
    L.mode,L.r,L.g,L.b,L.bright,
    A.enabled?1:0,A.hour,A.minute,A.window,A.firing?1:0);
  server.sendHeader("Cache-Control","no-store");
  server.send(200,"application/json",b);
}

// /api/report?end=1 -> end the running session NOW and save its report.
// The engine acts on the flag within one poll (~0.4 s).
void handleReport() {
  if (server.hasArg("end")) g_endSession = true;
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", "{\"ok\":1}");
}

// /api/light?mode=&r=&g=&b=&bright=
void handleLight() {
  xSemaphoreTake(mux, portMAX_DELAY);
  if (server.hasArg("mode"))   light.mode   = constrain(server.arg("mode").toInt(),   0, 2);
  if (server.hasArg("r"))      light.r      = constrain(server.arg("r").toInt(),      0, 255);
  if (server.hasArg("g"))      light.g      = constrain(server.arg("g").toInt(),      0, 255);
  if (server.hasArg("b"))      light.b      = constrain(server.arg("b").toInt(),      0, 255);
  if (server.hasArg("bright")) light.bright = constrain(server.arg("bright").toInt(), 0, 100);
  LightState L = light;
  xSemaphoreGive(mux);
  lightApply();
  settingsSaveLight();
  char o[120];
  snprintf(o,sizeof(o),"{\"mode\":%d,\"r\":%d,\"g\":%d,\"b\":%d,\"bright\":%d}",L.mode,L.r,L.g,L.b,L.bright);
  server.sendHeader("Cache-Control","no-store");
  server.send(200,"application/json",o);
}

// /api/alarm?en=&h=&m=&win=
void handleAlarm() {
  xSemaphoreTake(mux, portMAX_DELAY);
  if (server.hasArg("en"))  alarmCfg.enabled = server.arg("en").toInt() != 0;
  if (server.hasArg("h"))   alarmCfg.hour    = constrain(server.arg("h").toInt(),   0, 23);
  if (server.hasArg("m"))   alarmCfg.minute  = constrain(server.arg("m").toInt(),   0, 59);
  if (server.hasArg("win")) alarmCfg.window  = constrain(server.arg("win").toInt(), 0, 60);
  AlarmState A = alarmCfg;
  xSemaphoreGive(mux);
  settingsSaveAlarm();
  char o[100];
  snprintf(o,sizeof(o),"{\"en\":%d,\"h\":%d,\"m\":%d,\"win\":%d}",A.enabled?1:0,A.hour,A.minute,A.window);
  server.sendHeader("Cache-Control","no-store");
  server.send(200,"application/json",o);
}

// /api/session -> tonight's per-minute stage/hr/br timeline recorded ON DEVICE
// (the hypnogram is rebuilt from this, so a page refresh loses nothing)
void handleSession() {
  static uint8_t st[SESS_MAX], hh[SESS_MAX], bb[SESS_MAX];
  int n;
  xSemaphoreTake(mux, portMAX_DELAY);
  n = sessN;
  for (int i = 0; i < n; i++) {
    const SessSample &s = sessBuf[(sessStart + i) % SESS_MAX];
    st[i] = s.stage; hh[i] = s.hr; bb[i] = s.br;
  }
  xSemaphoreGive(mux);

  String out; out.reserve(n * 12 + 64);
  out = "{\"n\":" + String(n) + ",\"stage\":[";
  for (int i = 0; i < n; i++) { if (i) out += ','; out += st[i]; }
  out += "],\"hr\":[";
  for (int i = 0; i < n; i++) { if (i) out += ','; out += hh[i]; }
  out += "],\"br\":[";
  for (int i = 0; i < n; i++) { if (i) out += ','; out += bb[i]; }
  out += "]}";
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", out);
}

// /api/sensor?reset=1 -> recalibrate radar
void handleSensor() {
  if (server.hasArg("reset")) g_sensorReset = true;
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", "{\"ok\":1}");
}

// PWA manifest -> installable on phone home screen
void handleManifest() {
  server.send(200, "application/manifest+json",
    "{\"name\":\"SleepLamp\",\"short_name\":\"SleepLamp\",\"start_url\":\"/\",\"display\":\"standalone\","
    "\"background_color\":\"#020308\",\"theme_color\":\"#020308\","
    "\"icons\":[{\"src\":\"data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64'><circle cx='32' cy='32' r='28' fill='%237d7aff'/></svg>\",\"sizes\":\"any\",\"type\":\"image/svg+xml\"}]}");
}

void handleRoot() { server.send_P(200,"text/html",PAGE); }
