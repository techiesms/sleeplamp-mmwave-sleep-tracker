// SleepLamp · Settings.ino — persist lamp + alarm settings in NVS so they survive reboot
#include "types.h"
#include <Preferences.h>

static Preferences prefs;

void settingsBegin() {
  prefs.begin("sleeplamp", true);   // read-only
  light.mode   = prefs.getInt ("lmode", light.mode);
  light.r      = prefs.getInt ("lr",    light.r);
  light.g      = prefs.getInt ("lg",    light.g);
  light.b      = prefs.getInt ("lb",    light.b);
  light.bright = prefs.getInt ("lbri",  light.bright);
  alarmCfg.enabled = prefs.getBool("aen",  alarmCfg.enabled);
  alarmCfg.hour    = prefs.getInt ("ah",   alarmCfg.hour);
  alarmCfg.minute  = prefs.getInt ("am",   alarmCfg.minute);
  alarmCfg.window  = prefs.getInt ("awin", alarmCfg.window);
  prefs.end();
  Serial.println(F("[NVS] settings restored"));
}

void settingsSaveLight() {
  prefs.begin("sleeplamp", false);
  prefs.putInt("lmode", light.mode); prefs.putInt("lr", light.r); prefs.putInt("lg", light.g);
  prefs.putInt("lb", light.b);       prefs.putInt("lbri", light.bright);
  prefs.end();
}

void settingsSaveAlarm() {
  prefs.begin("sleeplamp", false);
  prefs.putBool("aen", alarmCfg.enabled); prefs.putInt("ah", alarmCfg.hour);
  prefs.putInt("am", alarmCfg.minute);    prefs.putInt("awin", alarmCfg.window);
  prefs.end();
}
