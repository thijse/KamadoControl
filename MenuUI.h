#pragma once
#include "arduino.h"
#include "Global.h"

#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include "RotaryClickEncoderStream.h"
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include "GxEPDOut.h"



#include "Screen.h"

class MenuUI
{
protected:
	Screen                      *_display;
	AppState                    *_appState;
	ControlValues               *_controlValues;
    int                          _posX;
	int                          _posY;
	int                          _width;
	int                          _height;	
	MenuState                    internalMenuState;

public:
    result setTemperatureControl(eventMask e, navNode& nav, prompt& item);
    result setDamperMin         (eventMask e, navNode& nav, prompt& item);
    result setDamperMax         (eventMask e, navNode& nav, prompt& item);
    result setTempControlSource (eventMask e, navNode& nav, prompt& item);
	result setIdle              (menuOut& o, idleEvent e);
    MenuUI                      (Screen* display, AppState *appState, ControlValues *controlValues);
	void init                   ();
	void draw                   ();
	bool update                 (MenuState &menuState);
	void setControlValues       ();
	void getControlValues       ();
};


