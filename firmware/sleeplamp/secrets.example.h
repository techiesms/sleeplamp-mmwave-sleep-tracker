// SleepLamp · secrets.example.h — TEMPLATE (this one IS committed).
//
// Setup:
//   1. Copy this file to "secrets.h" in the same folder.
//   2. Put your real WiFi name + password in secrets.h.
//   3. secrets.h is git-ignored, so your password stays off GitHub.
//
// Don't have your WiFi yet? Skip this — flash as-is, the lamp starts a
// "SleepLamp-Setup" hotspot (password "sleeplamp123"); join it and open
// http://192.168.4.1/wifi to enter your network. It's saved on the device.
#pragma once

#define WIFI_SSID  "YOUR_WIFI_NAME"
#define WIFI_PASS  "YOUR_WIFI_PASSWORD"
