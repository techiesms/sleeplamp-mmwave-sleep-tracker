// SleepLamp · Sleep.ino — on-device sleep-staging engine.
//
// WHY THIS EXISTS: the C1001's internal staging needs 15-20+ minutes of
// in-bed data before it reports anything (sleepState stays "None"), and its
// nightly-statistics frame arrives only once per completed night — useless
// for naps, and corrupt frames used to flood the history with junk numbers.
// This engine stages every minute from movement + vitals, works from minute
// one, and builds the night report itself so it can also be generated on
// demand from the dashboard.
//
// Runs entirely inside sensorTask (single thread): sleepFeed() is called once
// per radar poll (~0.4 s) and finalises one staging epoch per minute.
//
// Staging rules (epoch = 1 minute):
//   session starts : present + in bed
//   asleep (light) : ONSET_QUIET_EP consecutive quiet minutes (back-dated)
//   deep           : DEEP_AFTER_EP very-quiet minutes AND heart rate below
//                    the awake-in-bed baseline (skipped if no HR lock)
//   awakening      : sustained movement while asleep
//   turnover       : brief movement spike while asleep (stays asleep)
//   session ends   : out of bed/zone OOB_END_EP min, or "End session" button
#include "types.h"
#include "config.h"
#include <time.h>

// ---- per-epoch accumulators ----
static uint32_t epStart = 0;
static int  nSamp=0, sumRange=0, bursts=0, presCnt=0, bedCnt=0;
static long sumHR=0;  static int nHR=0;
static long sumBR=0;  static int nBR=0;

// ---- session state ----
static bool sOn=false;
static int  stage=3;                       // 0 deep, 1 light, 2 awake, 3 none
static int  bedMin=0, deepMin=0, lightMin=0, awakeMin=0;
static int  quietRun=0, vQuietRun=0, oobRun=0, onset=-1, wakes=0, turns=0;
static int  hrBase=0;                      // awake-in-bed heart-rate baseline
static long sesHR=0;  static int sesHRn=0; // averaged over asleep minutes only
static long sesBR=0;  static int sesBRn=0;
static char sinceClk[6]="--:--";

static void clockNow(char* out) {
  struct tm t;
  if (getLocalTime(&t, 20)) snprintf(out, 6, "%02d:%02d", t.tm_hour, t.tm_min);
  else strcpy(out, "--:--");
}

// 45% efficiency + 25% deep share (target 25%) + 15% duration (target 7 h)
// + 15% few awakenings. Clamped 1..99 so a junk-free score is always plausible.
static int scoreOf(int bed, int sleep, int deep, int wk) {
  if (bed <= 0 || sleep <= 0) return sleep > 0 ? 1 : 0;
  int eff   = sleep * 100 / bed;
  int deepP = deep * 100 / sleep;
  int sc = (45*eff + 25*min(100, deepP*4) + 15*min(100, sleep*100/420)
            + 15*max(0, 100 - 12*wk)) / 100;
  return constrain(sc, 1, 99);
}

static void resetSession() {
  sOn=false; stage=3; bedMin=deepMin=lightMin=awakeMin=0;
  quietRun=vQuietRun=oobRun=0; onset=-1; wakes=turns=0; hrBase=0;
  sesHR=sesBR=0; sesHRn=sesBRn=0; strcpy(sinceClk,"--:--");
}

// dropTrail: minutes spent already-gone before the out-of-bed timeout fired
static void buildReport(NightReport& r, int dropTrail) {
  r.valid    = true;
  r.bedMin   = max(0, bedMin  - dropTrail);
  r.awakeMin = max(0, awakeMin- dropTrail);
  r.deepMin  = deepMin;  r.lightMin = lightMin;
  r.sleepMin = deepMin + lightMin;
  r.onsetMin = max(0, onset);
  r.wakes    = wakes;    r.turns = turns;
  r.avgHR    = sesHRn ? (int)(sesHR/sesHRn) : 0;
  r.avgBR    = sesBRn ? (int)(sesBR/sesBRn) : 0;
  r.apnea    = 0;                       // caller fills from radar if sane
  r.score    = scoreOf(r.bedMin, r.sleepMin, r.deepMin, r.wakes);
  r.when[0]  = 0;                       // stamped by reportSave()
}

void sleepFeed(const SensorData& s, SleepLive& outLive,
               NightReport& outRep, bool& repReady) {
  repReady = false;
  uint32_t now = millis();
  uint32_t epLen = SLEEP_EPOCH_SEC * 1000UL;
  if (!epStart) epStart = now;

  // Radar outage / long stall: sleepFeed wasn't called for several epochs,
  // so the accumulators mix samples that are minutes apart. A staging
  // decision from that mush would be garbage — restart the epoch cleanly.
  if (now - epStart >= epLen * 3) {
    epStart = now; nSamp = 0; sumRange = 0; bursts = 0;
    presCnt = bedCnt = 0; sumHR = sumBR = 0; nHR = nBR = 0;
  }

  nSamp++; sumRange += s.movingRange;
  if (s.movingRange >= BURST_RANGE) bursts++;
  if (s.presence) presCnt++;
  if (s.presence && s.inBed) bedCnt++;
  if (s.heartRate  > 0) { sumHR += s.heartRate;  nHR++; }
  if (s.breathRate > 0) { sumBR += s.breathRate; nBR++; }

  // ---- epoch boundary: one staging decision per "minute" ----
  if (now - epStart >= epLen && nSamp >= 3) {
    int  meanRange = sumRange / nSamp;
    int  burstPct  = bursts * 100 / nSamp;   // % of samples spiking >= BURST_RANGE
    bool present   = presCnt * 2 > nSamp;
    bool inbed     = present && bedCnt * 2 > nSamp;
    int  epHR      = nHR ? (int)(sumHR/nHR) : 0;
    int  epBR      = nBR ? (int)(sumBR/nBR) : 0;
    // burst thresholds as a fraction of the epoch, so they hold at any
    // epoch length: 4% of a 60 s epoch ≈ 2.4 s of movement spikes.
    bool quiet     = inbed && meanRange <= QUIET_RANGE && burstPct <= 4;
    bool vQuiet    = quiet && meanRange <= 4 && burstPct <= 1;

    if (!sOn) {
      if (inbed) {                              // ---- session begins ----
        resetSession(); sOn = true; stage = 2;
        clockNow(sinceClk);
        Serial.println(F("[SLEEP] session started (in bed)"));
      }
    } else {
      bedMin++;
      if (!present) {                           // out of bed / out of zone
        oobRun++; awakeMin++;
        stage = 2; quietRun = vQuietRun = 0;
      } else {
        oobRun = 0;
        if (stage == 2 && epHR > 0)             // learn baseline while awake
          hrBase = hrBase ? (hrBase*3 + epHR)/4 : epHR;

        if (stage == 2) {                       // awake -> falling asleep?
          awakeMin++;
          quietRun = quiet ? quietRun + 1 : 0;
          if (quietRun >= ONSET_QUIET_EP) {
            stage = 1;                          // asleep — back-date onset
            awakeMin -= quietRun; lightMin += quietRun;
            if (onset < 0) onset = max(0, bedMin - quietRun);
            vQuietRun = vQuiet ? 1 : 0;
            Serial.println(F("[SLEEP] fell asleep (light)"));
          }
        } else {                                // asleep
          // awakening = sustained movement (>=13% of the epoch ≈ 8 s) or
          // high mean activity for the whole minute
          bool moveWake = meanRange >= WAKE_RANGE || burstPct >= 13;
          if (moveWake) {
            stage = 2; wakes++; awakeMin++; quietRun = vQuietRun = 0;
            Serial.println(F("[SLEEP] awakening (movement)"));
          } else {
            if (burstPct >= 3) turns++;         // brief stir (~2 s), still asleep
            vQuietRun = vQuiet ? vQuietRun + 1 : 0;
            if (stage == 1) {
              bool hrLow = epHR > 0 && hrBase > 0 &&
                           epHR <= hrBase - max(3, hrBase/20);
              if (vQuietRun >= DEEP_AFTER_EP && (hrLow || hrBase==0 || epHR==0))
                stage = 0;
              lightMin++;
            } else {                            // deep
              bool hrUp = epHR > 0 && hrBase > 0 && epHR >= hrBase - 1;
              if (!vQuiet || hrUp) stage = 1;
              deepMin++;
            }
            if (epHR > 0) { sesHR += epHR; sesHRn++; }
            if (epBR > 0) { sesBR += epBR; sesBRn++; }
          }
        }
      }
      if (oobRun >= OOB_END_EP) {               // ---- auto end ----
        if (bedMin - oobRun >= AUTO_SAVE_MIN && deepMin + lightMin >= 1) {
          buildReport(outRep, oobRun); repReady = true;
        } else Serial.println(F("[SLEEP] session too short, discarded"));
        resetSession();
      }
    }
    epStart = now; nSamp = 0; sumRange = 0; bursts = 0;
    presCnt = bedCnt = 0; sumHR = sumBR = 0; nHR = nBR = 0;
  }

  // ---- manual end (dashboard "End session" button) ----
  if (g_endSession) {
    g_endSession = false;
    if (sOn && bedMin >= MANUAL_SAVE_MIN) {
      buildReport(outRep, oobRun); repReady = true;
      Serial.println(F("[SLEEP] session ended by user, report built"));
    } else if (sOn) Serial.println(F("[SLEEP] session under 3 min, discarded"));
    resetSession();
  }

  outLive.active  = sOn;     outLive.stage    = stage;
  outLive.bedMin  = bedMin;  outLive.deepMin  = deepMin;
  outLive.lightMin= lightMin;outLive.awakeMin = awakeMin;
  outLive.sleepMin= deepMin + lightMin;
  outLive.onsetMin= onset;   outLive.wakes    = wakes;  outLive.turns = turns;
  outLive.score   = scoreOf(bedMin, deepMin + lightMin, deepMin, wakes);
  memcpy(outLive.since, sinceClk, 6);
}
