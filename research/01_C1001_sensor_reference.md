# C1001 (SEN0623) 60GHz mmWave Human Detection Sensor — Technical Reference

> The radar "brain" of our lamp. This is the single component that makes the
> Sleepal clone possible: contactless presence, sleep staging, breathing and
> heart rate — all without a camera or wearable.

Sources:
- Product page: https://www.dfrobot.com/product-2861.html (SKU **SEN0623**)
- Wiki: https://wiki.dfrobot.com/SKU_SEN0623_C1001_mmWave_Human_Detection_Sensor
- API/Protocol/Pinout: https://wiki.dfrobot.com/sen0623/docs/21571
- Driver library: https://github.com/DFRobot/DFRobot_HumanDetection

---

## 1. What it is

A 60 GHz FMCW millimeter-wave radar module (Seeed/DFRobot "C1001"). Unlike the
common 24 GHz radars (LD2410 etc.), the 60 GHz band gives enough bandwidth to
resolve **micro-motions** — the sub-millimeter chest displacement of breathing
and the even smaller motion of a heartbeat. The module ships with **on-board
firmware** that already does all the hard DSP and outputs cooked, human-readable
values (sleep stage, BPM, breaths/min, fall events) over UART. We do **not**
process raw radar — we just read results. This is why the project is realistic.

## 2. Electrical / RF specs

| Parameter | Value |
|---|---|
| Operating voltage | **5 V** (VIN) |
| Current | **≤ 100 mA** |
| RF frequency | 61 – 61.5 GHz (60 GHz band) |
| Transmit power | 6 dBm |
| Detection angle | 100° × 100° cone |
| Max presence range | 11 m |
| Sleep (chest) detection range | **0.4 – 2.5 m** |
| Breathing/heartbeat range | **0.4 – 1.5 m** ← critical for lamp placement |
| Breathing rate output | 10 – 25 breaths/min |
| Heart rate output | 60 – 100 bpm |
| Operating temp | −20 to 60 °C |
| Interface | **UART, 115200 baud, 8N1** |
| Logic | 3.3 V on RX/TX and IO pins |

> ⚠️ **Placement constraint that drives the whole industrial design:** accurate
> breathing/heart-rate needs the sensor within **1.5 m of the sleeper's chest**.
> A bedside lamp ~40–80 cm from the pillow is comfortably inside that window.
> Sleep-stage (movement-based) works out to 2.5 m. Keep the sensor face
> roughly pointed at the torso, unobstructed (no thick blanket mound, no metal).

## 3. Pinout

| Pin | Function | Connect to ESP32-S3 |
|---|---|---|
| VIN | 5 V power | 5 V rail |
| GND | Ground | GND |
| RX  | Sensor serial RX (data **in** to sensor) | ESP32 **TX** GPIO |
| TX  | Sensor serial TX (data **out** of sensor) | ESP32 **RX** GPIO |
| IO2 | Human-presence level out (3.3 V, **Fall mode only**) | optional GPIO |
| IO1 | Fall-status level out (3.3 V, **Fall mode only**) | optional GPIO |

For our sleep lamp we only need **VIN, GND, RX, TX**. IO1/IO2 are hardware
shortcut outputs that are only active in Fall mode — we read everything over
UART instead.

### Reference wiring from the official example
```
C1001        ESP32 (Arduino core)
 VCC   <----  5V (3.3/5V tolerant, use 5V)
 GND   <----  GND
 RX    <----  D2  (ESP32 TX)
 TX    ---->  D3  (ESP32 RX)
```
`Serial1.begin(115200, SERIAL_8N1, /*rx=*/D3, /*tx=*/D2);`

> NOTE: `D2`/`D3` in the example are **Arduino board aliases, NOT GPIO numbers.**
> On classic ESP32 `D2` often == GPIO2, but on the **ESP32-S3** they may be
> undefined or map elsewhere. On the S3, ignore the aliases and pass **explicit
> GPIO numbers** — the UART can route to almost any free pin via the pin matrix:
> ```cpp
> Serial1.begin(115200, SERIAL_8N1, /*RX=*/18, /*TX=*/17);
> //  ESP32-S3 GPIO18 (RX) <-- C1001 TX ;  GPIO17 (TX) --> C1001 RX  (cross-over)
> ```
> ⚠️ On the **N16R8** module avoid GPIO 26–37 (flash + octal PSRAM, esp. 33–37),
> GPIO 0/3/45/46 (strapping), and 19/20 (USB). Safe: 17/18, 15/16, 4–14.
> Always use a **hardware UART** (UART1/UART2), never SoftwareSerial at 115200.

## 4. Two firmware work modes (mutually exclusive)

Set with `configWorkMode()`. You must reset (`sensorRet()`) after config writes.

- **`eSleepMode` (0x02)** ← what we use. Outputs presence, in/out-of-bed,
  sleep state, heart rate, breathing, turnovers, apnea events, whole-night
  statistics + sleep score.
- **`eFallingMode` (0x01)** — posture/fall/static-dwell alarm. Not used for the
  lamp (could be a future "elderly care" SKU).

## 5. UART protocol (handled for us by the library)

Frame: `0x53 0x59 | control | command | lenH lenL | <data...> | checksum | 0x54 0x43`
- Header: `0x53 0x59` ("SY")
- Footer: `0x54 0x43` ("TC")
- Checksum: 8-bit sum of all preceding bytes.

You normally never touch this — the DFRobot library builds/parses frames. It's
documented here only so we can debug with a logic analyzer if readings stall.

## 6. Sleep-mode data we can read (the gold)

### Live, per-second (`smSleepData()`, `smHumanData()`, getters)
| Call | Meaning |
|---|---|
| `smHumanData(eHumanPresence)` | someone present 0/1 |
| `smHumanData(eHumanMovement)` | none/still/active |
| `smHumanData(eHumanMovingRange)` | body-movement magnitude 0–100 |
| `smSleepData(eInOrNotInBed)` | 0 out of bed / 1 in bed |
| `smSleepData(eSleepState)` | **0 deep / 1 light / 2 awake / 3 none** |
| `getHeartRate()` | bpm |
| `getBreatheValue()` | breaths/min |
| `getBreatheState()` | 1 normal / 2 fast / 3 slow / 4 none |
| `smSleepData(eWakeDuration)` | minutes awake |
| `smSleepData(eDeepSleepDuration)` | minutes deep |
| `smSleepData(eSleepQuality)` | live quality score |

### Composite snapshot (`getSleepComposite()` → `sSleepComposite`)
presence, sleepState, **averageRespiration, averageHeartbeat**, turnoverNumber,
largeBodyMove %, minorBodyMove %, **apneaEvents**.

### Whole-night report (`getSleepStatistics()` → `sSleepStatistics`)
Emitted automatically when the sensor decides the sleep session ended:
**sleepQualityScore**, sleepTime (min), wakeDuration, **shallowSleepPercentage,
deepSleepPercentage**, timeOutOfBed, exitCount, turnOverCount,
averageRespiration, averageHeartbeat, apneaEvents.

### Quality ratings / alarms
- `eSleepQualityRating`: 1 good / 2 average / 3 poor
- `eSleepDisturbances`: <4 h, >12 h, long absence
- `eAbnormalStruggle`: normal vs. abnormal struggle (safety)

> This single sensor already gives us **every health metric the Sleepal lamp
> advertises** (sleep stages, duration, breathing rate, heart rate, position
> changes/turnovers, apnea, a nightly sleep score). Snoring is the only metric
> it does NOT provide — that needs a microphone (see architecture doc).

## 7. Driver library summary

`DFRobot_HumanDetection` (Arduino C++; also a Raspberry Pi Python port).
- Constructor takes a `Stream*` (a `HardwareSerial`).
- `begin()` returns 0 on success — poll until ready.
- Verified working on Mega2560, Leonardo, **ESP32** (S3 works via Arduino-ESP32).
- Files mirrored locally in `../firmware/` for offline reference.

## 8. Gotchas learned from the source

1. Always call `sensorRet()` after writing any config, or the module ignores it.
2. `begin()` can take a few seconds; loop with a 1 s delay (see example).
3. Heart-rate/breathing only populate when the chest is **< 1.5 m** and still —
   expect garbage/zeros while the person moves around or is out of bed.
4. The nightly `sSleepStatistics` only arrives at end-of-session; cache it.
5. Module is 5 V powered but **3.3 V logic** — direct-wire to ESP32-S3 is fine,
   do NOT feed 5 V into a GPIO.
