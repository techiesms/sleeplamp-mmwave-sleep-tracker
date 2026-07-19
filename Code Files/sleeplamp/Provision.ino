// SleepLamp · Provision.ino — WiFi setup portal, OTA firmware update, factory reset
#include "types.h"
#include "config.h"
#include <Preferences.h>
#include <Update.h>
#include <LittleFS.h>

static Preferences pv;

String getWifiSsid() { pv.begin("sleeplamp", true); String s = pv.getString("wssid", WIFI_SSID); pv.end(); return s; }
String getWifiPass() { pv.begin("sleeplamp", true); String s = pv.getString("wpass", WIFI_PASS); pv.end(); return s; }

static const char* HEAD =
  "<!DOCTYPE html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>"
  "<style>body{background:#0a0e16;color:#e7ebf3;font:15px/1.5 system-ui;max-width:430px;margin:26px auto;padding:0 16px}"
  "h2{font-weight:700;margin-bottom:6px}.m{color:#828ea6;font-size:13px}"
  "input,button{width:100%;padding:12px;margin:7px 0;border-radius:10px;border:1px solid #212a3b;background:#10161f;color:#e7ebf3;font-size:15px;box-sizing:border-box}"
  "button{background:#7c84f2;color:#0b0f18;font-weight:700;border:0;cursor:pointer}a{color:#7c84f2}</style>";

void handleWifiPage() {
  server.send(200, "text/html", String(HEAD) +
    "<h2>WiFi Setup</h2><p class=m>Connect SleepLamp to your home WiFi.</p>"
    "<form method='POST' action='/wifi'>"
    "<input name='ssid' placeholder='WiFi name' value='" + getWifiSsid() + "'>"
    "<input name='pass' type='password' placeholder='WiFi password'>"
    "<button>Save &amp; Reboot</button></form><p><a href='/'>&larr; Dashboard</a></p>");
}

void handleWifiSave() {
  if (!server.hasArg("ssid")) { server.send(400, "text/plain", "missing ssid"); return; }
  pv.begin("sleeplamp", false);
  pv.putString("wssid", server.arg("ssid"));
  pv.putString("wpass", server.arg("pass"));
  pv.end();
  server.send(200, "text/html", String(HEAD) +
    "<h2>Saved. Rebooting&hellip;</h2><p class=m>Join the same WiFi, then open <b>http://" MDNS_HOST ".local</b></p>");
  delay(900); ESP.restart();
}

void handleUpdatePage() {
  server.send(200, "text/html", String(HEAD) +
    "<h2>Firmware Update</h2><p class=m>Upload a compiled .bin (Arduino: Sketch &rarr; Export Compiled Binary).</p>"
    "<form method='POST' action='/update' enctype='multipart/form-data'>"
    "<input type='file' name='fw' accept='.bin'><button>Upload &amp; Flash</button></form>"
    "<p><a href='/'>&larr; Dashboard</a></p>");
}

void handleUpdateDone() {
  bool ok = !Update.hasError();
  server.send(200, "text/html", String(HEAD) +
    (ok ? "<h2>Update OK. Rebooting&hellip;</h2>" : "<h2>Update FAILED</h2><p><a href='/update'>Try again</a></p>"));
  if (ok) { delay(900); ESP.restart(); }
}

void handleUpdateUpload() {
  HTTPUpload& up = server.upload();
  if (up.status == UPLOAD_FILE_START)      { Serial.printf("[OTA] %s\n", up.filename.c_str()); Update.begin(UPDATE_SIZE_UNKNOWN); }
  else if (up.status == UPLOAD_FILE_WRITE) { Update.write(up.buf, up.currentSize); }
  else if (up.status == UPLOAD_FILE_END)   { Update.end(true); }
}

void handleFactoryReset() {
  if (!server.hasArg("confirm")) { server.send(200, "application/json", "{\"need\":\"confirm\"}"); return; }
  Preferences p; p.begin("sleeplamp", false); p.clear(); p.end();
  LittleFS.remove("/history.csv");
  server.send(200, "text/html", String(HEAD) + "<h2>Factory reset done. Rebooting&hellip;</h2>");
  delay(900); ESP.restart();
}
