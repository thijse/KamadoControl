// https://savjee.be/2019/12/esp32-tips-to-increase-battery-life/
// https://www.robmiles.com/journal/2020/1/20/disabling-the-esp32-brownout-detector


#include "WiFi.h" 
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

#include <SD.h>
#include <SPI.h>
#include "System.h"

#include "Constants.h"
#include "MeasureAndControl.h"
#include "ThermoCouple.h"

// UI
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

MeasureAndControl measureControl;

Battery           battery(&display, BATTERY_PIN);
Timer             timer  (&display);
Logo              logo   (&display);
SDCard            sdcard (&display, SDCARD_SS);



void TaskMain(void* pvParameters);
void TaskMeasureAndControl(void* pvParameters);

void setup() {

    // initialize serial communication at 115200 bits per second:
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");
    SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
    sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

    // Settings to reduce power consumption
    System::PowerSafeMode();

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

    xTaskCreatePinnedToCore(
        TaskMain
        , "TaskMain"
        , 1024  // Todo: stack size can be checked & adjusted by reading the Stack Highwater
        , NULL
        , 2  // 
        , NULL
        , ARDUINO_RUNNING_CORE); // The core running Arduino

    xTaskCreatePinnedToCore(
        TaskMeasureAndControl
        , "TaskMeasureAndControl"
        , 1024  // Stack size
        , NULL
        , 1     // Priority
        , NULL
        , 0);   // The core not running Arduino

    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
    // Empty. Things are done in Tasks.
}

/*---------------------- Tasks ---------------------*/


void TaskMain(void* pvParameters)
{
    (void)pvParameters;

    for (;;) // A Task shall never return or exit.
    {
        (void)pvParameters;
        battery.update();
        timer  .update();
        logo   .update();
        sdcard .update();
        display.update(); // Update the screen depending on update requests.

        auto* data = measureControl.GetMeasurements();
        if (data!=nullptr) Serial.print("Temperature: "); Serial.println(data->Temperature[0]);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for one second
    }
}

void TaskMeasureAndControl(void* pvParameters)  // This is a task.
{
    (void)pvParameters;

    for (;;)
    {
        //Serial.print("Main TaskMeasure and control");
        measureControl.Run();        
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
