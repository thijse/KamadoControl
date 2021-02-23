#pragma once
// Time
#pragma once 
#include "arduino.h"

#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include "Screen.h"

class Logo
{
protected:
	Screen* _display;
	int           _posX;
	int           _posY;
	int           _width;
	int           _height;

	const static unsigned char lilygo[];
public:
	Logo(const Logo& other)            = delete;
	Logo& operator=(const Logo& other) = delete;
	Logo(Screen* display);
	void init();
	void update();
	void draw();
};

