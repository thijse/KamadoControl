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
	int           _posX;
	int           _posY;
	int16_t       _x;
	int16_t       _y;
	uint16_t      _w;
	uint16_t      _h;
	unsigned long _time_start_ms;
	char          _prevTime[6];
public:
	Timer           (const Timer& other) = delete;
	Timer& operator=(const Timer& other) = delete;
	Timer(Screen* display);
	void init();
	void update();
	void draw();
};




