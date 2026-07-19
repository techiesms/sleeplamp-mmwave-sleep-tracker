// SleepLamp · config.h — all settings + pin map (ESP32-S3, arduino-esp32 core 3.x)
#pragma once

// ---------------- WiFi / network ----------------
// Real credentials live in secrets.h, which is git-ignored so your WiFi
// password never reaches GitHub. Copy secrets.example.h -> secrets.h and fill
// it in. (You can also set WiFi later from the dashboard's /wifi portal — that
// value is stored in NVS and overrides these compile-time defaults.)
#if __has_include("secrets.h")
  #include "secrets.h"
#endif
#ifndef WIFI_SSID
  #define WIFI_SSID  "YOUR_WIFI_NAME"
#endif
#ifndef WIFI_PASS
  #define WIFI_PASS  "YOUR_WIFI_PASSWORD"
#endif
#define MDNS_HOST  "sleeplamp"     // -> http://sleeplamp.local

// ---------------- C1001 radar (UART) ----------------
#define RADAR_RX   18              // ESP32-S3 RX <- C1001 TX
#define RADAR_TX   17              // ESP32-S3 TX -> C1001 RX

// ---------------- DHT11 (temp/humidity) ----------------
#define USE_DHT    1
#define DHT_PIN    4               // DATA (VCC=3V3, GND=GND)

// ---------------- TTP223 capacitive touch button ----------------
// Module: VCC->3V3, GND->GND, OUT/SIG->TOUCH_PIN. Default TTP223 mode is
// momentary + active-HIGH (OUT goes high while touched) — that's what we expect.
// Tap = toggle lamp on/off; a tap while the alarm rings dismisses it.
#define USE_TOUCH       1
#define TOUCH_PIN       6          // TTP223 OUT
#define TOUCH_ACTIVE_HIGH 1        // 1 = OUT high on touch (TTP223 default); 0 = active low
#define TOUCH_DEBOUNCE_MS 250      // ignore re-triggers within this window

// ---------------- RGB lamp: NeoPixel ring (WS2812 / SK6812) ----------------
// Round 12-LED addressable ring. ONE data wire. Library: Adafruit NeoPixel
// (Library Manager -> "Adafruit NeoPixel", use 1.12.3 or newer; tested 1.15.5).
// Power the ring's 5V + GND from the EXTERNAL 5V supply (12 px ~= up to 0.7 A at
// full white) — never from the ESP's 3V3. DIN goes to NEOPIXEL_PIN.
#define USE_RGB         1
#define NEOPIXEL_PIN    5          // ring DIN  (change if you wired another GPIO)
#define NEOPIXEL_COUNT  12         // pixels on the ring
#define RGB_BOOT_TEST   1          // sweep R->G->B at boot to verify wiring/colour order

// ---------------- Time / smart-wake alarm ----------------
#define TZ_OFFSET_SEC   19800      // India = GMT+5:30
#define DST_OFFSET_SEC  0
#define NTP_SERVER      "pool.ntp.org"
#define ALARM_WINDOW_MIN 30        // wake during light sleep within this window before set time
#define SUNRISE_SEC      300       // sunrise ramp length (5 min)

// ---------------- sleep engine (on-device staging) ----------------
// The radar's internal staging needs 15-20+ min before it reports anything;
// this engine stages every minute from movement + vitals, so naps work too.
#define SLEEP_EPOCH_SEC   60   // one staging decision per minute
#define ONSET_QUIET_EP     3   // quiet minutes in bed -> counted as asleep
#define DEEP_AFTER_EP     10   // very-quiet asleep minutes -> deep sleep
#define QUIET_RANGE        8   // mean movement <= this = quiet minute
#define WAKE_RANGE        14   // mean movement >= this while asleep = awakening
#define BURST_RANGE       30   // single-sample movement spike
#define OOB_END_EP         8   // minutes out of bed/zone -> session auto-ends
#define AUTO_SAVE_MIN     15   // auto-ended sessions saved if in bed >= this
#define MANUAL_SAVE_MIN    1   // "End session" button: explicit press always saves
                               // (>= 1 min). Was 3 — that silently discarded short
                               // test sessions, which looked like "history saves once".

// ---------------- history ----------------
#define HIST_MAX        60     // sessions kept on flash (oldest dropped first)

// ---------------- Matter (smart-home pairing) ----------------
// Needs arduino-esp32 core >= 3.1.0, the "Huge APP" partition (already set), and
// "Erase All Flash Before Sketch Upload = Enabled" on the Matter upload (this
// wipes saved history + WiFi, so set WiFi again afterwards). The lamp joins
// Apple Home / Google / Alexa as a Matter Color Light; scan the QR printed to
// the serial monitor at boot to pair. Set to 0 to build without Matter.
#define ENABLE_MATTER   1

// ---------------- misc ----------------
#define FW_VERSION      "1.2"
#define POLL_DELAY_MS   120
#define SERIAL_PRINT_MS 2000
