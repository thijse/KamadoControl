// Time
#pragma once 
#include "arduino.h"

#include <SD.h>
#include <SPI.h>
#include "Screen.h"

class SDCard
{
protected:
	Screen*     _display;
	const int   _sdCardPin;
	SPIClass   *_spiClass;
	int         _posX;
	int         _posY;
	int		    _prevCardSize;
public:
	SDCard(const SDCard& other) = delete;
	SDCard& operator=(const SDCard& other) = delete;
	SDCard(
		Screen* display,
		const int   sdCardPin);
	void init(SPIClass* spiClass);
	void update();
	void draw();
};