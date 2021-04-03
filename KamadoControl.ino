// https://savjee.be/2019/12/esp32-tips-to-increase-battery-life/
// https://www.robmiles.com/journal/2020/1/20/disabling-the-esp32-brownout-detector


#include <analogWrite.h>
#include <ArduinoLog.h>
#include "WiFi.h" 
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

#include <SD.h>
#include <SPI.h>
#include "System.h"

#include "Constants.h"
#include "RotaryEncoder.h"

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
#include "MenuControl.h"

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

SemaphoreHandle_t wireMutex = xSemaphoreCreateMutex();
Screen            display(wireMutex,ELINK_SS, ELINK_DC, ELINK_RESET, ELINK_BUSY);
SPIClass          sdSPI(VSPI);
ControlValues     controlValues;
MeasureAndControl measureControl(wireMutex, &controlValues);
//RotaryEncoder     rotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, 4);

Battery           battery(&display, BATTERY_PIN);
Timer             timer  (&display);
SetTemperature    setTemperature(&display, &rotaryEncoder);
//Logo              logo   (&display);
//SDCard            sdcard (&display, SDCARD_SS);
MenuControl       menuControl(&display, &controlValues);
int               uiLoopCounter;
int               measureLoopCounter;
MenuState         menuState = MenuState::menuIdle;


void taskMain(void* pvParameters);
void taskMeasureAndControl(void* pvParameters);

void setup() {

    // Turn on measurement board
    pinMode(O_33V, OUTPUT);   // turn on power to board  
    digitalWrite(O_33V, 1);
    //pinMode(O_50V1, OUTPUT); // turn on power to first external 5V 
    //digitalWrite(O_50V1, 1);
    //pinMode(O_50V2, OUTPUT); // turn on power to second external 5V 
    //digitalWrite(O_50V2, 1);

    // initialize I2C communication 
    Wire.setClock(1000L);   
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // initialize Serial communication 
    Serial.begin(115200);
    while (!Serial && !Serial.available()) { delay(5); };
    
    // Start console output & logging
    Log.begin(LOG_LEVEL_ERROR, &Serial);
    Serial.println("Kamado Control build 1");
    Serial.println();

    // Loop counters
    uiLoopCounter      = 0;
    measureLoopCounter = 0;

    // Initialize rotary click encoder    
    rotaryEncoder.setAccelerationEnabled(true);
    rotaryEncoder.setDoubleClickEnabled(false); // Disable doubleclicks makes the response faster.  
    rotaryEncoder.init();                       // Triggers timer interrupt driven polling

    // Start SPI Communication
    SPI.begin  (SPI_CLK   , SPI_MISO   , SPI_MOSI   , ELINK_SS);
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
    menuControl   .init();


    // Draw UI elements
    display       .fillScreen(GxEPD_WHITE);
    battery       .draw();
    timer         .draw();
    setTemperature.draw();
    //logo        .draw();
    //sdcard      .draw();
    menuControl   .draw();
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
       // Log.traceln(F("ui loop %d"),uiLoopCounter++);
        battery       .update();
        timer         .update();
        menuControl   .update(menuState);
        setTemperature.update(menuState);
        //logo        .update();
        //sdcard      .update();
        
        display       .update(); // Update the screen depending on update requests.

        controlValues.lock();
        controlValues.targetTemperature = setTemperature.getTargetTemperature();
        controlValues.unlock();

        //measureControl.setTargetTemperature(setTemperature.getTargetTemperature());
        
        MeasurementData* data = measureControl.getMeasurements();
        if (data != nullptr) {
            //Serial.print("Temperature 0     : ") ; if (data->temperatureResults.success[0]) Serial.println(data->temperatureResults.temperature[0]); else Serial.println("--:--");
            //Serial.print("Temperature 1     : ") ; if (data->temperatureResults.success[1]) Serial.println(data->temperatureResults.temperature[1]); else Serial.println("--:--");
            //Serial.print("Temperature 2     : ") ; if (data->temperatureResults.success[2]) Serial.println(data->temperatureResults.temperature[2]); else Serial.println("--:--");
            //Serial.print("Temperature 3     : ") ; if (data->temperatureResults.success[3]) Serial.println(data->temperatureResults.temperature[3]); else Serial.println("--:--");
            //Serial.print("Temperature 4     : ") ; if (data->temperatureResults.success[4]) Serial.println(data->temperatureResults.temperature[4]); else Serial.println("--:--");
            //Serial.print("Temperature 5     : ") ; if (data->temperatureResults.success[5]) Serial.println(data->temperatureResults.temperature[5]); else Serial.println("--:--");
            //Serial.print("Target Temperature: ") ; Serial.println(data->targetTemperature);

            setTemperature.setCurrentTemperature(data->temperatureResults.temperature[0]);
        }                

        // todo: make waiting time adaptive.
        //vTaskDelay(50 / portTICK_PERIOD_MS); 
    }
}

void taskMeasureAndControl(void* pvParameters)  // This is a task.
{
    (void)pvParameters;

    for (;;)
    {
        Log.traceln(F("measure loop %d"), measureLoopCounter++);
        measureControl.update();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
