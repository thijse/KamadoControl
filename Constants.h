#pragma once
#include "arduino.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

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
#define SDCARD_SS                 13
#define SDCARD_CLK                14
#define SDCARD_MOSI               15
#define SDCARD_MISO                2

// Battery pin
#define BATTERY_PIN               35

// Rotary encoder pins
#define ROTARY_ENCODER_A_PIN      36
#define ROTARY_ENCODER_B_PIN      34
#define ROTARY_ENCODER_BUTTON_PIN 39
#define ROTARY_ENCODER_VCC_PIN    -1 /*put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */



class Const
{
public:
   // static const
};