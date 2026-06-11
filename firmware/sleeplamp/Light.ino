// SleepLamp · Light.ino — common-cathode RGB lamp (3.x LEDC API) + adaptive + sunrise
#include "types.h"
#include "config.h"

// raw 0..255 per channel (handles common-cathode vs anode)
static void writeRGB(int r, int g, int b) {
#if USE_RGB
  r = constrain(r,0,255); g = constrain(g,0,255); b = constrain(b,0,255);
  if (!RGB_COMMON_CATHODE) { r = 255 - r; g = 255 - g; b = 255 - b; }
  ledcWrite(RGB_R_PIN, r);            // 3.x: address by PIN, not channel
  ledcWrite(RGB_G_PIN, g);
  ledcWrite(RGB_B_PIN, b);
#endif
}
static void writeScaled(int r, int g, int b, int bright) {
  writeRGB(r * bright / 100, g * bright / 100, b * bright / 100);
}

void lightBegin() {
#if USE_RGB
  ledcAttach(RGB_R_PIN, 5000, 8);    // 3.x: ledcAttach(pin, freq, resolution)
  ledcAttach(RGB_G_PIN, 5000, 8);
  ledcAttach(RGB_B_PIN, 5000, 8);
  writeRGB(0,0,0);
#endif
}

// apply manual/off immediately (auto handled by lightAuto / alarm by lightSunrise)
void lightApply() {
  if (light.mode == 2)       writeRGB(0,0,0);
  else if (light.mode == 1)  writeScaled(light.r, light.g, light.b, light.bright);
}

// adaptive: colour from presence + OUR sleep engine (responds from minute 1)
void lightAuto() {
  if (light.mode != 0) return;
  SensorData d; SleepLive lv;
  xSemaphoreTake(mux, portMAX_DELAY); d = g; lv = live; xSemaphoreGive(mux);
  int r, gg, b, br = light.bright;
  if (!d.presence) {
    // night-light: you left bed mid-session (3am bathroom trip) -> dim amber
    // guide light. lv.active stays true ~8 min after exit, then lamp goes off.
    if (lv.active) { r=255; gg=50;  b=0;  br=min(br,6); }
    else           { r=0;   gg=0;   b=0; }
  }
  else if (lv.active && (lv.stage==0||lv.stage==1))  { r=0;   gg=0;   b=0; }       // asleep -> dark
  else if (lv.active && lv.stage==2)                 { r=255; gg=80;  b=10; br=min(br,12); } // in bed awake
  else                                               { r=255; gg=150; b=60; }      // up -> warm
  writeScaled(r, gg, b, br);
}

// sunrise ramp for the alarm. p = 0..1 (amber/dim -> bright warm-white)
void lightSunrise(float p) {
  p = constrain(p, 0.0f, 1.0f);
  int r = 255;
  int g = (int)(40 + p * 175);     // 40 -> 215
  int b = (int)(0  + p * 130);     // 0  -> 130
  int br = (int)(3 + p * 97);      // 3% -> 100%
  writeScaled(r, g, b, br);
}

// boot wiring check: shows pure R, G, B so you can verify pins + cathode/anode
void lightBootTest() {
#if USE_RGB && RGB_BOOT_TEST
  Serial.println(F("[LIGHT] boot test -> RED, GREEN, BLUE (0.8s each)."));
  Serial.println(F("        Wrong colour order? swap R/G/B pins. LED ON when it says OFF? set RGB_COMMON_CATHODE 0."));
  writeRGB(255,0,0); Serial.println(F("  RED"));   delay(800);
  writeRGB(0,255,0); Serial.println(F("  GREEN")); delay(800);
  writeRGB(0,0,255); Serial.println(F("  BLUE"));  delay(800);
  writeRGB(0,0,0);   Serial.println(F("  OFF"));   delay(300);
#endif
}
