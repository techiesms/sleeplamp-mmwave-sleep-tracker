# Sleepal AI Lamp — Product Teardown / What We're Cloning

Sources:
- Kickstarter: https://www.kickstarter.com/projects/sleepal/sleepal-worlds-first-contactless-ai-sleep-tracking-lamp-0
- Product site: https://sleepal.ai/ and https://lamp.sleepal.ai/
- HomeKit News (Matter detail): https://homekitnews.com/2026/04/01/sleepal-unveils-sleep-tracking-matter-lamp-ahead-of-kickstarter-launch/
- Android Headlines: https://www.androidheadlines.com/2026/03/sleepal-ai-lamp-introduces-contactless-ai-sleep-monitoring-in-a-bedside-lamp.html

## The pitch in one line
A bedside **lamp** that tracks your sleep **contactlessly** (no wearable, no
camera) using **mmWave radar**, then uses **adaptive light + sound** to help you
fall asleep and wake up at the right point in your sleep cycle.

## Feature breakdown → which subsystem provides it

| Sleepal feature | How they do it | Our component |
|---|---|---|
| Contactless sleep staging (deep/light/awake) | mmWave radar | **C1001** `eSleepState` |
| Respiratory rate (claims RMSE < 2 vs medical) | mmWave | C1001 `getBreatheValue` |
| Heart rate / micro-motion | mmWave | C1001 `getHeartRate` |
| Presence / in-bed / out-of-bed auto detect | mmWave | C1001 `eInOrNotInBed` |
| Sleep position & turnovers | mmWave | C1001 turnover/bodyMove |
| **Snoring detection & frequency** | **microphone + audio AI** | **I2S MEMS mic + ESP32-S3** |
| Room light / noise / temp / humidity | env sensors | BH1750 + BME280 (+ mic) |
| Full-spectrum Ra95+, zero-blue-light lamp | tunable LED | **WS2812/SK6812 or 5-ch LED driver** |
| Adaptive dimming (fades when asleep) | firmware logic | ESP32-S3 + radar trigger |
| Night-light on wake/movement | firmware + radar | ESP32-S3 |
| Sunrise simulation wake-up | LED ramp | ESP32-S3 + RTC |
| **Smart light-sleep alarm** (wake in light phase) | radar + alarm window | C1001 stage + firmware |
| Wind-down: breathing guide, meditation, white noise | speaker | **I2S DAC + amp + speaker** |
| Turn-to-snooze / stand-to-stop | radar/IMU | radar presence (+ optional IMU) |
| Companion app + sleep profile | cloud/app | Wi-Fi/BLE + app/dashboard |
| **Matter** smart-home integration | Matter-over-WiFi | ESP32-S3 (Matter SDK) |
| Privacy (no camera, data on-device, mic kill switch) | design choice | hardware mic-mute switch |

## Key takeaways for our build
1. **One sensor (C1001) covers ~70% of the feature list.** The radar is the
   moat. Everything else is "smart lamp" engineering we fully control.
2. The **only sensing gap vs. Sleepal is snoring**, which needs a microphone +
   a little on-device audio classification. ESP32-S3 is specifically good at
   this (vector DSP / ESP-DL / ESP-SR).
3. **Matter support = they're on ESP32 (or similar Wi-Fi SoC).** This validates
   our ESP32-S3 choice — Espressif is the reference platform for Matter.
4. Their differentiators are **UX and algorithms**, not exotic hardware:
   - choosing the *right moment* in the sleep cycle to wake you,
   - smooth circadian light curves (warm/dim at night, sunrise at wake),
   - a calm wind-down (audio), and a polished app.
   These are firmware/app problems — achievable by us.
5. Privacy framing ("no camera, mic kill switch, on-device") is a selling point
   we should copy: add a **physical mic-mute switch** to the BOM.

## MVP vs. full clone
- **MVP (prove the magic):** C1001 + ESP32-S3 + tunable LED. Read sleep stage,
  HR, breathing → drive sunrise alarm that fires during a light-sleep window,
  auto-dim when asleep, night-light on out-of-bed. No app needed (serial/web UI).
- **V1 (Sleepal parity):** add I2S mic (snore), I2S speaker (white noise /
  breathing coach), BME280+BH1750 (environment), RTC, Wi-Fi app + Matter.
