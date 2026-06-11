// SleepLamp · Env.ino — DHT11 temperature + humidity (bit-banged, no library)
#include "types.h"
#include "config.h"

#if USE_DHT
static bool waitLevel(uint8_t pin, uint8_t level, uint32_t to_us) {
  uint32_t s = micros();
  while (digitalRead(pin) != level) if (micros() - s > to_us) return false;
  return true;
}
static bool readDHT11(uint8_t pin, int &temp, int &hum) {
  uint8_t bits[5] = {0,0,0,0,0};
  pinMode(pin, OUTPUT); digitalWrite(pin, LOW); delay(20);
  noInterrupts();
  digitalWrite(pin, HIGH); delayMicroseconds(30);
  pinMode(pin, INPUT_PULLUP);
  bool ok = waitLevel(pin, LOW, 100) && waitLevel(pin, HIGH, 100) && waitLevel(pin, LOW, 100);
  if (ok) for (int i = 0; i < 40 && ok; i++) {
    if (!waitLevel(pin, HIGH, 80)) { ok = false; break; }
    uint32_t t0 = micros();
    if (!waitLevel(pin, LOW, 100)) { ok = false; break; }
    bits[i/8] <<= 1; if (micros() - t0 > 45) bits[i/8] |= 1;
  }
  interrupts();
  if (!ok) return false;
  if (((bits[0]+bits[1]+bits[2]+bits[3]) & 0xFF) != bits[4]) return false;
  hum = bits[0]; temp = bits[2];
  return true;
}
#endif

void envTask(void*) {
  uint32_t lastDht = 0;
  for (;;) {
#if USE_DHT
    if (millis() - lastDht > 3000) {                 // DHT11 is slow: every 3 s
      lastDht = millis(); int t, h;
      if (readDHT11(DHT_PIN, t, h)) {
        xSemaphoreTake(mux, portMAX_DELAY); g.tempC = t; g.humidity = h; g.envValid = true; xSemaphoreGive(mux);
      }
    }
#endif
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
