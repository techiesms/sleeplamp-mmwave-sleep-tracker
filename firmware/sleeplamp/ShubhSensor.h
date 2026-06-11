/*!
 * @file    ShubhSensor.h
 * @brief   ShubhSensor - single-file driver for the DFRobot C1001 / SEN0623
 *          60 GHz mmWave human-detection sensor: presence, in-bed, sleep
 *          staging, heart rate and breathing over UART. The original library's
 *          header and implementation are merged here into ONE file.
 *
 * @author  Packaged, patched & maintained by Shubh Jaiswal (2026)
 *          https://github.com/Shubhjaiswal408/sleeplamp-mmwave-sleep-tracker
 *          Patches: non-blocking cached reads (pump() / cacheGet()), removed
 *          per-byte delays, hardened RX buffers against corrupt-frame crashes,
 *          and a shorter command timeout.
 *
 * @credit  Built on the DFRobot_HumanDetection library:
 *            Copyright (c) 2010 DFRobot Co.Ltd  -  The MIT License (MIT)
 *            Original author: tangjie <jie.tang@dfrobot.com>
 *            https://github.com/DFRobot/DFRobot_HumanDetection
 *          Original copyright retained as required by the MIT License.
 *
 * @note    The implementation lives in this header. Include it from only ONE
 *          translation unit - the SleepLamp sketch does (via types.h).
 * @license MIT
 */
#pragma once
#include "Arduino.h"

#if (defined ARDUINO_AVR_UNO) && (defined ESP8266)
#include "SoftwareSerial.h"
#else
#include "HardwareSerial.h"
#endif

//#define ENABLE_DBG ///< Uncomment this macro to see detailed runtime process of the program
#ifdef ENABLE_DBG
#define DBG(...)                     \
    {                                \
        Serial.print("[");           \
        Serial.print(__FUNCTION__);  \
        Serial.print("(): ");        \
        Serial.print(__LINE__);      \
        Serial.print(" ] ");         \
        Serial.println(__VA_ARGS__); \
    }
#else
#define DBG(...)
#endif

#define TIME_OUT 2500   // PATCH: was 5000 - shorter stall when a query goes unanswered

#define CMD_HEAD 0
#define CMD_CONFIG 1
#define CMD_CMD 2
#define CMD_LEN_H 3
#define CMD_LEN_L 4
#define CMD_END_H 5
#define CMD_END_L 6
#define CMD_WHITE 7
#define CMD_DATA 8

/**
 * @brief Sleep composite state data
 */
typedef struct
{
    uint8_t presence;           ///< Presence state
    uint8_t sleepState;         ///< Sleep state
    uint8_t averageRespiration; ///< Average respiration
    uint8_t averageHeartbeat;   ///< Average heartbeat
    uint8_t turnoverNumber;     /// Turnover number
    uint8_t largeBodyMove;      ///< Large body movement percentage
    uint8_t minorBodyMove;      ///< Minor body movement percentage
    uint8_t apneaEvents;        ///< Apnea events

} sSleepComposite;

/**
 * @brief Sleep statistics query
 */
typedef struct
{
    uint8_t sleepQualityScore;      ///< Sleep quality score
    uint16_t sleepTime;             ///< Sleep duration in minutes
    uint8_t wakeDuration;           ///< Wake duration
    uint8_t shallowSleepPercentage; ///< Shallow sleep duration percentage
    uint8_t deepSleepPercentage;    ///< Deep sleep duration percentage
    uint8_t timeOutOfBed;           ///< Time out of bed
    uint8_t exitCount;              ///< Exit count
    uint8_t turnOverCount;          ///< Turnover count
    uint8_t averageRespiration;     ///< Average respiration
    uint8_t averageHeartbeat;       /// Average heartbeat
    uint8_t apneaEvents;            ///< Apnea events

} sSleepStatistics;

class ShubhSensor
{
public:
    /**
     * @brief Working mode configuration structure
     */
    typedef enum
    {
        eSleepMode = 0x02,
        eFallingMode = 0x01,
    } eWorkMode;

    /**
     * @brief LED light selection
     */
    typedef enum
    {
        eFALLLed = 0x03,
        eHPLed = 0x04,
    } eLed;

    /**
     * @brief Human-related data in sleep mode
     */
    typedef enum
    {
        eHumanPresence,    ///< Human presence query
        eHumanMovement,    ///< Movement information query
        eHumanMovingRange, ///< Movement distance, range 0~100
        eHumanDistance,

    } esmHuman;

    /**
     * @brief Sleep data in sleep mode
     */
    typedef enum
    {
        // eSleepSwitch,///<Get sleep switch query
        eInOrNotInBed,           ///< Get in or out of bed status
        eSleepState,             ///< Get sleep state
        eWakeDuration,           ///< Get wake duration
        eLightsleep,             ///< Shallow sleep
        eDeepSleepDuration,      ///< Get deep sleep duration
        eSleepQuality,           ///< Get sleep quality
        eSleepDisturbances,      ///< Sleep abnormality query
        eSleepQualityRating,     ///< Sleep quality rating
        eAbnormalStruggle,       ///< Abnormal struggle
        eUnattendedState,        ///< No one timing query
        eAbnormalStruggleSwitch, ///< Abnormal struggle switch query
        eUnattendedSwitch,       ///< No timing switch query
        eUnattendedTime,         ///< No timing time query
        esleepDeadline,          ///< Sleep deadline
        eReportingmode,          // Reporting mode
    } eSmSleep;

    /**
     * @brief Sleep function configuration in sleep mode
     */
    typedef enum
    {
        eReportingmodeC,    ///< Reporting mode
        eAbnormalStruggleC, ///< Abnormal struggle
        eUnattendedStateC,  ///< No timing
        eUnattendedTimeC,   ///< No timing time
        esleepDeadlineC,    ///< Sleep deadline

    } eSmSleepConfig;

    /**
     * @brief Human data in falling mode
     */
    typedef enum
    {
        // eHumanSwitch,///<Get fall mode human presence switch query
        eExistence,                ///< Human presence query
        eMotion,                   ///< Motion query
        eBodyMove,                 ///< Body movement query
        eTrajectorySwitch,         ///< Trajectory point switch query
        eSeatedHorizontalDistance, ///< Seated horizontal distance
        eMotionHorizontalDistance, /// Motion horizontal distance

    } eDmHuman;

    /**
     * @brief Get fall data
     */
    typedef enum
    {
        // eFallSwitch,///<Fall mode switch query
        eFallState,             ///< Fall state
        eFallBreakHeight,       ///< Fall break height
        eHeightRatioSwitch,     ///< Height ratio switch query
        estaticResidencyState,  ///< Static residency state
        estaticResidencySwitch, ///< Static residency switch
        eFallSensitivity,       ///< Fall sensitivity
    } eDmFall;

    /**
     * @brief Human configuration in falling mode
     */
    typedef enum
    {
        eSeatedHorizontalDistanceC, ///< Seated horizontal distance
        eMotionHorizontalDistanceC, /// Motion horizontal distance
    } eDmHumanConfig;

    /**
     * @brief Get fall data
     */
    typedef enum
    {
        eFallBreakHeightC,   ///< Fall break height
        eHeightRatioSwitchC, ///< Height ratio switch
        eReportFreqC,        ///< Trajectory point information reporting frequency
        eReportSwitchC,      ///< Trajectory point reporting switch
        eAltTimeC,           ///< Height cumulative time
        eFallSensitivityC,   ///< Fall sensitivity setting
        eResidenceSwitchC,   ///< Residency switch
        eResidenceTime,      ///< Residency time

    } eDmFallConfig;

    /**
     * @fn ShubhSensor
     * @brief Constructor of the millimeter-wave human detection sensor
     * @param s Serial reception object
     */
    ShubhSensor(Stream *s);
    ~ShubhSensor() {};

    /**
     * @fn begin
     * @brief Initialize the sensor
     * @return Initialization status
     * @retval 0 Initialization successful
     * @retval 1 Initialization failed
     */
    uint8_t begin(void);

    /**
     * @fn configWorkMode
     * @brief Initialize mode
     * @param mode Mode selection
     * @return Initialization status
     * @retval 0 Mode configuration successful
     * @retval 1 Mode configuration failed
     */
    uint8_t configWorkMode(eWorkMode mode);

    /**
     * @fn getWorkMode
     * @brief Get working mode
     * @return Working mode
     */
    uint8_t getWorkMode(void);

    /**
     * @fn configLEDLight
     * @brief Configure LED light
     * @param led LED selection to turn on
     * @param sta 0: On, 1: Off
     * @return Control status
     * @retval 0 Configuration successful
     * @retval 1 Configuration failed
     */
    uint8_t configLEDLight(eLed led, uint8_t sta);

    /**
     * @fn getLEDLightState
     * @brief Get LED light status
     * @param led LED selection
     * @return Light status
     * @retval 0 Light off
     * @retval 1 Light on
     */
    uint8_t getLEDLightState(eLed led);

    /**
     * @fn sensorRet
     * @brief Reset the sensor
     * @return Reset status
     * @retval 0 Reset successful
     * @retval 1 Reset failed
     */
    uint8_t sensorRet(void);

    /**
     * @fn smHumanData
     * @brief Query human-related content in sleep mode
     * @param hm Data content selection
     */
    uint16_t smHumanData(esmHuman hm);

    /**
     * @fn getHeartRate
     * @brief Get heart rate
     * @return Heart rate
     */
    uint8_t getHeartRate(void);

    /**
     * @fn getBreatheState
     * @brief Get respiration detection information
     * @return Respiration information
     * @retval 1 Normal
     * @retval 2 Too fast
     * @retval 3 Too slow
     * @retval 4 None
     */
    uint8_t getBreatheState(void);

    /**
     * @fn getBreatheValue
     * @brief Get respiration value
     * @return Respiration value
     */
    uint8_t getBreatheValue(void);

    /**
     * @fn smSleepData
     * @brief Get sleep-related data
     * @param sl Data to retrieve
     * @return Retrieved data
     */
    uint16_t smSleepData(eSmSleep sl);

    /**
     * @fn getSleepComposite
     * @brief Query sleep composite state
     * @return Queryed composite data
     */
    sSleepComposite getSleepComposite(void);

    /**
     * @fn getSleepStatistics
     * @brief Query sleep statistics status
     * @return Queryed sleep statistics data
     */
    sSleepStatistics getSleepStatistics(void);

    /**
     * @fn configSleep
     * @brief Configure sleep mode function
     * @param sl Function selection
     * @param data Configuration data
     * @return Setting status
     */
    uint8_t configSleep(eSmSleepConfig sl, uint8_t data);

    /**
     * @fn installAngle
     * @brief Radar angle installation setting in fall mode
     * @param x x angle
     * @param y y angle
     * @param z z angle
     */
    void dmInstallAngle(int16_t x, int16_t y, int16_t z);

    /**
     * @fn dmGetInstallAngle
     * @brief Get radar installation angle
     * @param x x angle
     * @param y y angle
     * @param z z angle
     */
    void dmGetInstallAngle(int16_t *x, int16_t *y, int16_t *z);

    /**
     * @fn dmInstallHeight
     * @brief Set radar installation height
     * @param he Installation height
     */
    void dmInstallHeight(uint16_t he);

    /**
     * @fn dmGetInstallHeight
     * @brief Get installation height
     * @return Retrieved installation height
     */
    uint16_t dmGetInstallHeight(void);

    /**
     * @fn autoMeasureHeight
     * @brief Get automatic height measurement data
     * @return Retrieved automatic height measurement data
     */
    uint16_t dmAutoMeasureHeight(void);

    /**
     * @fn dmHumanData
     * @brief Get human-related data in fall mode
     * @return Retrieved data
     */
    uint16_t dmHumanData(eDmHuman dh);

    /**
     * @fn track
     * @brief Track point query
     * @param x x coordinate
     * @param y y coordinate
     */
    void track(uint16_t *x, uint16_t *y);

    /**
     * @fn trackFrequency
     * @brief Get track point reporting frequency
     * @return Retrieved track point reporting frequency
     */
    uint32_t trackFrequency(void);

    /**
     * @fn getUnmannedTime
     * @brief Unmanned time query
     */
    uint32_t getUnmannedTime(void);

    /**
     * @fn UnmannedTime
     * @brief Set unmanned time
     * @param Time Unmanned time
     */
    void dmUnmannedTime(uint32_t Time);

    /**
     * @fn getFallData
     * @brief Get fall detection function data
     * @param dm Data selection
     * @return Retrieved data
     */
    uint16_t getFallData(eDmFall dm);

    /**
     * @fn getFallTime
     * @brief Get fall duration
     * @return Retrieved fall duration
     */
    uint32_t getFallTime(void);

    /**
     * @fn dmFallTime
     * @brief Set fall duration
     * @param Time Fall duration
     */
    void dmFallTime(uint32_t Time);

    /**
     * @fn getStaticResidencyTime
     * @brief Residency time query
     * @return Retrieved residency time
     */
    uint32_t getStaticResidencyTime(void);

    /**
     * @fn accumulatedHeightDuration
     * @brief Height cumulative time query
     * @return Retrieved height cumulative time data
     */
    uint32_t accumulatedHeightDuration(void);

    /**
     * @fn dmHumanConfig
     * @brief Configure human configuration in fall mode
     * @param con Configuration selection
     * @param da Configuration data
     * @return Configuration status
     */
    uint8_t dmHumanConfig(eDmHumanConfig con, uint16_t data);

    /**
     * @fn unattendedTimeConfig
     * @brief Set unmanned time query in fall mode
     * @brief time Set time
     * @return Setting status
     */
    uint8_t unattendedTimeConfig(uint32_t time);
    /**
     * @fn dmFallConfig
     * @brief Fall mode configuration
     * @param con Configuration option
     * @param data Configuration data
     * @return Configuration status
     *
     */
    uint8_t dmFallConfig(eDmFallConfig con, uint32_t data);

    // ===== PATCH: passive report harvesting =====
    // The radar pushes unsolicited report frames in sleep mode; the stock driver
    // discarded them. pump() parses everything in the RX buffer into a cache;
    // cacheGet() reads a cached report (returns data length, or -1 if none fresh).
    void pump();
    int  cacheGet(uint8_t con, uint8_t cmd, uint8_t *out, uint16_t outSize, uint32_t maxAgeMs);

private:
    /**
     * @fn getData
     * @brief Send command and get data
     * @param con Control word
     * @param cmd Command word
     * @param len Data length
     * @param senData Sent data
     * @param retData Returned data
     * @return Communication status
     */
    uint8_t getData(uint8_t con, uint8_t cmd, uint16_t len, uint8_t *senData, uint8_t *retData);
    uint8_t sumData(uint8_t len, uint8_t *buf);
    Stream *_s = NULL;
    // PATCH: generic frame parser + report cache
    bool parseByte(uint8_t b);
    void cacheStore(void);
    struct { uint8_t st = 0; uint16_t need = 0, got = 0; uint8_t raw[64]; } _rx;
    struct sCacheEnt { uint8_t con = 0, cmd = 0, len = 0; uint8_t data[16]; uint32_t ts = 0; };
    sCacheEnt _cache[14];
    uint16_t _pairSeen[16]; uint8_t _pairN = 0;
};


// ===== implementation (merged from the original .cpp) =====

#include "stdio.h"

ShubhSensor::ShubhSensor(Stream *s) : _s(s)
{
}

uint8_t ShubhSensor::begin(void)
{
    // PATCH: no blind 10 s wait. Flush radar boot chatter, then probe once
    // (caller retries). Succeeds in <1 s when the radar is already running.
    while (_s->available()) _s->read();
    delay(300);
    uint8_t data = 0x0f;
    uint8_t buf[64];
    if (getData(0x01, 0x83, 1, &data, buf) == 0)
    {
        return 0;
    }
    return 1;
}

uint8_t ShubhSensor::configWorkMode(eWorkMode mode)
{
    uint8_t data = 0x0f;
    uint8_t buf[64];
    uint8_t cmdBuf[10] = {0x53, 0x59, 0x02, 0x08, 0x00, 0x01, 0x00, 0x00, 0x54, 0x43};

    if (getData(0x02, 0xA8, 1, &data, buf) == 0) // Query the current working mode
    {
        if (buf[6] == mode) // Determine whether mode switching is necessary
        {
            return 0;
        }
        else // Mode switch required
        {
            cmdBuf[6] = mode;
            cmdBuf[7] = sumData(7, cmdBuf);
            _s->write(cmdBuf, 10);

            delay(10000); // Waiting for mode switch to start

            if (getData(0x02, 0xA8, 1, &data, buf) == 0) // Query current working mode
            {
                if (buf[6] == mode) // Verify if the mode switch was successful
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
            return 1;
        }
    }
    return 1;
}

uint8_t ShubhSensor::getWorkMode(void)
{
    uint8_t buf[64];
    uint8_t data = 0x0f, ret = 2;
    if (getData(0x02, 0xA8, 1, &data, buf) == 0)
    {
        ret = buf[6];
    }
    return ret;
}

uint8_t ShubhSensor::configLEDLight(eLed led, uint8_t sta)
{
    uint8_t data = sta;
    uint8_t buf[64];
    if (led == eFALLLed)
    {
        if (getData(0x01, 0x04, 1, &data, buf) == 0)
        {
            return 0;
        }
    }
    else
    {
        if (getData(0x01, 0x03, 1, &data, buf) == 0)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t ShubhSensor::getLEDLightState(eLed led)
{
    uint8_t data = 0x0f;
    uint8_t buf[64];
    uint8_t ret = 2;
    if (led == eFALLLed)
    {
        if (getData(0x01, 0x84, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
    }
    else
    {
        if (getData(0x01, 0x83, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
    }
    return ret;
}

uint8_t ShubhSensor::sensorRet(void)
{
    uint8_t data = 0x0f;
    uint8_t buf[64];
    if (getData(0x01, 0x02, 1, &data, buf) == 0)
    {
        delay(10000);
        return 0;
    }
    return 1;
}

uint16_t ShubhSensor::smHumanData(esmHuman hm)
{
    uint8_t data = 0x0f;
    uint8_t buf[64];
    uint16_t ret = 0;
    switch (hm)
    {
    case eHumanPresence:
        if (getData(0x80, 0x81, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eHumanMovement:
        if (getData(0x80, 0x82, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eHumanMovingRange:
        if (getData(0x80, 0x83, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eHumanDistance:
        if (getData(0x80, 0x84, 1, &data, buf) == 0)
        {
            ret = buf[6] << 8 | buf[7];
        }
        break;
    default:
        break;
    }
    return ret;
}

uint8_t ShubhSensor::getHeartRate(void)
{
    uint8_t data = 0x0f, ret = 0xff;
    uint8_t buf[64];
    if (getData(0x85, 0x82, 1, &data, buf) == 0)
    {
        ret = buf[6];
    }
    return ret;
}

uint8_t ShubhSensor::getBreatheState(void)
{
    uint8_t data = 0x0f, ret = 0xff;
    uint8_t buf[64];
    if (getData(0x81, 0x81, 1, &data, buf) == 0)
    {
        ret = buf[6];
    }
    return ret;
}

uint8_t ShubhSensor::getBreatheValue(void)
{
    uint8_t data = 0x0f, ret = 0xff;
    uint8_t buf[64];
    if (getData(0x81, 0x82, 1, &data, buf) == 0)
    {
        ret = buf[6];
    }
    return ret;
}

uint16_t ShubhSensor::smSleepData(eSmSleep sl)
{
    uint16_t ret = 0xff;
    uint8_t data = 0x0f;
    uint8_t buf[64];

    switch (sl)
    {
    case eReportingmode:
        if (getData(0x84, 0x8C, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eInOrNotInBed:
        if (getData(0x84, 0x81, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eSleepState:
        if (getData(0x84, 0x82, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eWakeDuration:
        if (getData(0x84, 0x83, 1, &data, buf) == 0)
        {
            ret = buf[6] << 8 | buf[7];
        }
        break;
    case eLightsleep:
        if (getData(0x84, 0x84, 1, &data, buf) == 0)
        {
            ret = buf[6] << 8 | buf[7];
        }
        break;
    case eDeepSleepDuration:
        if (getData(0x84, 0x85, 1, &data, buf) == 0)
        {
            ret = buf[6] << 8 | buf[7];
        }
        break;
    case eSleepQuality:
        if (getData(0x84, 0x86, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eSleepDisturbances:
        if (getData(0x84, 0x8E, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eSleepQualityRating:
        if (getData(0x84, 0x90, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eAbnormalStruggle:
        if (getData(0x84, 0x91, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eUnattendedState:
        if (getData(0x84, 0x92, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eAbnormalStruggleSwitch:
        if (getData(0x84, 0x93, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eUnattendedSwitch:
        if (getData(0x84, 0x94, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case eUnattendedTime:
        if (getData(0x84, 0x95, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;
    case esleepDeadline:
        if (getData(0x84, 0x96, 1, &data, buf) == 0)
        {
            ret = buf[6];
        }
        break;

    default:
        break;
    }
    return ret;
}

sSleepComposite ShubhSensor::getSleepComposite(void)
{
    uint8_t data = 0x0f;
    uint8_t buf[64];
    sSleepComposite sleepData = {
        .presence = 0,
        .sleepState = 0,
        .averageRespiration = 0,
        .averageHeartbeat = 0,
        .turnoverNumber = 0,
        .largeBodyMove = 0,
        .minorBodyMove = 0,
        .apneaEvents = 0,
    };
    if (getData(0x84, 0x8D, 1, &data, buf) == 0)
    {
        memcpy(&sleepData, &buf[6], sizeof(sSleepComposite));
    }
    return sleepData;
}

sSleepStatistics ShubhSensor::getSleepStatistics(void)
{
    uint8_t data = 0x0f;
    uint8_t buf[64];
    sSleepStatistics sleepSta = {
        .sleepQualityScore = 0,
        .sleepTime = 0,
        .wakeDuration = 0,
        .shallowSleepPercentage = 0,
        .deepSleepPercentage = 0,
        .timeOutOfBed = 0,
        .exitCount = 0,
        .turnOverCount = 0,
        .averageRespiration = 0,
        .averageHeartbeat = 0,
        .apneaEvents = 0,
    };

    if (getData(0x84, 0x8F, 1, &data, buf) == 0)
    {
        memcpy(&sleepSta, &buf[6], sizeof(sSleepStatistics));
    }
    return sleepSta;
}

uint8_t ShubhSensor::configSleep(eSmSleepConfig sl, uint8_t data)
{
    // uint8_t _data = data;
    uint8_t buf[64];
    switch (sl)
    {
    case eReportingmodeC:
        if (getData(0x84, 0x0F, 1, &data, buf) == 0)
        {
            return 0;
        }
        break;
    case eAbnormalStruggleC:
        if (getData(0x84, 0x13, 1, &data, buf) == 0)
        {
            return 0;
        }
        break;
    case eUnattendedStateC:
        if (getData(0x84, 0x14, 1, &data, buf) == 0)
        {
            return 0;
        }
        break;
    case eUnattendedTimeC:
        if (getData(0x84, 0x15, 1, &data, buf) == 0)
        {
            return 0;
        }
        break;
    case esleepDeadlineC:
        if (getData(0x84, 0x16, 1, &data, buf) == 0)
        {
            return 0;
        }
        break;
    default:
        break;
    }
    return 1;
}

void ShubhSensor::dmInstallAngle(int16_t x, int16_t y, int16_t z)
{
    uint8_t sendBuf[6];
    uint8_t readBuf[64];
    sendBuf[0] = x >> 8 & 0xff;
    sendBuf[1] = x & 0xff;
    sendBuf[2] = y >> 8 & 0xff;
    sendBuf[3] = y & 0xff;
    sendBuf[4] = z >> 8 & 0xff;
    sendBuf[5] = y & 0xff;
    if (getData(0x06, 0x01, 6, sendBuf, readBuf) == 0)
    {
    }
}

void ShubhSensor::dmGetInstallAngle(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    if (getData(0x06, 0x81, 1, &data, readBuf) == 0)
    {
        *x = readBuf[6] << 8 | readBuf[7];
        *y = readBuf[8] << 8 | readBuf[9];
        *z = readBuf[10] << 8 | readBuf[11];
    }
    else
    {
        *x = 0;
        *y = 0;
        *z = 0;
    }
}

void ShubhSensor::dmInstallHeight(uint16_t he)
{
    uint8_t buf[2];
    uint8_t readBuf[64];
    buf[0] = he >> 8 & 0xff;
    buf[1] = he & 0xff;
    getData(0x06, 0x02, 2, buf, readBuf);
}

uint16_t ShubhSensor::dmGetInstallHeight(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint16_t ret = 0;
    if (getData(0x06, 0x82, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 8 | readBuf[7];
    }
    return ret;
}

uint16_t ShubhSensor::dmAutoMeasureHeight(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint16_t ret = 0;
    if (getData(0x83, 0x90, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 8 | readBuf[7];
    }
    return ret;
}

uint16_t ShubhSensor::dmHumanData(eDmHuman dh)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint16_t ret = 0;
    switch (dh)
    {
    case eExistence:
        if (getData(0x80, 0x81, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case eMotion:
        if (getData(0x80, 0x82, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case eBodyMove:
        if (getData(0x80, 0x83, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case eTrajectorySwitch:
        if (getData(0x83, 0x94, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case eSeatedHorizontalDistance:
        if (getData(0x80, 0x8D, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6] << 8 | readBuf[7];
        }
        break;
    case eMotionHorizontalDistance:
        if (getData(0x80, 0x8e, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6] << 8 | readBuf[7];
        }
        break;
    default:
        break;
    }
    return ret;
}

void ShubhSensor::track(uint16_t *x, uint16_t *y)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    if (getData(0x83, 0x8e, 1, &data, readBuf) == 0)
    {
        *x = readBuf[6] << 8 | readBuf[7];
        *y = readBuf[8] << 8 | readBuf[9];
    }
    else
    {
        *x = 0;
        *y = 0;
    }
}

uint32_t ShubhSensor::trackFrequency(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint32_t ret = 0;
    if (getData(0x83, 0x93, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 24 | readBuf[7] << 16 | readBuf[8] << 8 | readBuf[9];
    }
    return ret;
}

uint32_t ShubhSensor::getUnmannedTime(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint32_t ret = 0;
    if (getData(0x80, 0x92, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 24 | readBuf[7] << 16 | readBuf[8] << 8 | readBuf[9];
    }
    return ret;
}

void ShubhSensor::dmUnmannedTime(uint32_t Time)
{
    uint8_t buf[4];
    uint8_t readBuf[64];
    buf[0] = Time >> 24 & 0xff;
    buf[1] = Time >> 16 & 0xff;
    buf[2] = Time >> 8 & 0xff;
    buf[3] = Time & 0xff;
    getData(0x80, 0x12, 4, buf, readBuf);
}

uint16_t ShubhSensor::getFallData(eDmFall dm)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint16_t ret = 0;
    switch (dm)
    {
    case eFallState:
        if (getData(0x83, 0x81, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case eFallBreakHeight:
        if (getData(0x83, 0x91, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6] << 8 | readBuf[7];
        }
        break;
    case eHeightRatioSwitch:
        if (getData(0x83, 0x95, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case estaticResidencyState:
        if (getData(0x83, 0x85, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case estaticResidencySwitch:
        if (getData(0x83, 0x8B, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    case eFallSensitivity:
        if (getData(0x83, 0x8D, 1, &data, readBuf) == 0)
        {
            ret = readBuf[6];
        }
        break;
    default:
        break;
    }
    return ret;
}

uint32_t ShubhSensor::getFallTime(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint32_t ret = 0;
    if (getData(0x83, 0x8c, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 24 | readBuf[7] << 16 | readBuf[8] << 8 | readBuf[9];
    }
    return ret;
}

void ShubhSensor::dmFallTime(uint32_t Time)
{
    uint8_t buf[4];
    uint8_t readBuf[64];
    buf[0] = Time >> 24 & 0xff;
    buf[1] = Time >> 16 & 0xff;
    buf[2] = Time >> 8 & 0xff;
    buf[3] = Time & 0xff;
    getData(0x83, 0x0C, 4, buf, readBuf);
}

uint32_t ShubhSensor::getStaticResidencyTime(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint32_t ret = 0;
    if (getData(0x83, 0x8a, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 24 | readBuf[7] << 16 | readBuf[8] << 8 | readBuf[9];
    }
    return ret;
}

uint32_t ShubhSensor::accumulatedHeightDuration(void)
{
    uint8_t readBuf[64];
    uint8_t data = 0x0f;
    uint32_t ret = 0;
    if (getData(0x83, 0x8F, 1, &data, readBuf) == 0)
    {
        ret = readBuf[6] << 24 | readBuf[7] << 16 | readBuf[8] << 8 | readBuf[9];
    }

    return ret;
}

uint8_t ShubhSensor::dmHumanConfig(eDmHumanConfig con, uint16_t data)
{
    uint8_t readBuf[64];
    uint8_t sendBuf[2];
    sendBuf[0] = data >> 8 & 0xff;
    sendBuf[1] = data & 0xff;
    uint8_t ret = 1;
    switch (con)
    {
    case eSeatedHorizontalDistanceC:
        if (getData(0x80, 0x0D, 2, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eMotionHorizontalDistanceC:
        if (getData(0x80, 0x0E, 2, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    default:
        break;
    }
    return ret;
}

uint8_t ShubhSensor::unattendedTimeConfig(uint32_t time)
{
    uint8_t readBuf[64];
    uint8_t sendBuf[4];
    sendBuf[0] = time >> 24 & 0xff;
    sendBuf[1] = time >> 16 & 0xff;
    sendBuf[2] = time >> 8 & 0xff;
    sendBuf[3] = time & 0xff;
    uint8_t ret = 1;
    if (getData(0x80, 0x12, 4, sendBuf, readBuf) == 0)
    {
        ret = 0;
    }
    return ret;
}

uint8_t ShubhSensor::dmFallConfig(eDmFallConfig con, uint32_t data)
{
    uint8_t readBuf[64];
    uint8_t sendBuf[4];
    uint8_t ret = 1;
    switch (con)
    {
    case eFallBreakHeightC:
        sendBuf[0] = (data & 0xffff) >> 8;
        sendBuf[1] = data & 0xff;
        if (getData(0x83, 0x11, 2, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eHeightRatioSwitchC:
        sendBuf[0] = data & 0xff;
        if (getData(0x83, 0x15, 1, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eReportFreqC:
        sendBuf[0] = data >> 24 & 0xff;
        sendBuf[1] = data >> 16 & 0xff;
        sendBuf[2] = data >> 8 & 0xff;
        sendBuf[3] = data & 0xff;
        if (getData(0x83, 0x13, 4, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eReportSwitchC:
        sendBuf[0] = data & 0xff;
        if (getData(0x83, 0x14, 1, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eAltTimeC:
        sendBuf[0] = data >> 24 & 0xff;
        sendBuf[1] = data >> 16 & 0xff;
        sendBuf[2] = data >> 8 & 0xff;
        sendBuf[3] = data & 0xff;
        if (getData(0x83, 0x0f, 4, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eFallSensitivityC:
        sendBuf[0] = data & 0xff;
        if (getData(0x83, 0x0D, 1, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eResidenceSwitchC:
        sendBuf[0] = data & 0xff;
        if (getData(0x83, 0x0b, 1, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    case eResidenceTime:
        sendBuf[0] = data >> 24 & 0xff;
        sendBuf[1] = data >> 16 & 0xff;
        sendBuf[2] = data >> 8 & 0xff;
        sendBuf[3] = data & 0xff;
        if (getData(0x83, 0x0A, 4, sendBuf, readBuf) == 0)
        {
            ret = 0;
        }
        break;
    default:
        break;
    }
    return ret;
}

// ===================== PATCHED COMMS CORE =====================
// The radar pushes unsolicited report frames in sleep mode. The stock driver
// discarded every byte that wasn't the awaited reply (and even flushed the RX
// buffer before each retransmit). This core parses EVERY valid frame: the
// awaited reply is returned, everything else is harvested into a small report
// cache (see pump()/cacheGet()) so most values can be read with NO query at all.

bool ShubhSensor::parseByte(uint8_t b)
{
    switch (_rx.st)
    {
    case 0: if (b == 0x53) { _rx.raw[0] = b; _rx.st = 1; } break;
    case 1: _rx.raw[1] = b; _rx.st = (b == 0x59) ? 2 : 0; break;
    case 2: _rx.raw[2] = b; _rx.st = 3; break;                       // control word
    case 3: _rx.raw[3] = b; _rx.st = 4; break;                       // command word
    case 4: _rx.raw[4] = b; _rx.need = ((uint16_t)b) << 8; _rx.st = 5; break;
    case 5:
        _rx.raw[5] = b; _rx.need |= b; _rx.got = 0;
        if (_rx.need > 50) { _rx.st = 0; break; }                    // corrupt-length guard
        _rx.st = _rx.need ? 6 : 7;
        break;
    case 6:
        _rx.raw[6 + _rx.got++] = b;
        if (_rx.got >= _rx.need) _rx.st = 7;
        break;
    case 7: _rx.st = (b == sumData(6 + _rx.need, _rx.raw)) ? 8 : 0; break;  // checksum
    case 8: _rx.st = (b == 0x54) ? 9 : 0; break;
    case 9: _rx.st = 0; if (b == 0x43) return true; break;
    default: _rx.st = 0; break;
    }
    return false;
}

void ShubhSensor::cacheStore(void)
{
    uint8_t con = _rx.raw[2], cmd = _rx.raw[3];
    uint8_t len = _rx.need > 16 ? 16 : (uint8_t)_rx.need;

    // one-time discovery log: shows which frames this radar actually pushes
    uint16_t pair = ((uint16_t)con << 8) | cmd;
    bool seen = false;
    for (uint8_t i = 0; i < _pairN; i++) if (_pairSeen[i] == pair) { seen = true; break; }
    if (!seen && _pairN < 16)
    {
        _pairSeen[_pairN++] = pair;
        Serial.printf("[C1001] report frame discovered: con=0x%02X cmd=0x%02X len=%u\n", con, cmd, (unsigned)_rx.need);
    }

    int slot = -1;
    for (int i = 0; i < 14; i++) if (_cache[i].con == con && _cache[i].cmd == cmd) { slot = i; break; }
    if (slot < 0) { slot = 0; for (int i = 1; i < 14; i++) if (_cache[i].ts < _cache[slot].ts) slot = i; }
    _cache[slot].con = con; _cache[slot].cmd = cmd; _cache[slot].len = len;
    memcpy(_cache[slot].data, _rx.raw + 6, len);
    _cache[slot].ts = millis();
}

int ShubhSensor::cacheGet(uint8_t con, uint8_t cmd, uint8_t *out, uint16_t outSize, uint32_t maxAgeMs)
{
    for (int i = 0; i < 14; i++)
    {
        if (_cache[i].ts && _cache[i].con == con && _cache[i].cmd == cmd && (millis() - _cache[i].ts) <= maxAgeMs)
        {
            uint8_t n = _cache[i].len > outSize ? (uint8_t)outSize : _cache[i].len;
            memcpy(out, _cache[i].data, n);
            return n;
        }
    }
    return -1;
}

void ShubhSensor::pump()
{
    while (_s->available() > 0) if (parseByte(_s->read())) cacheStore();
}

uint8_t ShubhSensor::getData(uint8_t con, uint8_t cmd, uint16_t len, uint8_t *senData, uint8_t *retData)
{
    uint8_t cmdBuf[20];
    cmdBuf[0] = 0x53; cmdBuf[1] = 0x59; cmdBuf[2] = con; cmdBuf[3] = cmd;
    cmdBuf[4] = (len >> 8) & 0xff; cmdBuf[5] = len & 0xff;
    memcpy(&cmdBuf[6], senData, len);
    cmdBuf[6 + len] = sumData(6 + len, cmdBuf);
    cmdBuf[7 + len] = 0x54; cmdBuf[8 + len] = 0x43;

    uint32_t tStart = millis(), tSend = 0;
    while ((millis() - tStart) < TIME_OUT)
    {
        if (tSend == 0 || (millis() - tSend) > 600)
        {                                            // retransmit; NO RX flush â€” report frames survive
            _s->write(cmdBuf, 9 + len);
            tSend = millis();
        }
        while (_s->available() > 0)
        {
            if (parseByte(_s->read()))
            {
                if (_rx.raw[2] == con && _rx.raw[3] == cmd)
                {                                    // legacy layout: callers read retData[6..]
                    memcpy(retData, _rx.raw, 6 + _rx.need);
                    return 0;
                }
                cacheStore();                        // someone else's data â€” harvest, never discard
            }
        }
        delay(1);
    }
    DBG("Time out");
    return 2;
}

uint8_t ShubhSensor::sumData(uint8_t len, uint8_t *buf)
{
    uint16_t data = 0;
    uint8_t *_buf = buf;
    for (uint8_t i = 0; i < len; i++)
    {
        data += _buf[i];
    }
    return data & 0xff;
}
