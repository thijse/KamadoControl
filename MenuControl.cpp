#include "MenuControl.h"
#include <Fonts/FreeMonoBold9pt7b.h>

MenuControl* pointerToMenuControl = nullptr;


// Stubs to connect to class functions
result setTemperatureControl(eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setTemperatureControl(e, nav, item); }
result setDamperMin         (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setDamperMin         (e, nav, item); }
result setDamperMax         (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setDamperMax         (e, nav, item); }
result setTempControlSource (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setTempControlSource (e, nav, item); }
result idle                 (menuOut & o, idleEvent e)                { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setIdle              (o,e)         ; }


/* Toggle temperature control */
int    temperatureControl      = HIGH;
int    temperatureControlPauze = LOW;
int    damperMin               = 0;
int    damperMax               = 180;
int    tempControlSource       = -1;
double pPid                    = 1.0;
double iPid                    = 60.0;
double dPid                    = 0.0 ;

TOGGLE(temperatureControl, temperatureControlToggleSubmenu, "Temp control: ", doNothing, noEvent, noStyle
    , VALUE("On", HIGH, setTemperatureControl, noEvent)
    , VALUE("Off", LOW, setTemperatureControl, noEvent)
);

/* Damper min- &  max-settings */

CHOOSE(tempControlSource, tempControlSourceSubmenu, "Choose", doNothing, noEvent, noStyle
    , VALUE("Thermocouple 1",  0, setTempControlSource, enterEvent)
    , VALUE("Thermocouple 2",  1, setTempControlSource, enterEvent)
    , VALUE("Thermistor   1",  2, setTempControlSource, enterEvent)
    , VALUE("Thermistor   2",  3, setTempControlSource, enterEvent)
    , VALUE("Thermistor   3",  4, setTempControlSource, enterEvent)
    , VALUE("Thermistor   4", -1, setTempControlSource, enterEvent)
);

/* Main menu */
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
    , SUBMENU(temperatureControlToggleSubmenu)
    , SUBMENU(tempControlSourceSubmenu)
    , FIELD(damperMin, "Damper min", "", 0, 180, 10, 1, setDamperMin, anyEvent, noStyle)
    , FIELD(damperMax, "Damper max", "", 0, 180, 10, 1, setDamperMax, anyEvent, noStyle)
    , EXIT("<Back")
);

#define MAX_DEPTH 2
MENU_OUTPUTS(out, MAX_DEPTH, SERIAL_OUT(Serial), NONE);
serialIn serial(Serial);
ClickEncoderStream encStream(rotaryEncoder, 1);
//MENU_INPUTS(in, &rotaryEncoderIn, &serial); // &encButton,
NAVROOT(nav, mainMenu, MAX_DEPTH, encStream, out);

//result idle(menuOut& o, idleEvent e) {
//
//    // o.clear();
//    switch (e) {
//    case idleStart:o.println("suspending menu!"); break;
//    case idling:o.println("suspended..."); break;
//    case idleEnd:o.println("resuming menu.");
//        nav.reset();
//        break;
//    }
//    return proceed;
//}

/***********************************************************************************************/

MenuControl::MenuControl(Screen* display, ControlValues* controlValues) :
    _display      (display),
    _controlValues(controlValues),
    _posX         (40),
    _posY         (10),
    _width        (150),
    _height       (39)
{

}

void MenuControl::init()
{
    if (pointerToMenuControl != nullptr)
    {
        //Log.errorln(F("Already a MenuControl class instantiated. There can be only one "));
        return; // throw;
    }
    pointerToMenuControl = this;

    Serial.println("KamadoControl menu");

    Serial.println("Select : *");
    Serial.println("Escape : /");
    Serial.println("Up     : +");
    Serial.println("Down   : -");
    Serial.println("Numeric keys (1-9) will jump to the respective menu element");

    nav.timeOut = 20;
    nav.idleTask = idle; //point a function to be used when menu is suspended
}

void MenuControl::draw()
{
    // initial drawing same as update
    //update(MenuState::idle);
}

void MenuControl::update(MenuState &menuState)
{
    // Check if menu is starting, running or idle
    if (menuState == MenuState::menuWaking) {  menuState = MenuState::menuActive; internalMenuState = menuState; nav.reset(); }
    if (menuState == MenuState::menuIdle  ) { return;                                                                         }

    setControlValues(); // set current values to control values
    nav.poll();
    getControlValues();// gets update values from control values

    menuState = internalMenuState;
    // Always request a partial update 
    _display->updateRequest(Screen::none);
}

void MenuControl::setControlValues()
{
    pPid                    = _controlValues->P;
    iPid                    = _controlValues->I;
    dPid                    = _controlValues->D;
    damperMin               = _controlValues->damperMin;
    damperMax               = _controlValues->damperMax;    
    tempControlSource       = _controlValues->tempControlSource;
    temperatureControl      = _controlValues->temperatureControl;
    temperatureControlPauze = _controlValues->temperatureControlPauze;
}

void MenuControl::getControlValues()
{
    _controlValues->P                       = pPid;
    _controlValues->I                       = iPid;
    _controlValues->D                       = dPid;
    _controlValues->damperMin               = damperMin;
    _controlValues->damperMax               = damperMax;
    _controlValues->tempControlSource       = tempControlSource;
    _controlValues->temperatureControl      = temperatureControl;
    _controlValues->temperatureControlPauze = temperatureControlPauze;
}

result MenuControl::setTemperatureControl(eventMask e, navNode& nav, prompt& item) {
    Serial.print("\nTemperature control set to "); Serial.println(temperatureControl);
    //Serial.print("event type: ");
    //Serial.println(e);
    return proceed;
}

result MenuControl::setDamperMin(eventMask e, navNode& nav, prompt& item) {
    switch (e)
    {
    case Menu::enterEvent:
        Serial.println("\npause measure&control");
        temperatureControlPauze = LOW;
        break;
    case Menu::exitEvent:
    case Menu::blurEvent:
    case Menu::selBlurEvent:
        Serial.println("\nresume measure&control");
        temperatureControlPauze = HIGH;
        break;
        return proceed;
    }
    Serial.print("Set damper damper range to min value "); Serial.print(damperMin);

    return proceed;
}

result MenuControl::setDamperMax(eventMask e, navNode& nav, prompt& item) {
    switch (e)
    {
    case Menu::enterEvent:
        Serial.println("\npause measure&control");
        // todo: make this a pauze & resume to the temperatureControl state
        temperatureControlPauze = LOW;
        break;
    case Menu::exitEvent:
    case Menu::blurEvent:
    case Menu::selBlurEvent:
        Serial.println("\nresume measure&control");
        temperatureControlPauze = HIGH;
        break;
    return proceed;
    }
    Serial.print("Set damper damper range to max value "); Serial.print(damperMax);

    return proceed;
}

result MenuControl::setTempControlSource(eventMask e, navNode& nav, prompt& item)
{
    if (tempControlSource == -1) tempControlSource = 5;
    Serial.print("Set temperature control source to "); Serial.print(tempControlSource);
    return proceed;
}

result MenuControl::setIdle(menuOut& o, idleEvent e) {
    // o.clear();
    switch (e) {
    //case idleStart:        
    case idling:o.println("suspended..."); 
        internalMenuState = MenuState::menuIdle;
        break;    
    case idleEnd  :o.println("resuming menu.");
        internalMenuState = MenuState::menuActive;
        nav.reset();
        break;
    }
    return proceed;
}