// https://savjee.be/2019/12/esp32-tips-to-increase-batteryUI-life/
// https://www.robmiles.com/journal/2020/1/20/disabling-the-esp32-brownout-detector


#include <analogWrite.h>
#include <ArduinoLog.h>
#include "WiFi.h" 
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

// System
#include <SD.h>
#include <SPI.h>
#include "Global.h"
#include "System.h"
#include "Esp32RotaryEncoder.h"
#include "MeasureAndControl.h"

// UI
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Screen.h"
#include "BatteryUI.h"
//#include "Time.h"
#include "MainValuesUI.h"
#include "TimerUI.h"
#include "MenuUI.h"



// System
AppState          appState;        // holds application state (that the MeasureAndControl loop does not need to know about)
ControlValues     controlValues;   // holds control values & state the MeasureAndControl loop needs to know about
MeasurementData   measurements;    // holds measurements coming back from the MeasureAndControl loop 
MeasureAndControl measureControl( wireMutex, &controlValues, &measurements);

// UI
BatteryUI         batteryUI     (& display, BATTERY_PIN);
TimerUI           timerUI       (& display);
MainValuesUI      mainValuesUI  (& display, &controlValues, &measurements);
MenuUI            menuUI        (& display, &appState, &controlValues);
int               uiLoopCounter;
int               measureLoopCounter;
MenuState         menuState = MenuState::menuIdle;


void taskMain(void* pvParameters);
void menuAndNumbersUpdate();
void taskMeasureAndControl(void* pvParameters);

void setup() {

    // Turn on measurement board
    pinMode(O_33V, OUTPUT);   // turn on power to board  
    digitalWrite(O_33V, 1);
    pinMode(O_50V1, OUTPUT); // turn on power to first external 5V 
    digitalWrite(O_50V1, 1);
    pinMode(O_50V2, OUTPUT); // turn on power to second external 5V 
    digitalWrite(O_50V2, 1);

    // initialize I2C communication 
    Wire.setClock(1000L);   
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // initialize Serial communication 
    Serial.begin(115200);
    while (!Serial && !Serial.available()) { delay(5); }
    
    // Start console output & logging
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Serial.println("Kamado Control build 1");
    Serial.println();

    // Loop counters
    uiLoopCounter      = 0;
    measureLoopCounter = 0;

    // Initialize rotary click encoder    
     rotaryEncoder.begin();
     rotaryEncoder.setup(
		[] {  rotaryEncoder.readEncoder_ISR(); },
		[] {  rotaryEncoder.readButton_ISR (); });
     rotaryEncoder.setAcceleration(100);
     rotaryEncoder.setBoundaries(0, 500, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

    // Start SPI Communication
    SPI.begin  (SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);

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
    batteryUI    .init();
    timerUI      .init();
    mainValuesUI .init();
    menuUI       .init();

    // Draw UI elements
     display.fillScreen(GxEPD_WHITE);
    batteryUI      .draw();
    timerUI        .draw();
    mainValuesUI   .draw();
    menuUI         .draw();
     display.display(false); // full update

    xTaskCreatePinnedToCore(
        taskMain
        , "TaskMain"
        , 4096  // Todo: stack size can be checked & adjusted by reading the Stack Highwater
        , nullptr
        , 2  // 
        , nullptr
        , ARDUINO_RUNNING_CORE); // The core running Arduino

    xTaskCreatePinnedToCore(
        taskMeasureAndControl
        , "TaskMeasureAndControl"
        , 2048  // Stack size
        , nullptr
        , 1     // Priority
        , nullptr
        , 0);   // The core not running Arduino
    
}

void loop()
{
    // Empty. Things are done in Tasks.
}

/*---------------------- Tasks ---------------------*/

int pos = 0;

void taskMain(void* pvParameters)
{
    (void)pvParameters;

	for (;;) // A Task shall never return or exit.
	{
        //Log.traceln(F("ui loop %d"), uiLoopCounter++);

	    // Check for button press in menu idle state
		if (menuState == MenuState::menuIdle &&  rotaryEncoder.ClickOccured()) { menuState = MenuState::menuWaking;}         

        // Draw UI elements
		batteryUI      .update();
		timerUI        .update();		        
	    menuAndNumbersUpdate();
         display.update();  // Update the screen depending on update requests.              

    /*        Serial.print("Temperature 0     : ") ; if (measurements.temperatureResults.success[0]) Serial.println(measurements.temperatureResults.temperature[0]); else Serial.println("--:--");
            Serial.print("Temperature 1     : ") ; if (measurements.temperatureResults.success[1]) Serial.println(measurements.temperatureResults.temperature[1]); else Serial.println("--:--");
            Serial.print("Temperature 2     : ") ; if (measurements.temperatureResults.success[2]) Serial.println(measurements.temperatureResults.temperature[2]); else Serial.println("--:--");
            Serial.print("Temperature 3     : ") ; if (measurements.temperatureResults.success[3]) Serial.println(measurements.temperatureResults.temperature[3]); else Serial.println("--:--");
            Serial.print("Temperature 4     : ") ; if (measurements.temperatureResults.success[4]) Serial.println(measurements.temperatureResults.temperature[4]); else Serial.println("--:--");
            Serial.print("Temperature 5     : ") ; if (measurements.temperatureResults.success[5]) Serial.println(measurements.temperatureResults.temperature[5]); else Serial.println("--:--");
            Serial.print("Target Temperature: ") ; Serial.println(measurements.targetTemperature);
*/
            //mainValuesUI.setCurrentTemperature(data->temperatureResults.temperature[controlValues.tempControlSource], data->damperValue);                        
        
        // todo: make waiting time adaptive.
        //vTaskDelay(50 / portTICK_PERIOD_MS); 
    }
}

void menuAndNumbersUpdate()
{
    switch (menuState)
    {
    case MenuState::menuIdle:
        // Draw temperature menu. Target temperature either from rotary encoder or from last stored value
        mainValuesUI.update((int16_t) rotaryEncoder.readEncoderValue());
        break;
    default:
        // Draw menu
        menuUI.update(menuState);
        if (menuState == MenuState::menuIdle)
        {	// We switched to menu idle, now restore position of rotary button
             rotaryEncoder.setAcceleration(100);
             rotaryEncoder.setEncoderValue(mainValuesUI.getTargetTemperature());
        }
    }
}

void taskMeasureAndControl(void* pvParameters)  // This is a task.
{
    (void)pvParameters;

    for (;;)
    {
        //Log.traceln(F("measure loop %d"), measureLoopCounter++);
        measureControl.update();
        vTaskDelay((1000 ) / portTICK_PERIOD_MS);
    }
}
