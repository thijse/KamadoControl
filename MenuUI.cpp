#include "MenuUI.h"

#include <Fonts/FreeMonoBold9pt7b.h>
#include "Fonts/FreeSansBold8pt7b.h"
#include "Fonts/FreeSans8pt7b.h"
#include <Fonts/Picopixel.h>


MenuUI* pointerToMenuControl = nullptr;


// Stubs to connect to class functions
result setTemperatureControl(eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setTemperatureControl(e, nav, item); }
result setDamperMin         (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setDamperMin         (e, nav, item); }
result setDamperMax         (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setDamperMax         (e, nav, item); }
result setTempControlSource (eventMask e, navNode& nav, prompt& item) { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setTempControlSource (e, nav, item); }
result idle                 (menuOut & o, idleEvent e)                { if (pointerToMenuControl != nullptr) return pointerToMenuControl->setIdle              (o,e)         ; }


/* Toggle temperature control */
int    temperatureControl      = HIGH;
int    damperMin               = 0;
int    damperMax               = 180;
//int    damperVal               = 90;
int    tempControlSource       = -1;
double pPid                    = 4.5;
double iPid                    = 1.200;
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

MENU(pidSubmenu, "PID settings", doNothing, noEvent, wrapStyle
    , FIELD(pPid, "P", "", 0.0,  20.0,  0.5, 0.1, setDamperMin, anyEvent, noStyle)
    , FIELD(iPid, "I", "", 0.0, 500.0, 10.0, 0.5, setDamperMax, anyEvent, noStyle)
    , FIELD(dPid, "D", "", 0.0,  10.0,  0.5, 0.1, setDamperMax, anyEvent, noStyle)
    , EXIT("<Back")
);

/* Main menu */
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
    , SUBMENU(temperatureControlToggleSubmenu)
    , SUBMENU(tempControlSourceSubmenu)
    , SUBMENU(pidSubmenu)
    , FIELD(damperMin, "Damper min", "", 0, 180, 10, 1, setDamperMin, anyEvent, noStyle)
    , FIELD(damperMax, "Damper max", "", 0, 180, 10, 1, setDamperMax, anyEvent, noStyle)
    , EXIT("<Back")
);


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


RotaryClickEncoderStream encStream( rotaryEncoder);

const panel panels[] MEMMODE = { {0, 0, menuColumns, menuLines} }; // Main menu panel
//const panel panels[] MEMMODE = { {0, 0, 200/ fontW, 100/ fontH} }; // Main menu panel
navNode*   nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, sizeof(panels) / sizeof(panel)); //a list of panels and nodes
//idx_t tops[MAX_DEPTH]={0,0}; // store cursor positions for each level
idx_t      eSpiTops[MAX_DEPTH] = { 0 };
GxEPDOut   gxEPDOut( display, colors, eSpiTops, pList, fontW, fontH + 1, menuLeft,menuTop ,12);
idx_t      serialTops[MAX_DEPTH] = { 0 };
serialOut  outSerial(Serial, serialTops);
menuOut* constMEM outputs[] MEMMODE = { &gxEPDOut }; // { &outSerial, & gxEPDOut };            //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*)); //outputs list
serialIn    serial(Serial);
//MENU_INPUTS(in, &encStream, &serial); 
NAVROOT(nav, mainMenu, MAX_DEPTH, encStream, out);



/***********************************************************************************************/

MenuUI::MenuUI(Screen* display, AppState *appState, ControlValues *controlValues) :
    _display      (display),
    _appState     (appState),
    _controlValues(controlValues),
    _posX         (40),
    _posY         (10),
    _width        (150),
    _height       (39)
{

}

void MenuUI::init()
{
    if (pointerToMenuControl != nullptr)
    {
        //Log.errorln(F("Already a MenuUI class instantiated. There can be only one "));
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

void MenuUI::draw()
{
    // initial drawing same as update
    //update(MenuState::idle);
}

// Returns true when menu is turned off
bool MenuUI::update(MenuState &menuState)
{
	if (menuState == MenuState::menuIdle)
	{
		return false;
	}
    // Check if menu is starting, running or idle
    if (menuState == MenuState::menuWaking)
	{  
		// No idea
		setControlValues(); // set current values to control values
		encStream.flush();							// flush makes sure we are not going to take steps when switching to menu mode
		nav.reset();
		nav.idleOff();
		menuState = MenuState::menuActive;
		internalMenuState = MenuState::menuActive;
         rotaryEncoder.setAcceleration(0);
		// TO DO: MAKE SURE LIMITS OF ROTARY ENCODER ARE TURNED OFF!!!
	}
        

    setControlValues(); // set current values to control values
    // Set font
    _display->setFont(&FreeSansBold8pt7b);
	// Process input as long as there is input...
	while (encStream.available())
	{
		nav.doInput();
	}
	// Generate output
	nav.doOutput();

    getControlValues();// gets update values from control values

    if (internalMenuState == MenuState::menuIdleStart) {
        // cleaning up when menu off
        _display->fillRect(menuLeft, menuTop, menuWidth, menuHeight, GxEPD_WHITE);
        internalMenuState = MenuState::menuIdle;        
    }
        
    menuState = internalMenuState;
    // Always request a partial update 
    _display->updateRequest(Screen::none);

	return menuState == MenuState::menuIdle;
}

void MenuUI::setControlValues()
{
    pPid                    = _controlValues->P;
    iPid                    = _controlValues->I*1e2;
    dPid                    = _controlValues->D;
    damperMin               = _controlValues->damperMin;
    damperMax               = _controlValues->damperMax;  
   
    tempControlSource       = _controlValues->tempControlSource;
    temperatureControl      = _appState->temperatureControl;
    
}

void MenuUI::getControlValues()
{
    _controlValues->P                       = pPid;
    _controlValues->I                       = iPid*1e-2;
    _controlValues->D                       = dPid;
    _controlValues->damperMin               = damperMin;
    _controlValues->damperMax               = damperMax;
    _controlValues->tempControlSource       = tempControlSource;
    _appState->temperatureControl           = temperatureControl;
    _controlValues->damperMode              = _appState->calibrateDamperMinMax!= DamperMode::damperModeNone? _appState->calibrateDamperMinMax: //calibrateDamperMinMax contains if damper calibration is going on, and if it is min or max
                                              (_appState->temperatureControl? DamperMode::damperModeAutomatic: DamperMode::damperModeManual);

    Serial.print("Set damperMode"); Serial.println(_controlValues->damperMode);
}

result MenuUI::setTemperatureControl(eventMask e, navNode& nav, prompt& item) {
    //Serial.print("\nTemperature control set to "); Serial.println(temperatureControl);
    //Serial.print("event type: ");
    //Serial.println(e);
    return proceed;
}

result MenuUI::setDamperMin(eventMask e, navNode& nav, prompt& item) {
    switch (e)
    {
    case Menu::enterEvent:
        //Serial.println("\npause measure&control");
        _appState->calibrateDamperMinMax = DamperMode::damperModeMin;
        break;
    case Menu::exitEvent:
    case Menu::blurEvent:
    case Menu::selBlurEvent:
        //Serial.println("\nresume measure&control");
        _appState->calibrateDamperMinMax = DamperMode::damperModeNone;
        break;
        return proceed;
    }
    if (damperMin > damperMax) damperMin = damperMax;
    return proceed;
}

result MenuUI::setDamperMax(eventMask e, navNode& nav, prompt& item) {
    switch (e)
    {
    case Menu::enterEvent:
        //Serial.println("\npause measure&control");
        // todo: make this a pauze & resume to the temperatureControl state
        _appState->calibrateDamperMinMax = DamperMode::damperModeMax;
        break;
    case Menu::exitEvent:
    case Menu::blurEvent:
    case Menu::selBlurEvent:
        //Serial.println("\nresume measure&control");
        _appState->calibrateDamperMinMax = DamperMode::damperModeNone;
        break;
    return proceed;
    }
    if (damperMax < damperMin ) damperMax = damperMin;
    return proceed;
}

result MenuUI::setTempControlSource(eventMask e, navNode& nav, prompt& item)
{
    if (tempControlSource == -1) tempControlSource = 5;
    //Serial.print("Set temperature control source to "); Serial.print(tempControlSource);
    return proceed;
}

result MenuUI::setIdle(menuOut& o, idleEvent e) {
    // 
    switch (e) {
    //case idleStart:        
    case idling:
        Serial.println("suspended...");
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