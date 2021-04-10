#pragma once
#include "arduino.h"
#include "Screen.h"

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

// Battery pin
#define BATTERY_PIN               35

// Rotary encoder pins
#define ROTARY_ENCODER_A_PIN      36
#define ROTARY_ENCODER_B_PIN      34
#define ROTARY_ENCODER_BUTTON_PIN 39
#define ROTARY_ENCODER_VCC_PIN    -1 /*put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */


extern SemaphoreHandle_t wireMutex;
extern Screen            display;


enum MenuState {
    menuIdle,
    menuIdleStart,
    menuActive,
    menuWaking,
};

class ControlValues 
{
private:
    SemaphoreHandle_t _mutex = nullptr;
public:   
    ControlValues           (const ControlValues& other) = delete;
    ControlValues& operator=(const ControlValues& other) = delete;
    ControlValues() { _mutex = xSemaphoreCreateMutex();                   }
    void lock()     { xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY);  }
    void unlock()   { xSemaphoreGive(_mutex);                             }

    int tempControlSource        = 0;
    int temperatureControl       = HIGH;
    bool calibrateDamperMinMax   = false;
    int damperMin                = 0;
    int damperMax                = 180;
    int damperVal                = 90;
    double P                     = 1.0;
    double I                     = 60.0;
    double D                     = 0.0;
    float  targetTemperature     = 20; 
};