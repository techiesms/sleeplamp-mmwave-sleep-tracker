// SleepLamp - document head + opening <style> (PROGMEM, streamed by handleRoot in WebUI.ino)
#pragma once
#include <Arduino.h>

const char PAGE_HEAD[] PROGMEM = R"SLHEAD(
<!DOCTYPE html><html lang="en"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#020308">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<meta name="apple-mobile-web-app-title" content="SleepLamp">
<link rel="manifest" href="/manifest.json">
<link rel="icon" href="data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 32 32'><circle cx='16' cy='16' r='13' fill='%237d7aff'/></svg>">
<title>SleepLamp</title>
<style>)SLHEAD";
