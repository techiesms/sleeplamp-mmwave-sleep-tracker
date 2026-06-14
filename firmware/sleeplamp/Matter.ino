// SleepLamp · Matter.ino — exposes the lamp as a Matter Color Light so it can be
// added to Apple Home / Google Home / Alexa and controlled alongside the web
// dashboard, touch button and the adaptive sleep logic.
//
// Requires arduino-esp32 core >= 3.1.0 (set ENABLE_MATTER 0 in config.h to skip).
//
// Two-way sync, kept thread-safe:
//   hub  -> lamp : onMatterChange() (Matter task) only updates LightState +
//                  raises g_lightDirty; loop() applies it (single-threaded ring).
//   lamp -> hub  : matterReflect() is called from writeRGB() (loop task) on every
//                  colour change — touch, web, auto and sunrise all flow through
//                  it — and pushes to the hub only when the colour actually changes.
//
// Pairing: when the device isn't commissioned yet, the CHIP stack prints a
// scannable ASCII QR to the boot log; we also print the manual code + QR URL.
#include "types.h"
#include "config.h"

#if ENABLE_MATTER
#include <Matter.h>
#include <WiFi.h>

static MatterColorLight matterLight;
static bool matterReady = false;

// hub -> lamp : a smart-home app changed our light
static bool onMatterChange(bool state, espHsvColor_t hsv) {
  espRgbColor_t rgb = espHsvColorToRgbColor(hsv);
  xSemaphoreTake(mux, portMAX_DELAY);
  if (!state) {
    light.mode = 2;                         // off
  } else {
    light.mode = 1;                         // manual colour
    light.r = rgb.r; light.g = rgb.g; light.b = rgb.b;
    light.bright = map(hsv.v, 0, 254, 0, 100);
    if (light.bright < 1) light.bright = 1;
  }
  xSemaphoreGive(mux);
  g_lightDirty = true;                       // loop() applies + persists it
  return true;
}

void matterBegin() {
  matterLight.onChange(onMatterChange);
  matterLight.begin();                       // starts off; the hub re-syncs on pairing
  Matter.begin();
  matterReady = true;

  if (!Matter.isDeviceCommissioned()) {
    Serial.println(F("\n========================= MATTER PAIRING ========================="));
    Serial.println(F("[MATTER] Add \"SleepLamp\" in Apple Home / Google Home / Alexa."));
    Serial.println(F("[MATTER] Scan the QR code in the boot log above, or use:"));
    Serial.printf ("[MATTER]   Manual pairing code : %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf ("[MATTER]   QR code (open/scan)  : %s\n", Matter.getOnboardingQRCodeUrl().c_str());
    Serial.println(F("=================================================================\n"));
  } else {
    Serial.println(F("[MATTER] already paired — connected to your smart-home fabric"));
  }
}

void matterLoop() {
  if (!matterReady) return;
  static uint32_t tNag = 0;
  static bool wasCommissioned = false;
  bool commissioned = Matter.isDeviceCommissioned();
  if (!commissioned) {
    if (millis() - tNag > 30000) {           // gentle reminder while unpaired
      tNag = millis();
      Serial.printf("[MATTER] waiting to be paired — code %s | %s\n",
                    Matter.getManualPairingCode().c_str(),
                    Matter.getOnboardingQRCodeUrl().c_str());
    }
  } else if (!wasCommissioned) {
    wasCommissioned = true;
    Serial.println(F("[MATTER] paired! the lamp is now in your smart-home app"));
  }
}

// lamp -> hub : called from writeRGB() on every ring colour change
void matterReflect(int r, int g, int b) {
  if (!matterReady) return;
  static int lr = -1, lg = -1, lb = -1;
  if (r == lr && g == lg && b == lb) return;  // dedupe — no spam to the fabric
  lr = r; lg = g; lb = b;
  bool on = (r | g | b) != 0;
  matterLight.setOnOff(on);
  if (on) { espRgbColor_t c = {(uint8_t)r, (uint8_t)g, (uint8_t)b}; matterLight.setColorRGB(c); }
}

#else   // ----- Matter disabled: no-op stubs so the rest of the code is unchanged -----
void matterBegin()   {}
void matterLoop()    {}
void matterReflect(int, int, int) {}
#endif
