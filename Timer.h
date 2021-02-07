// Time
#pragma once 
#include "arduino.h"

#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include "Screen.h"

class Timer
{
protected:
	Screen*       _display;
	int16         _t  _x;
	int16         _t  _y;
	uint16        _t _w;
	uint16        _t _h;
	unsigned long _time_start_ms;
	char          _prevTime[6];
public:
	Timer(Screen* display);
	void init();
	void update();
	void draw();
};




