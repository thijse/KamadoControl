// MainValuesUI
#pragma once 
#include "arduino.h"
#include <ArduinoLog.h>
#include "MeasureAndControl.h"
#include "Global.h"
#include "Screen.h"


class MainValuesUI
{
protected:
	Screen*  _display;
	ControlValues  *_controlValues;
	MeasurementData *_measurements;
	int      _posTempX;
	int      _posTempY;
	int      _posXDamper;
	int      _posYDamper;

	int16_t  _xTemp{};
	int16_t  _yTemp{};
	uint16_t _wTemp{};
	uint16_t _hTemp{};

	int16_t  _xDamper{};
	int16_t  _yDamper{};
	uint16_t _wDamper{};
	uint16_t _hDamper{};

	int16_t  _targetTemperature;
	float    _currentTemperature;
	int		 _damperValue;


public:
	MainValuesUI           (const MainValuesUI& other) = delete;
	MainValuesUI& operator=(const MainValuesUI& other) = delete;    
    MainValuesUI           (Screen* display, ControlValues* controlValues, MeasurementData* measurements);
	
	
	void init                    ();   
    void draw                    ();
	void update					 (int16_t targetTemperature);
	int16_t getTargetTemperature () const;
	void    setCurrentTemperature(float currentTemperature, int damperValue);
	void    setDamperValue(int damperValue);
};