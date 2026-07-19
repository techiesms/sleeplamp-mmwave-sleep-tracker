// SleepLamp · Control.ino — endpoints that change something (lamp, alarm,
// session, radar). All reply with small JSON confirmations.
//   GET /api/light?mode=&r=&g=&b=&bright=
//   GET /api/alarm?en=&h=&m=&win=
//   GET /api/report?end=1     -> end the running session now and save its report
//   GET /api/sensor?reset=1   -> recalibrate the radar
#include "types.h"
#include "config.h"

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

// /api/report?end=1 -> end the running session NOW and save its report.
// The engine acts on the flag within one poll (~0.4 s).
void handleReport() {
  if (server.hasArg("end")) g_endSession = true;
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", "{\"ok\":1}");
}

// /api/sensor?reset=1 -> recalibrate radar
void handleSensor() {
  if (server.hasArg("reset")) g_sensorReset = true;
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", "{\"ok\":1}");
}
