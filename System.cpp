// 
// 
// 

#include "System.h"
#include "driver/adc.h"

void System::printBootReason()
{
    esp_reset_reason_t reset_reason = esp_reset_reason();

    switch (reset_reason)
    {
    case ESP_RST_UNKNOWN:    Serial.println(F("Reset reason can not be determined"                    )); break;
    case ESP_RST_POWERON:    Serial.println(F("Reset due to power-on event"                           )); break;
    case ESP_RST_EXT:        Serial.println(F("Reset by external pin (not applicable for ESP32)"      )); break;
    case ESP_RST_SW:         Serial.println(F("Software reset via esp_restart"                        )); break;
    case ESP_RST_PANIC:      Serial.println(F("Software reset due to exception/panic"                 )); break;
    case ESP_RST_INT_WDT:    Serial.println(F("Reset (software or hardware) due to interrupt watchdog")); break;
    case ESP_RST_TASK_WDT:   Serial.println(F("Reset due to task watchdog"                            )); break;
    case ESP_RST_WDT:        Serial.println(F("Reset due to other watchdogs"                          )); break;
    case ESP_RST_DEEPSLEEP:  Serial.println(F("Reset after exiting deep sleep mode"                   )); break;
    case ESP_RST_BROWNOUT:   Serial.println(F("Brownout reset (software or hardware)"                 )); break;
    case ESP_RST_SDIO:       Serial.println(F("Reset over SDIO"                                       )); break;
    }

    if (reset_reason == ESP_RST_DEEPSLEEP)
    {
        esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

        switch (wakeup_reason)
        {
        case ESP_SLEEP_WAKEUP_UNDEFINED:    Serial.println(F("In case of deep sleep: reset was not caused by exit from deep sleep"                        )); break;
        case ESP_SLEEP_WAKEUP_ALL:          Serial.println(F("Not a wakeup cause: used to disable all wakeup sources with esp_sleep_disable_wakeup_source")); break;
        case ESP_SLEEP_WAKEUP_EXT0:         Serial.println(F("Wakeup caused by external signal using RTC_IO"                                              )); break;
        case ESP_SLEEP_WAKEUP_EXT1:         Serial.println(F("Wakeup caused by external signal using RTC_CNTL"                                            )); break;
        case ESP_SLEEP_WAKEUP_TIMER:        Serial.println(F("Wakeup caused by timer"                                                                     )); break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:     Serial.println(F("Wakeup caused by touchpad"                                                                  )); break;
        case ESP_SLEEP_WAKEUP_ULP:          Serial.println(F("Wakeup caused by ULP program"                                                               )); break;
        case ESP_SLEEP_WAKEUP_GPIO:         Serial.println(F("Wakeup caused by GPIO (light sleep only)"                                                   )); break;
        case ESP_SLEEP_WAKEUP_UART:         Serial.println(F("Wakeup caused by UART (light sleep only)"                                                   )); break;
        }
    }
}

void System::PowerSafeMode()
{
    setCpuFrequencyMhz(80);      // Reduce CPU speed
    btStop();                    // Turn off Bluetooth                                    
    WiFi.disconnect(true);       // Turn off Wi-Fi   
    WiFi.mode(WIFI_OFF);
    adc_power_off();            // Turn off ADC (we are using an external ADC)
}


void System::lightSleepDelay(uint64_t delay)
{
    esp_sleep_enable_timer_wakeup(1000L * delay); //delay in mseconds
    int ret = esp_light_sleep_start();
}


void System::deepSleepDelay(uint64_t delay)
{
    // Disconnect radio, not automatically turned off by deep  sleep
    WiFi.disconnect(true);          // Turn off Wi-Fi
    WiFi.mode(WIFI_OFF);
    esp_wifi_stop();

    btStop();                       // Turn off Bluetooth
    esp_bt_controller_disable();

    adc_power_off();                // Turn off ADC 

    esp_sleep_enable_timer_wakeup(1000L * delay); //delay in mseconds
    esp_deep_sleep_start();
}
