/*
 * Sleepal Core — clean, robust C1001 vitals reader (ESP32-S3)
 * Wiring:  C1001 TX -> GPIO18 (RX) , RX -> GPIO17 (TX) , VCC -> external 5V , GND common
 *
 * What it does well:
 *   - validates every reading (drops corrupt frames like presence=15 / resp=255)
 *   - smooths heart rate & respiration so they stay steady (no 74<->120 jumping)
 *   - clear LOCK status so you know when the numbers are trustworthy
 *   - no WiFi / no heavy reads -> never hangs
 */
#include "DFRobot_HumanDetection.h"

#define RADAR_RX 18
#define RADAR_TX 17

DFRobot_HumanDetection hu(&Serial1);

// ---------- small rolling-average filter (with outlier rejection) ----------
struct Smoother {
  int buf[6]; int n = 0, idx = 0, miss = 0;
  void push(int v) { buf[idx] = v; idx = (idx + 1) % 6; if (n < 6) n++; miss = 0; }
  void noData()    { if (++miss >= 4) { n = 0; idx = 0; } }   // clear after 4 bad cycles
  int  value()     { if (!n) return 0; long s = 0; for (int i = 0; i < n; i++) s += buf[i]; return (int)(s / n); }
  bool locked()    { return n >= 3; }
};
Smoother hrF, brF;

const char* motionTxt(int v){ switch(v){case 0:return "None";case 1:return "Still";case 2:return "Active";} return "-"; }
const char* breathTxt(int v){ switch(v){case 1:return "Normal";case 2:return "Fast";case 3:return "Slow";case 4:return "None";} return "-"; }

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RADAR_RX, RADAR_TX);

  Serial.println("\nSleepal Core - initializing radar...");
  while (hu.begin() != 0)                        { Serial.println("  init error, retrying"); delay(800); }
  while (hu.configWorkMode(hu.eSleepMode) != 0)  { Serial.println("  mode set failed");      delay(800); }
  hu.configLEDLight(hu.eHPLed, 1);
  hu.sensorRet();
  Serial.println("Ready. Sit/lie STILL, chest ~0.7 m facing the sensor.\n");
}

void loop() {
  // --- raw reads ---
  int presence = hu.smHumanData(hu.eHumanPresence);
  int motion   = hu.smHumanData(hu.eHumanMovement);
  int range    = hu.smHumanData(hu.eHumanMovingRange);
  int bs        = hu.getBreatheState();
  int rawHR     = hu.getHeartRate();
  int rawBR     = hu.getBreatheValue();

  // --- validate (drop corrupt values) ---
  if (presence != 0 && presence != 1) presence = (presence == 1);   // anything weird -> treat as not-clean
  bool pres = (presence == 1);
  if (motion < 0 || motion > 2) motion = 0;
  if (range  < 0 || range > 100) range = 0;
  if (bs < 1 || bs > 4) bs = 4;

  // heart rate valid window 40..180 ; respiration 6..30
  if (rawHR >= 40 && rawHR <= 180) hrF.push(rawHR); else hrF.noData();
  if (rawBR >=  6 && rawBR <=  30) brF.push(rawBR); else brF.noData();

  int hr = hrF.value();
  int br = brF.value();
  bool lock = hrF.locked();

  // --- clean output ---
  if (!pres) {
    Serial.println("No one present  -  searching...");
  } else if (lock) {
    Serial.printf("LOCKED   HR %3d bpm   resp %2d rpm   (breath %-6s, motion %s)\n",
                  hr, br, breathTxt(bs), motionTxt(motion));
  } else {
    Serial.printf("present  -  locking... hold still   (motion %s, range %d, breath %s)\n",
                  motionTxt(motion), range, breathTxt(bs));
  }

  delay(1000);
}
