// https://savjee.be/2019/12/esp32-tips-to-increase-battery-life/
// https://www.robmiles.com/journal/2020/1/20/disabling-the-esp32-brownout-detector

#include <ArduinoLog.h>
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
#include "SetTemperature.h"
//#include "Logo.h"
#include "SDCard.h"
#include "Battery.h"
#include "Timer.h"

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


Screen display(ELINK_SS, ELINK_DC, ELINK_RESET, ELINK_BUSY);
SPIClass sdSPI(VSPI);

MeasureAndControl measureControl;

Battery           battery(&display, BATTERY_PIN);
Timer             timer  (&display);
SetTemperature    setTemperature(&display);
//Logo              logo   (&display);
//SDCard            sdcard (&display, SDCARD_SS);
int               loopCounter;


void taskMain(void* pvParameters);
void taskMeasureAndControl(void* pvParameters);

void setup() {

    // initialize serial communication at 115200 bits per second:
    Wire.setClock(100000L);
    Wire.begin(I2C_SDA, I2C_SCL);

    Serial.begin(115200);
    while (!Serial && !Serial.available()) {}
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    Log.traceln(F("setup KamadoControl"));
    loopCounter  = 0;

    SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
    sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

    // Settings to reduce power consumption
    System::PowerSafeMode();

    // Initialize display
    display.init         ();  
    display.setRotation  (3);
    display.setFullWindow();
    display.fillScreen   (GxEPD_WHITE);
    display.setTextColor (GxEPD_BLACK);
    display.setFont      (&FreeMonoBold9pt7b);
    display.setCursor    (0, 0);

    // Initialize UI elements
    battery       .init();
    timer         .init();
    setTemperature.init();
    //logo        .init();
    //sdcard      .init(&sdSPI);

    // Draw UI elements
    display       .fillScreen(GxEPD_WHITE);
    battery       .draw();
    timer         .draw();
    setTemperature.draw();
    //logo        .draw();
    //sdcard      .draw();
    display       .display(false); // full update

    xTaskCreatePinnedToCore(
        taskMain
        , "TaskMain"
        , 4096  // Todo: stack size can be checked & adjusted by reading the Stack Highwater
        , NULL
        , 2  // 
        , NULL
        , ARDUINO_RUNNING_CORE); // The core running Arduino

    xTaskCreatePinnedToCore(
        taskMeasureAndControl
        , "TaskMeasureAndControl"
        , 2048  // Stack size
        , NULL
        , 1     // Priority
        , NULL
        , 0);   // The core not running Arduino
    
}

void loop()
{
    // Empty. Things are done in Tasks.
}

/*---------------------- Tasks ---------------------*/


void taskMain(void* pvParameters)
{
    (void)pvParameters;

    for (;;) // A Task shall never return or exit.
    {
        Log.traceln(F("loop %d"),loopCounter++);
        (void)pvParameters;
        battery       .update();
        timer         .update();
        setTemperature.update();
        //logo        .update();
        //sdcard      .update();
        display       .update(); // Update the screen depending on update requests.

        measureControl.SetTargetTemperature(setTemperature.getTargetTemperature());
        //setTemperature.setCurrentTemperature();


        MeasurementData* data = measureControl.GetMeasurements();
        if (data != nullptr) {
            Serial.print("Temperature 0     : ")  ; Serial.println(data->Temperature[0]);
            Serial.print("Temperature 1     : ")  ; Serial.println(data->Temperature[1]);
            Serial.print("Temperature 2     : ")  ; Serial.println(data->Temperature[2]);
            Serial.print("Temperature 3     : ")  ; Serial.println(data->Temperature[3]);
            Serial.print("Target Temperature: ")  ; Serial.println(data->targetTemperature);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // wait for one second
    }
}

void taskMeasureAndControl(void* pvParameters)  // This is a task.
{
    (void)pvParameters;

    for (;;)
    {
        measureControl.update();        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
