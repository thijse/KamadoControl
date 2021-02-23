// SetTemperature
#pragma once 
#include "arduino.h"
#include <ArduinoLog.h>
#include "Constants.h"
#include "RotaryEncoder.h"
#include "Screen.h"



class SetTemperature
{
protected:
	Screen* _display;
	int                  _posX;
	int                  _posY;
	int16_t              _x{};
	int16_t              _y{};
	uint16_t             _w{};
	uint16_t             _h{};
	
	int16_t              _temp;

public:
	SetTemperature(const SetTemperature& other) = delete;
	SetTemperature& operator=(const SetTemperature& other) = delete;
	SetTemperature(Screen* display);
	
	RotaryEncoder _rotaryEncoder;
	void init();
	void update();
    void rotaryInput();
    void draw();
	int16_t getTargetTemperature() const;
};