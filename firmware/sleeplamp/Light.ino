// SleepLamp · Light.ino — NeoPixel ring lamp (WS2812/SK6812) + adaptive + sunrise.
// The whole ring shows one colour (a lamp, not an animation). All output goes
// through writeRGB()/writeScaled(), so the adaptive + sunrise logic is unchanged
// from the old single-LED version — only the driver underneath is different.
//
// Library: Adafruit NeoPixel (>= 1.12.3; uses the ESP32-S3 RMT peripheral, which
// is free here — the radar is on UART). All calls run from loop()/core 1.
#include "types.h"
#include "config.h"
#include <Adafruit_NeoPixel.h>

#if USE_RGB
static Adafruit_NeoPixel ring(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

// paint the entire ring one colour (raw 0..255 per channel)
static void writeRGB(int r, int g, int b) {
#if USE_RGB
  r = constrain(r, 0, 255); g = constrain(g, 0, 255); b = constrain(b, 0, 255);
  uint32_t c = ring.Color(r, g, b);
  for (int i = 0; i < NEOPIXEL_COUNT; i++) ring.setPixelColor(i, c);
  ring.show();
  matterReflect(r, g, b);          // keep the smart-home app in sync (no-op if Matter off)
#endif
}
static void writeScaled(int r, int g, int b, int bright) {
  writeRGB(r * bright / 100, g * bright / 100, b * bright / 100);
}

void lightBegin() {
#if USE_RGB
  ring.begin();
  ring.setBrightness(255);      // we scale colours ourselves in writeScaled()
  ring.clear();
  ring.show();
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

// boot wiring check: shows pure R, G, B so you can verify DIN + colour order
void lightBootTest() {
#if USE_RGB && RGB_BOOT_TEST
  Serial.println(F("[LIGHT] boot test -> RED, GREEN, BLUE (0.8s each) on the ring."));
  Serial.println(F("        Colours wrong order? change NEO_GRB in Light.ino. Nothing lights? check DIN pin / 5V."));
  writeRGB(255,0,0); Serial.println(F("  RED"));   delay(800);
  writeRGB(0,255,0); Serial.println(F("  GREEN")); delay(800);
  writeRGB(0,0,255); Serial.println(F("  BLUE"));  delay(800);
  writeRGB(0,0,0);   Serial.println(F("  OFF"));   delay(300);
#endif
}
