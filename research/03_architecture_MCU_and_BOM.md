# System Architecture, MCU Choice & Bill of Materials

## 1. Is the ESP32-S3 enough? — YES (it's the right pick)

The C1001 does all radar DSP internally and talks plain UART, so the MCU's job
is: read one UART, drive LEDs, play/record I2S audio, read I2C env sensors, run
Wi-Fi/BLE/Matter, and host a small app/API. The ESP32-S3 is built for exactly
this.

| Requirement | ESP32-S3 capability | Verdict |
|---|---|---|
| Read C1001 @115200 | 3× hardware UART | ✅ trivial (use UART1) |
| Drive addressable LED (sunrise) | RMT + LCD/I2S peripheral, libs for WS2812/SK6812 | ✅ |
| Play white noise / meditation | 2× I2S, plays WAV/MP3 from flash/SD | ✅ |
| Snore detection (mic) | I2S PDM mic input + **vector DSP (ESP-DL/ESP-SR)** | ✅ S3-specific strength |
| Env sensors (light/temp/RH) | 2× I2C | ✅ |
| Wi-Fi app + **Matter** | Wi-Fi 4 + BLE 5, official Matter SDK | ✅ (Espressif = Matter reference) |
| RAM for audio buffers / TLS | up to **8 MB PSRAM**, 512 KB SRAM | ✅ pick an N16R8 module |
| Flash for sounds/OTA | up to 16 MB; or add SD | ✅ |
| GPIO count | ~36 usable | ✅ plenty |

**Recommended part:** ESP32-S3-WROOM-1 **N16R8** (16 MB flash, 8 MB PSRAM).
For prototyping: ESP32-S3-DevKitC-1 N16R8. The R8 PSRAM matters for audio
playback buffers + TLS + Matter at the same time.

### When would S3 NOT be enough? (and the fallbacks)
- **If you want true on-device audio AI / voice assistant + Matter + audio all
  at once** and run out of headroom → step up to **ESP32-P4** (dual RISC-V
  400 MHz, much stronger DSP/AI, but Wi-Fi via co-processor — more complex) for
  compute, *or* keep S3 and offload snore ML to the cloud/app.
- **If you don't need Wi-Fi/Matter at all** (pure standalone lamp) → an
  RP2040/STM32 could drive it, but you'd lose the app + Matter story. Not worth
  it; S3 gives the most product for least effort.
- **Bottom line:** **ESP32-S3-WROOM-1 N16R8 is the recommended MCU.** Only move
  to ESP32-P4 (as a co-processor or main) if heavy local audio ML becomes a
  hard requirement. No reason to go smaller.

## 2. Block diagram

```
                         ┌───────────────────────────┐
        5V PSU ──────────┤  Buck/LDO  5V→3.3V         │
                         └───────────────────────────┘
                                     │ 3.3V / 5V rails
   ┌──────────────┐  UART1 115200    │
   │  C1001 mmWave │◄────────────────┤   ESP32-S3-WROOM-1 (N16R8)
   │  (SEN0623)    │  RX/TX, 5V,GND   │   - reads sleep/HR/breath
   └──────────────┘                  │   - Wi-Fi/BLE/Matter
                                     │
   I2C: BH1750 (lux) ───────────────┤   ┌──────────────────────────────┐
        BME280 (temp/RH/press) ─────┤   │ RMT → WS2812/SK6812 RGBW ring │ (light)
                                     │   └──────────────────────────────┘
   I2S in: PDM/MEMS mic ◄───────────┤   ┌──────────────────────────────┐
        (snore detection)           │   │ I2S out → MAX98357A → speaker │ (audio)
                                     │   └──────────────────────────────┘
   GPIO: rotary encoder/touch, ─────┤
         physical MIC-MUTE switch,  │
         status RGB, RTC (DS3231)   │
                                     └──── USB-C (power + flashing + serial)
```

## 3. Bill of Materials (prototype → product)

### Core (MVP — order these first)
| Qty | Part | Notes |
|---|---|---|
| 1 | **DFRobot C1001 / SEN0623** mmWave sensor | the radar; UART |
| 1 | **ESP32-S3-DevKitC-1 N16R8** (proto) → WROOM-1 N16R8 module (product) | MCU |
| 1 | WS2812B/SK6812 **RGBW** ring or COB strip (or 5-ch warm/cool LED + driver) | lamp light. RGBW or dedicated warm+cool gives better "zero-blue" tuning than RGB |
| 1 | 5 V / 3 A USB-C power supply + USB-C connector | power |
| — | Dupont/JST wires, perfboard | proto |

### V1 add-ons (Sleepal parity)
| Qty | Part | Purpose |
|---|---|---|
| 1 | **MAX98357A** I2S DAC+amp + 3–4 Ω speaker | white noise / meditation / breathing coach |
| 1 | **INMP441 / ICS-43434** I2S MEMS mic | snore + ambient noise |
| 1 | **BME280** (I2C) | room temp / humidity / pressure |
| 1 | **BH1750** (I2C) | ambient light (auto-brightness) |
| 1 | **DS3231** RTC (I2C) | reliable alarm time across power loss |
| 1 | Rotary encoder w/ push, or capacitive touch | local control |
| 1 | **Slide switch (mic-mute)** in mic power/clk line | privacy selling point |
| — | LED driver MOSFETs / constant-current driver if using high-power COB | brightness |

### Higher-end lighting option
For genuine Ra95+ "full-spectrum, zero blue", a **5-channel LED** (R,G,B,
warm-white ~2700K, cool-white ~6500K) driven by a constant-current driver
(e.g. based on an LED controller IC or PWM+MOSFETs) gives far better circadian
control than RGB(W) pixels. WS2812-style is fine for the MVP/aesthetic ring.

## 4. Firmware architecture (ESP32-S3, Arduino or ESP-IDF)

Modules / FreeRTOS tasks:
1. **RadarTask** — poll `DFRobot_HumanDetection` once/sec; publish a struct
   {presence, inBed, sleepState, hr, breath, turnovers, score} to a queue.
2. **LightTask** — circadian engine: time-of-day curve + reactions
   (auto-dim when `sleepState != awake`, night-light on out-of-bed, sunrise
   ramp during the smart-alarm light-sleep window).
3. **AudioTask** — I2S playback of white-noise/meditation; breathing-pacer tone;
   alarm. I2S capture for snore RMS/AI when armed.
4. **AlarmEngine** — given target wake time T and a window (e.g. T−30min..T),
   trigger sunrise + sound the moment radar reports `light/awake`; hard-fire at T.
5. **EnvTask** — BH1750/BME280 every few seconds for auto-brightness + context.
6. **Connectivity** — Wi-Fi provisioning (BLE), small web/REST or MQTT, optional
   **Matter** endpoint (light + occupancy + sleep-tracking sensor), OTA.
7. **Storage** — nightly `sSleepStatistics` to flash/SD + push to app/cloud.

### "Smart wake" core algorithm (the headline feature)
```
on each radar tick during [wakeTarget - window, wakeTarget]:
    if sleepState == LIGHT or sleepState == AWAKE:
        start sunrise ramp + gentle sound; mark alarm fired
at wakeTarget:
    if not fired: hard sunrise + alarm
stop on: out-of-bed (stand-to-stop) OR big turnover (turn-to-snooze)
```

## 5. Open questions / things to validate on real hardware
- Real-world HR/breathing stability with a duvet and at the chosen lamp
  distance/angle (datasheet says <1.5 m chest — confirm bedside geometry).
- Sensor RF interference from the LED switching supply / Wi-Fi — keep the radar
  on its own clean 5 V, physically separated from LED PWM and antenna.
- Whether the C1001's nightly score is good enough, or we compute our own from
  the raw per-minute stream (we have stage + HR + breath + movement, so we can).
- Matter currently has no native "sleep tracker" device type → expose as
  occupancy + light, send sleep data through the companion app instead.

## 6. Suggested next steps
1. Order C1001 + ESP32-S3-DevKitC-1 N16R8 + an LED ring + USB-C PSU (MVP set).
2. Flash the DFRobot `sleep.ino` example, confirm UART readings on your desk.
3. Port readings into a FreeRTOS RadarTask + a basic sunrise LightTask.
4. Add audio + env + app once the radar→light loop feels right.
