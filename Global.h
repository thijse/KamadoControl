#pragma once
#include "arduino.h"
#include "Screen.h"
#include "Esp32RotaryEncoder.h"


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

    // output pins
#define O_33V                     19
#define O_50V1                    26
#define O_50V2                    25
#define O_PWM1                    14

// I2C pins
#define I2C_SDA                   21
#define I2C_SCL                   22

// SPI pins
#define SPI_MOSI                  23
#define SPI_MISO                  -1
#define SPI_CLK                   18

// Screen pins                  
#define ELINK_SS                   5
#define ELINK_BUSY                 4
#define ELINK_RESET               16
#define ELINK_DC                  17

// SPI pins for SD card reader
//#define SDCARD_SS                 13
//#define SDCARD_CLK                14
//#define SDCARD_MOSI               15
//#define SDCARD_MISO                2

// BatteryUI pin
#define BATTERY_PIN               35

// Rotary encoder pins
#define ROTARY_ENCODER_A_PIN      36
#define ROTARY_ENCODER_B_PIN      34
#define ROTARY_ENCODER_BUTTON_PIN 39


extern SemaphoreHandle_t wireMutex;
extern Screen            display;
extern Esp32RotaryEncoder rotaryEncoder;



enum MenuState {
    menuIdle,
    menuIdleStart,
    menuActive,
    menuWaking,
};

enum DamperMode {
    damperModeAutomatic,
    damperModeManual,
    damperModeMin,
    damperModeMax,
    damperModeNone,
};


// ControlValues holds control values & state the measure&control loop needs to know about
class ControlValues
{
private:
    SemaphoreHandle_t _mutex = nullptr;
public:
    ControlValues           (const ControlValues& other) = delete;
    ControlValues& operator=(const ControlValues& other) = delete;
    ControlValues           () { _mutex = xSemaphoreCreateMutex(); }
    void lock               () { xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY); }
    void unlock             () { xSemaphoreGive(_mutex); }

    int        tempControlSource     = 0;    
    int        damperMin             =   0;
    int        damperMax             = 180;
    int        damperVal             =  90;
    
    DamperMode damperMode            = DamperMode::damperModeAutomatic;
    double P                         =   4.5;
    double I                         =  1.2;
    double D                         =   0.0;
    float  targetTemperature         =  20;
};

// AppState holds state that the measure&control loop does not need to know about
class AppState
{
public:
    AppState                (const AppState& other) = delete;
    AppState& operator=     (const AppState& other) = delete;
    AppState()              { }

    bool       temperatureControl    = true;
    DamperMode calibrateDamperMinMax = DamperMode::damperModeNone;

};

