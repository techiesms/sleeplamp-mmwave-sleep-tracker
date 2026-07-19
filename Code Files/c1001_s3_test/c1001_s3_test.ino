/*
 * C1001 simple test for ESP32-S3 (same wiring as the dashboard)
 *   C1001 TX -> GPIO18 (RX) , C1001 RX -> GPIO17 (TX) , VCC=5V , GND=GND
 *
 * Only reads vitals -> clean Serial @115200. No WiFi, no web, no extra reads.
 * Use this to confirm Heart rate / Respiration work (it isolates the sensor
 * from the full dashboard).
 */
#include "DFRobot_HumanDetection.h"

#define RADAR_RX 18
#define RADAR_TX 17

DFRobot_HumanDetection hu(&Serial1);

const char* breathTxt(int v){ switch(v){case 1:return "Normal";case 2:return "Fast";case 3:return "Slow";case 4:return "None";} return "-"; }
const char* motionTxt(int v){ switch(v){case 0:return "None";case 1:return "Still";case 2:return "Active";} return "-"; }

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RADAR_RX, RADAR_TX);

  Serial.println("Init...");
  while (hu.begin() != 0) { Serial.println("init error, retry"); delay(800); }
  while (hu.configWorkMode(hu.eSleepMode) != 0) { Serial.println("mode fail, retry"); delay(800); }
  hu.configLEDLight(hu.eHPLed, 1);
  hu.sensorRet();
  Serial.println("Sleep mode active. Sit/lie STILL, chest ~0.7m facing sensor, wait 1-2 min.");
  Serial.println();
}

void loop() {
  int presence  = hu.smHumanData(hu.eHumanPresence);
  int motion    = hu.smHumanData(hu.eHumanMovement);
  int range     = hu.smHumanData(hu.eHumanMovingRange);
  int hr        = hu.getHeartRate();
  int br        = hu.getBreatheValue();
  int bs        = hu.getBreatheState();

  Serial.printf("presence:%d  motion:%-6s  range:%-3d  breathState:%-7s  resp:%-3d rpm  HR:%-3d bpm\n",
                presence, motionTxt(motion), range, breathTxt(bs), br, hr);
  delay(1000);
}
