// System
#pragma once 
#include "arduino.h"
#include "WiFi.h" 
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

class System
{
    protected:
public:
    System           (const System& other) = delete;
    System& operator=(const System& other) = delete;
    static void printBootReason();
    static void PowerSafeMode();
    static void lightSleepDelay(uint64_t delay);
    static void deepSleepDelay(uint64_t delay);
    static void dynamicDelay();


    static void inputEvent();
    
    
};