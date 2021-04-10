#include "MenuControl.h"

#include <Fonts/FreeMonoBold9pt7b.h>
#include "Fonts/FreeSansBold8pt7b.h"
#include "Fonts/FreeSans8pt7b.h"
#include <Fonts/Picopixel.h>

MenuControl* pointerToMenuControl = nullptr;


// Stubs to connect to class functions
result setTemperatureControl(eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setTemperatureControl(e, nav, item); }
result setDamperMin         (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setDamperMin         (e, nav, item); }
result setDamperMax         (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setDamperMax         (e, nav, item); }
result setTempControlSource (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setTempControlSource (e, nav, item); }
result idle                 (menuOut & o, idleEvent e)                { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setIdle              (o,e)         ; }


/* Toggle temperature control */
int    temperatureControl      = HIGH;
bool   calibrateDamperMinMax   = false;
int    damperMin               = 0;
int    damperMax               = 180;
int    damperVal               = 90;
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

// define menu colors --------------------------------------------------------

//monochromatic color table
//#define GxEPD_BLACK RGB565(0,0,0)
//#define GxEPD_WHITE RGB565(255,255,255)

//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
const colorDef<uint16_t> colors[6] MEMMODE = {
  {{GxEPD_WHITE,GxEPD_BLACK},{GxEPD_WHITE,GxEPD_BLACK,GxEPD_BLACK}}, //bg     Color
  {{GxEPD_BLACK,GxEPD_WHITE},{GxEPD_BLACK,GxEPD_WHITE,GxEPD_WHITE}}, //fg     Color
  {{GxEPD_BLACK,GxEPD_WHITE},{GxEPD_BLACK,GxEPD_WHITE,GxEPD_WHITE}}, //val    Color
  {{GxEPD_BLACK,GxEPD_WHITE},{GxEPD_BLACK,GxEPD_WHITE,GxEPD_WHITE}}, //unit   Color
  {{GxEPD_BLACK,GxEPD_WHITE},{GxEPD_WHITE,GxEPD_WHITE,GxEPD_WHITE}}, //cursor Color
  {{GxEPD_BLACK,GxEPD_WHITE},{GxEPD_WHITE,GxEPD_BLACK,GxEPD_BLACK}}, //title  Color
};

#define fontW 7
#define fontH 20

#define MAX_DEPTH 2
/*
MENU_OUTPUTS(out, MAX_DEPTH, SERIAL_OUT(Serial), NONE);
serialIn serial(Serial);

//MENU_INPUTS(in, &rotaryEncoderIn, &serial); // &encButton,
NAVROOT(nav, mainMenu, MAX_DEPTH, encStream, out);
*/

const int menuLeft    = 2;
const int menuTop     = 12;
const int menuWidth   = 250 - menuLeft - menuLeft;
const int menuHeight  = 128 - menuTop  - 0;
const int menuColumns = menuWidth  / fontW;
const int menuLines   = menuHeight / fontH;


ClickEncoderStream encStream(rotaryEncoder, 1);

const panel panels[] MEMMODE = { {0, 0, menuColumns, menuLines} }; // Main menu panel
//const panel panels[] MEMMODE = { {0, 0, 200/ fontW, 100/ fontH} }; // Main menu panel
navNode*   nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, sizeof(panels) / sizeof(panel)); //a list of panels and nodes
//idx_t tops[MAX_DEPTH]={0,0}; // store cursor positions for each level
idx_t      eSpiTops[MAX_DEPTH] = { 0 };
GxEPDOut   gxEPDOut(display, colors, eSpiTops, pList, fontW, fontH + 1, menuLeft,menuTop ,12);
idx_t      serialTops[MAX_DEPTH] = { 0 };
serialOut  outSerial(Serial, serialTops);
menuOut* constMEM outputs[] MEMMODE = { &gxEPDOut }; // { &outSerial, & gxEPDOut };            //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*)); //outputs list
serialIn    serial(Serial);
//MENU_INPUTS(in, &encStream, &serial); 
NAVROOT(nav, mainMenu, MAX_DEPTH, encStream, out);



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
    if (menuState == MenuState::menuWaking)                                        {  menuState = MenuState::menuActive; internalMenuState = MenuState::menuActive; nav.reset(); }
    if (menuState == MenuState::menuIdle  )                                        { return;                                                                                     }

    setControlValues(); // set current values to control values
    
    _display->setFont(&FreeSansBold8pt7b);
    nav.poll();
    getControlValues();// gets update values from control values

    if (internalMenuState == MenuState::menuIdleStart) {
        // cleaning up when menu off
        Serial.println("cleaning up");
        _display->fillRect(menuLeft, menuTop, menuWidth, menuHeight, GxEPD_WHITE);
        internalMenuState = MenuState::menuIdle;        
    }
        
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
    damperVal               = _controlValues->damperVal;
    tempControlSource       = _controlValues->tempControlSource;
    temperatureControl      = _controlValues->temperatureControl;
    calibrateDamperMinMax = _controlValues->calibrateDamperMinMax;
}

void MenuControl::getControlValues()
{
    _controlValues->P                       = pPid;
    _controlValues->I                       = iPid;
    _controlValues->D                       = dPid;
    _controlValues->damperMin               = damperMin;
    _controlValues->damperMax               = damperMax;
    _controlValues->damperVal               = damperVal;
    _controlValues->tempControlSource       = tempControlSource;
    _controlValues->temperatureControl      = temperatureControl;
    _controlValues->calibrateDamperMinMax   = calibrateDamperMinMax;
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
        calibrateDamperMinMax = true;
        break;
    case Menu::exitEvent:
    case Menu::blurEvent:
    case Menu::selBlurEvent:
        Serial.println("\nresume measure&control");
        calibrateDamperMinMax = false;
        break;
        return proceed;
    }
    if (damperMin > damperMax) damperMin = damperMax;
    damperVal = damperMin;
    Serial.print("Set damper damper range to min value "); Serial.print(damperMin);
    Serial.print("Set damper damper val to "); Serial.print(damperVal);
    return proceed;
}

result MenuControl::setDamperMax(eventMask e, navNode& nav, prompt& item) {
    switch (e)
    {
    case Menu::enterEvent:
        Serial.println("\npause measure&control");
        // todo: make this a pauze & resume to the temperatureControl state
        calibrateDamperMinMax = true;
        break;
    case Menu::exitEvent:
    case Menu::blurEvent:
    case Menu::selBlurEvent:
        Serial.println("\nresume measure&control");
        calibrateDamperMinMax = false;
        break;
    return proceed;
    }
    if (damperMax < damperMin ) damperMax = damperMin;
    damperVal = damperMax;
    Serial.print("Set damper damper range to max value "); Serial.print(damperMax);
    Serial.print("Set damper damper val to "); Serial.print(damperVal);
    return proceed;
}

result MenuControl::setTempControlSource(eventMask e, navNode& nav, prompt& item)
{
    if (tempControlSource == -1) tempControlSource = 5;
    Serial.print("Set temperature control source to "); Serial.print(tempControlSource);
    return proceed;
}

result MenuControl::setIdle(menuOut& o, idleEvent e) {
    // 
    switch (e) {
    //case idleStart:        
    case idling:
        Serial.println("suspended...");
        //_display->fillRect(menuLeft, menuTop, menuWidth, menuHeight, GxEPD_WHITE);
        o.clear();
        internalMenuState = MenuState::menuIdleStart;
        //nav.exit();
        break;    
    case idleEnd  :
        //o.println("resuming menu.");
        o.clear();
        internalMenuState = MenuState::menuActive;
        nav.reset();
        break;
    }
    return proceed;
}