// SleepLamp · Alarm.ino — NTP time + smart wake (light-sleep window) + sunrise + stand-to-stop
#include "types.h"
#include "config.h"
#include <time.h>

static int      lastFireDay  = -1;
static uint32_t fireStartMs  = 0;
static uint32_t outOfBedSince = 0;

void timeBegin() {
  configTime(TZ_OFFSET_SEC, DST_OFFSET_SEC, NTP_SERVER, "time.google.com");
  Serial.println(F("[TIME] NTP sync requested"));
}

// returns true while the sunrise alarm is firing (loop then skips normal light logic)
bool alarmCheck() {
  struct tm t;
  if (!getLocalTime(&t, 50)) { timeOk = false; return alarmCfg.firing; }
  timeOk = true;
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", t.tm_hour, t.tm_min);

  SensorData d; SleepLive lv;
  xSemaphoreTake(mux, portMAX_DELAY); d = g; lv = live; xSemaphoreGive(mux);

  int nowMin   = t.tm_hour * 60 + t.tm_min;
  int target   = alarmCfg.hour * 60 + alarmCfg.minute;
  int winStart = target - alarmCfg.window;

  if (!alarmCfg.firing) {
    if (alarmCfg.enabled && lastFireDay != t.tm_yday) {
      bool inWindow     = (nowMin >= winStart && nowMin < target);
      bool atTime       = (nowMin >= target && nowMin <= target + 1);
      // smart wake follows OUR staging engine (responds from minute 1),
      // not the radar's internal state (often "None" for 15+ min)
      bool lightOrAwake = lv.active && (lv.stage == 1 || lv.stage == 2);
      if ((inWindow && lightOrAwake) || atTime) {
        alarmCfg.firing = true; fireStartMs = millis(); outOfBedSince = 0;
        Serial.println(F("[ALARM] firing — sunrise start"));
      }
    }
    return alarmCfg.firing;
  }

  // ---- firing: drive sunrise on the lamp ----
  float p = ((millis() - fireStartMs) / 1000.0f) / (float)SUNRISE_SEC;
  lightSunrise(p);

  // stand-to-stop: person leaves bed/zone for >3 s
  if (!d.presence || !d.inBed) { if (!outOfBedSince) outOfBedSince = millis(); }
  else                          outOfBedSince = 0;
  bool stood   = outOfBedSince && (millis() - outOfBedSince > 3000);
  bool timeout = (millis() - fireStartMs > (uint32_t)(SUNRISE_SEC + 600) * 1000UL);
  bool touched = g_alarmStop;       // a tap on the TTP223 dismisses the alarm

  if (stood || timeout || touched) {
    alarmCfg.firing = false;
    g_alarmStop = false;
    lastFireDay = t.tm_yday;        // one fire per day
    Serial.println(F("[ALARM] stopped"));
  }
  return alarmCfg.firing;
}
