// SetTemperature
#pragma once 
#include "arduino.h"
#include "Screen.h"

class SetTemperature
{
protected:
	Screen* _display;
	const int                                         _sdCardPin;
	SPIClass* _spiClass;
	int                                               _posX;
	int                                               _posY;
	int												 _prevCardSize;
public:
	SetTemperature(
		Screen* display
		);
	void init();
	void update();
	void draw();
};




