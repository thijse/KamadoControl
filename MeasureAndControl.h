// MeasureAndControl.h

#pragma once

#include "arduino.h"
#include <ArduinoLog.h>
#include <PID_v2.h>
#include "Global.h"
#include "ADC.h"
#include "DamperControl.h"
#include "ThermoCouple.h"
#include "TemperatureResults.h"
#include "Thermistor.h"

struct MeasurementData {
public:
    TemperatureResults temperatureResults;
    volatile float              targetTemperature;
    volatile int                damperValue;
};

class MeasureAndControl
{
private:
    SemaphoreHandle_t              _mutex;
    ControlValues                  _prevControlValues;
    ControlValues                 *_controlValues;
    MeasurementData               *_measurements;
    PID_v2                         _pidControl;
    ADC                            _adc;
    ThermoCouple                   _thermo1;
    ThermoCouple                   _thermo2;
    Thermistor                     _thermistor;
    DamperControl                  _damper;
    
    float                          _targetTemperature;
   
    int                            _lastDamperVal        = 90;

    static void configureThermoSensor(ThermoCouple& thermo);

public:
    MeasureAndControl                (const MeasureAndControl& other) = delete;
    MeasureAndControl& operator=     (const MeasureAndControl& other) = delete;
    MeasureAndControl                (SemaphoreHandle_t mutex, ControlValues *controlValues, MeasurementData *measurements);
    ~MeasureAndControl               ();
    void update                      ();
    void updateControl               ();
    static float readTemperature     (ThermoCouple& thermo);
};
