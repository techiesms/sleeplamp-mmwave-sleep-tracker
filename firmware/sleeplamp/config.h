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

// ---------------- RGB LED (COMMON CATHODE) ----------------
#define USE_RGB        1
#define RGB_R_PIN      5
#define RGB_G_PIN      6
#define RGB_B_PIN      7
#define RGB_COMMON_CATHODE 1       // 1 = common cathode (GND), 0 = common anode (3V3)
#define RGB_BOOT_TEST  1           // cycle R->G->B at boot to verify wiring

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
#define MANUAL_SAVE_MIN    3   // "End session" button saves if in bed >= this

// ---------------- history ----------------
#define HIST_MAX        60     // sessions kept on flash (oldest dropped first)

// ---------------- misc ----------------
#define FW_VERSION      "1.2"
#define POLL_DELAY_MS   120
#define SERIAL_PRINT_MS 2000
