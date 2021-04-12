// SetTemperature
#pragma once 
#include "arduino.h"
#include <ArduinoLog.h>
#include "Global.h"
#include "RangedRotaryEncoder.h"
#include "Screen.h"


class SetTemperature
{
protected:
	Screen*              _display;
	RotaryEncoder*		 _rotaryEncoder;
	int                  _posX;
	int                  _posY;
	int16_t              _x{};
	int16_t              _y{};
	uint16_t             _w{};
	uint16_t             _h{};

	int                  _posXDamper;
	int                  _posYDamper;
	int16_t              _xDamper{};
	int16_t              _yDamper{};
	uint16_t             _wDamper{};
	uint16_t             _hDamper{};

	RangedRotaryEncoder  _rangedRotaryEncoder;
	int16_t              _targetTemperature;
	float                _currentTemperature;
	int					 _damperValue;

public:
	SetTemperature           (const SetTemperature& other) = delete;
	SetTemperature& operator=(const SetTemperature& other) = delete;
    
    SetTemperature(Screen* display, RotaryEncoder* rotaryEncoder);
	
	
	void init                    ();   
    void draw                    ();
	void update					 (MenuState &menuState);
	void rotaryInput             (MenuState& menuState);
	int16_t getTargetTemperature () const;
	void    setCurrentTemperature(float currentTemperature, int damperValue);
};