// SetTemperature
#pragma once 
#include "arduino.h"
#include "Screen.h"

class Set
{
protected:
	Screen* _display;
	const int                                         _sdCardPin;
	SPIClass* _spiClass;
	int                                               _posX;
	int                                               _posY;
	int												 _prevCardSize;
public:
	SDCard(
		Screen* display,
		const int   sdCardPin);
	void init();
	void update();
	void draw();
};




