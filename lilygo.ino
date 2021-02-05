// https://savjee.be/2019/12/esp32-tips-to-increase-battery-life/
// https://www.robmiles.com/journal/2020/1/20/disabling-the-esp32-brownout-detector


#include "SetTemperature.h"
#include "WiFi.h" 
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

#include <SD.h>
#include <SPI.h>
#include "System.h"
#include "Screen.h"
#include "Battery.h"
#include "Time.h"
#include "Logo.h"
#include "SDCard.h"
#include "Battery.h"
#include "Timer.h"

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

// SPI pins
#define SPI_MOSI    23
#define SPI_MISO    -1
#define SPI_CLK     18

// Screen pins
#define ELINK_SS      5
#define ELINK_BUSY    4
#define ELINK_RESET  16
#define ELINK_DC     17
           
// SPI pins for SD card reader
#define SDCARD_SS    13
#define SDCARD_CLK   14
#define SDCARD_MOSI  15
#define SDCARD_MISO   2
                     
#define BUTTON_PIN   39
#define BATTERY_PIN  35

Screen display(ELINK_SS, ELINK_DC, ELINK_RESET, ELINK_BUSY);

SPIClass sdSPI(VSPI);

Battery battery(&display, BATTERY_PIN);
Timer   timer  (&display);
Logo    logo   (&display);
SDCard  sdcard (&display, SDCARD_SS);

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");
    SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
    sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

    // Settings to reduce power consumption
    setCpuFrequencyMhz(80);      // Reduce CPU speed
    btStop();                    // Turn off Bluetooth                                    
    WiFi.disconnect(true);       // Turn off Wi-Fi   
    WiFi.mode(WIFI_OFF);
    adc_power_off();            // Turn off ADC (we are using an external ADC)


    // Initialize display
    display.init(); // 
    display.setRotation(1);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 0);

    // Initialize UI elements
    battery.init();
    timer  .init();
    logo   .init();
    sdcard .init(&sdSPI);

    // Draw UI elements
    display.fillScreen(GxEPD_WHITE);
    battery.draw();
    timer  .draw();
    logo   .draw();
    sdcard .draw();
    display.display(false); // full update

    Serial.print("Width  "); Serial.println(display.width());
    Serial.print("Height "); Serial.println(display.height());

    // goto sleep
    //esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
    //esp_deep_sleep_start();
}


void loop()
{
  battery.update();
  timer  .update();
  logo   .update();
  sdcard .update();
  display.update(); // Update the screen depending on update requests.
 
  System::lightSleepDelay(2000); // put device to light sleep while delaying. Deepsleep will re-initialize device
}

