// SleepLamp · Sensor.ino — C1001 radar task (core 0)
// Owns the UART completely: init (non-blocking for the rest of the system),
// link-health watchdog with auto-recovery, validated + smoothed reads,
// and on-device sleep-session recording (1 sample/min ring).
#include "types.h"
#include "config.h"

// keep a value only if it falls in a sane range, else hold the previous one
static inline void upd(int &dst, int v, int lo, int hi) { if (v >= lo && v <= hi) dst = v; }

// rolling-average filter for vitals (drops corrupt/out-of-range, steadies the number)
struct Smoother {
  int buf[6]; int n = 0, idx = 0, miss = 0;
  void push(int v) { buf[idx] = v; idx = (idx + 1) % 6; if (n < 6) n++; miss = 0; }
  void noData()    { if (++miss >= 4) { n = 0; idx = 0; } }     // clear after 4 bad cycles
  void reset()     { n = 0; idx = 0; miss = 0; }
  int  value()     { if (!n) return 0; long s = 0; for (int i = 0; i < n; i++) s += buf[i]; return (int)(s / n); }
};
static Smoother hrSm, brSm;

// ---------------- init (runs inside this task; never blocks WiFi/web) ----------------
// The radar needs ~10-15 s after power-on. begin() is now light (flush + 1 probe),
// so we just retry until it answers. If the radar is already running (ESP reboot),
// this succeeds in under a second.
static bool radarInit() {
  while (Serial1.available()) Serial1.read();         // drop boot chatter / stale bytes
  if (hu.begin() != 0) return false;                  // not answering yet

  uint8_t mode = hu.getWorkMode();
  if (mode != 2) {                                    // not in sleep mode -> switch once
    Serial.println("[C1001] switching to sleep mode (takes ~20 s, once)");
    if (hu.configWorkMode(hu.eSleepMode) != 0) return false;
    hu.sensorRet();                                   // reset required after mode change
    vTaskDelay(pdMS_TO_TICKS(12000));                 // let the radar reboot fully
    while (Serial1.available()) Serial1.read();
    if (hu.getWorkMode() != 2) return false;
  }
  // NOTE: deliberately NO sensorRet() on normal boots — the mode persists in the
  // radar's flash, and a pointless reset cost 10+ s and a fresh failure window.
  hrSm.reset(); brSm.reset();
  return true;
}

void sensorTask(void*) {
  SensorData cur;
  uint32_t n = 0, lastSess = 0;
  int failStreak = 0;

  for (;;) {
    // ---- (re)initialise when needed ----
    if (!radarOk || g_sensorReset) {
      bool userAsk = g_sensorReset; g_sensorReset = false;
      if (userAsk) { Serial.println("[C1001] recalibrating (user)"); hu.sensorRet(); vTaskDelay(pdMS_TO_TICKS(11000)); }
      radarOk = radarInit();
      if (!radarOk) { vTaskDelay(pdMS_TO_TICKS(2000)); continue; }   // retry every ~2 s, web stays up
      failStreak = 0;
      Serial.println("[C1001] link up, sleep mode active");
    }

    // ---- harvest everything the radar PUSHED since last cycle (no queries) ----
    hu.pump();

    // cache-first read: pushed report cmd = reply cmd minus the 0x80 bit.
    // Fresh pushed value -> use it free of charge; otherwise fall back to a query.
    auto rep1 = [](uint8_t con, uint8_t qcmd) -> int {
      uint8_t d[16];
      int n = hu.cacheGet(con, qcmd & 0x7F, d, sizeof(d), 1600);
      return n > 0 ? d[0] : -1;
    };

    // ---- FAST group: presence / motion / vitals (every cycle, ~0.4 s) ----
    int rawP = rep1(0x80, 0x81);
    if (rawP < 0) rawP = hu.smHumanData(hu.eHumanPresence);
    if (rawP == 0 || rawP == 1) { cur.presence = rawP; failStreak = 0; }
    else if (++failStreak >= 12) {                     // ~10 s of dead/garbage answers
      Serial.println("[C1001] link lost, auto-reinit");
      radarOk = false; continue;
    }

    int v;
    v = rep1(0x80, 0x82); if (v < 0) v = hu.smHumanData(hu.eHumanMovement);    upd(cur.motion, v, 0, 2);
    v = rep1(0x80, 0x83); if (v < 0) v = hu.smHumanData(hu.eHumanMovingRange); upd(cur.movingRange, v, 0, 100);

    int rawHR = rep1(0x85, 0x82); if (rawHR < 0) rawHR = hu.getHeartRate();
    int rawBR = rep1(0x81, 0x82); if (rawBR < 0) rawBR = hu.getBreatheValue();
    if (!cur.presence) { hrSm.reset(); brSm.reset(); }   // no one -> drop vitals immediately
    else {
      if (rawHR >= 40 && rawHR <= 180) hrSm.push(rawHR); else hrSm.noData();
      if (rawBR >=  6 && rawBR <=  30) brSm.push(rawBR); else brSm.noData();
    }
    cur.heartRate  = hrSm.value();
    cur.breathRate = brSm.value();

    v = rep1(0x81, 0x81); if (v < 0) v = hu.getBreatheState();             upd(cur.breathState, v, 1, 4);
    v = rep1(0x84, 0x81); if (v < 0) v = hu.smSleepData(hu.eInOrNotInBed); upd(cur.inBed, v, 0, 1);
    v = rep1(0x84, 0x82); if (v < 0) v = hu.smSleepData(hu.eSleepState);   upd(cur.sleepState, v, 0, 3);
    if (!cur.presence) cur.inBed = 0;   // radar's in-bed flag goes stale when you walk away

    // ---- MEDIUM group: live session durations (every 4th cycle, ~1.6 s) ----
    if (n % 4 == 0) {
      upd(cur.distance,    hu.smHumanData(hu.eHumanDistance),    0, 1200);
      // 255 (0xFF) in all duration fields = corrupt frame, never real data
      v = hu.smSleepData(hu.eWakeDuration);      if (v != 255) upd(cur.wakeDur,  v, 0, 1440);
      v = hu.smSleepData(hu.eLightsleep);        if (v != 255) upd(cur.lightDur, v, 0, 1440);
      v = hu.smSleepData(hu.eDeepSleepDuration); if (v != 255) upd(cur.deepDur,  v, 0, 1440);
      upd(cur.quality,     hu.smSleepData(hu.eSleepQuality),      0, 100);
      upd(cur.disturbance, hu.smSleepData(hu.eSleepDisturbances), 0, 3);
      upd(cur.rating,      hu.smSleepData(hu.eSleepQualityRating),0, 3);
      upd(cur.abnormal,    hu.smSleepData(hu.eAbnormalStruggle),  0, 2);
    }

    // ---- SLOW group: composite + nightly statistics (every 8th cycle, ~3.2 s) ----
    if (n % 8 == 0) {
      sSleepComposite c = hu.getSleepComposite();
      upd(cur.cResp, c.averageRespiration, 6, 30);  upd(cur.cHeart, c.averageHeartbeat, 40, 180);
      upd(cur.cTurn, c.turnoverNumber, 0, 120);     upd(cur.cLarge, c.largeBodyMove, 0, 100);
      upd(cur.cMinor, c.minorBodyMove, 0, 100);     upd(cur.cApnea, c.apneaEvents, 0, 40);

      sSleepStatistics s = hu.getSleepStatistics();
      upd(cur.sScore, s.sleepQualityScore, 1, 100); upd(cur.sSleepTime, s.sleepTime, 1, 1440);
      upd(cur.sWake, s.wakeDuration, 0, 100);       upd(cur.sShallow, s.shallowSleepPercentage, 0, 100);
      upd(cur.sDeep, s.deepSleepPercentage, 0, 100);upd(cur.sOOB, s.timeOutOfBed, 0, 200);
      upd(cur.sExit, s.exitCount, 0, 50);           upd(cur.sTurn, s.turnOverCount, 0, 200);
      upd(cur.sResp, s.averageRespiration, 6, 30);  upd(cur.sHeart, s.averageHeartbeat, 40, 180);
      upd(cur.sApnea, s.apneaEvents, 0, 60);
    }

    cur.valid = true; cur.seq = ++n;

    // ---- our sleep-staging engine: one feed per poll, reports at session end ----
    SleepLive lv; NightReport rp; bool repReady = false;
    sleepFeed(cur, lv, rp, repReady);
    if (repReady) {
      if (cur.cApnea > 0 && cur.cApnea <= 40) rp.apnea = cur.cApnea;  // radar apnea if sane
      reportSave(rp);                                  // stamps rp.when, appends history
    }

    xSemaphoreTake(mux, portMAX_DELAY);
    cur.tempC = g.tempC; cur.humidity = g.humidity; cur.envValid = g.envValid;  // env owned by envTask
    g = cur;
    live = lv;
    if (repReady) lastReport = rp;

    // new session -> wipe the ring so the hypnogram shows ONLY this session
    static bool prevActive = false;
    if (lv.active && !prevActive) { sessN = 0; sessStart = 0; lastSess = 0; }
    prevActive = lv.active;

    // ---- session recorder: 1 sample/min into the ring (device-side hypnogram) ----
    // Records the ENGINE stage (responds from minute 1), not the radar's.
    if (millis() - lastSess >= 60000UL) {
      lastSess = millis();
      SessSample sm;
      sm.stage = (uint8_t)lv.stage;          // 3 = no session -> gap in hypnogram
      sm.hr    = (uint8_t)cur.heartRate;
      sm.br    = (uint8_t)cur.breathRate;
      if (sessN < SESS_MAX) sessBuf[(sessStart + sessN++) % SESS_MAX] = sm;
      else { sessBuf[sessStart] = sm; sessStart = (sessStart + 1) % SESS_MAX; }
    }
    xSemaphoreGive(mux);

    vTaskDelay(pdMS_TO_TICKS(400));   // radar updates ~1 Hz — no point hammering the UART
  }
}
