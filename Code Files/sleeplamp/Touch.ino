// SleepLamp · Touch.ino — TTP223 capacitive touch button.
// Polled from loop() (core 1, same context as the lamp/alarm), edge-detected
// and debounced. One tap:
//   • alarm ringing  -> dismiss the alarm
//   • otherwise      -> toggle the lamp (off  <->  warm manual light)
#include "types.h"
#include "config.h"

#if USE_TOUCH
static const int TOUCH_ON_LEVEL = TOUCH_ACTIVE_HIGH ? HIGH : LOW;

void touchBegin() {
  pinMode(TOUCH_PIN, TOUCH_ACTIVE_HIGH ? INPUT : INPUT_PULLUP);
  Serial.printf("[TOUCH] TTP223 on GPIO%d — tap toggles the lamp\n", TOUCH_PIN);
}

// turn the lamp on (warm) if it's off, else turn it off
static void toggleLamp() {
  xSemaphoreTake(mux, portMAX_DELAY);
  if (light.mode == 2) {                 // was off -> warm manual light
    light.mode = 1; light.r = 255; light.g = 150; light.b = 60;
    if (light.bright < 25) light.bright = 60;
  } else {                               // auto or manual -> off
    light.mode = 2;
  }
  int mode = light.mode;
  xSemaphoreGive(mux);
  lightApply();
  settingsSaveLight();
  Serial.printf("[TOUCH] tap -> lamp %s\n", mode == 2 ? "OFF" : "ON (warm)");
}

void handleTouch() {
  static int      last     = -1;
  static uint32_t lastEdge = 0;
  int v = digitalRead(TOUCH_PIN);
  if (last < 0) { last = v; return; }                 // first call: capture idle level

  // act on the press edge (idle -> touched), with debounce
  if (v == TOUCH_ON_LEVEL && last != TOUCH_ON_LEVEL &&
      millis() - lastEdge > TOUCH_DEBOUNCE_MS) {
    lastEdge = millis();
    bool firing;
    xSemaphoreTake(mux, portMAX_DELAY); firing = alarmCfg.firing; xSemaphoreGive(mux);
    if (firing) { g_alarmStop = true; Serial.println(F("[TOUCH] tap -> alarm dismissed")); }
    else        { toggleLamp(); }
  }
  last = v;
}
#else
void touchBegin() {}
void handleTouch() {}
#endif
