/*
 * C1001 mmWave Human Detection - basic demo (presence, respiration, heart rate)
 * Board: ESP32 Dev Module (WROOM-32)
 *
 * Wiring (WROOM-32):
 *   C1001 VCC -> 5V       GND -> GND
 *   C1001 TX  -> GPIO16   (ESP32 RX)
 *   C1001 RX  -> GPIO17   (ESP32 TX)
 *
 * Note: the DFRobot example uses D3/D2 which do not exist on a plain ESP32,
 *       so we use real GPIO numbers 16 (RX) and 17 (TX) instead.
 */

#include "DFRobot_HumanDetection.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define RADAR_RX 16   // ESP32 RX <- C1001 TX
#define RADAR_TX 17   // ESP32 TX -> C1001 RX

DFRobot_HumanDetection hu(&Serial1);

void setup() {
  // --- power-stability tweaks (when the radar is powered from the board's 5V) ---
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);   // disable brown-out detector (no reset on brief 5V dips)
  setCpuFrequencyMhz(80);                       // lower CPU current draw -> more headroom for the radar

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RADAR_RX, RADAR_TX);

  Serial.println("Start initialization");
  while (hu.begin() != 0) {
    Serial.println("init error!!!");
    delay(1000);
  }
  Serial.println("Initialization successful");

  Serial.println("Start switching work mode");
  while (hu.configWorkMode(hu.eSleepMode) != 0) {
    Serial.println("error!!!");
    delay(1000);
  }
  Serial.println("Work mode switch successful");

  Serial.print("Current work mode:");
  switch (hu.getWorkMode()) {
    case 1:
      Serial.println("Fall detection mode");
      break;
    case 2:
      Serial.println("Sleep detection mode");
      break;
    default:
      Serial.println("Read error");
  }

  hu.configLEDLight(hu.eHPLed, 1);  // sensor onboard LED off
  hu.sensorRet();                   // reset after config, required

  Serial.print("HP LED status:");
  switch (hu.getLEDLightState(hu.eHPLed)) {
    case 0:
      Serial.println("Off");
      break;
    case 1:
      Serial.println("On");
      break;
    default:
      Serial.println("Read error");
  }

  Serial.println();
  Serial.println();
}

void loop() {
  Serial.print("Existing information:");
  switch (hu.smHumanData(hu.eHumanPresence)) {
    case 0:
      Serial.println("No one is present");
      break;
    case 1:
      Serial.println("Someone is present");
      break;
    default:
      Serial.println("Read error");
  }

  Serial.print("Motion information:");
  switch (hu.smHumanData(hu.eHumanMovement)) {
    case 0:
      Serial.println("None");
      break;
    case 1:
      Serial.println("Still");
      break;
    case 2:
      Serial.println("Active");
      break;
    default:
      Serial.println("Read error");
  }

  Serial.print("Body movement parameters: ");
  Serial.println(hu.smHumanData(hu.eHumanMovingRange));
  Serial.print("Respiration rate: ");
  Serial.println(hu.getBreatheValue());
  Serial.print("Heart rate: ");
  Serial.println(hu.getHeartRate());
  Serial.println("-----------------------");
  delay(1000);
}
