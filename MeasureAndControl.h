// MeasureAndControl.h

#pragma once

#include "arduino.h"
#include <ArduinoLog.h>
#include <PID_v2.h>
#include "Global.h"
#include "ADC.h"
#include "DamperControl.h"
#include "ThermoCouple.h"
#include "TripleBuffer.h"
#include "State.h"
#include "TemperatureResults.h"
#include "Thermistor.h"

struct MeasurementData {
public:
    TemperatureResults temperatureResults;
    float              targetTemperature;
};

class MeasureAndControl
{
private:
    SemaphoreHandle_t             _mutex;
    PID_v2                        _pidControl;
    ADC                           _adc;
    ThermoCouple                  _thermo1;
    ThermoCouple                  _thermo2;
    Thermistor                    _thermistor;
    DamperControl                 _damper;
    TripleBuffer<MeasurementData> _measurements;
    float                         _targetTemperature;
    ControlValues                *_controlValues;

    int                           _tempControlSource  = 0;
    bool                          _temperatureMeasure = true;
    bool                          _delayNextMeasurement = true;
    int                           _temperatureControl = HIGH;
    int                           _damperMin          = 0;
    int                           _damperMax          = 180;
    int                           _damperVal          = 90;
    double                        _pidP               = 1.0;
    double                        _pidI               = 60.0;
    double                        _pidD               = 0.0;

    static void configureThermoSensor(ThermoCouple& thermo);
    void        setDamperMin();
    void        setDamperMax();
    void        setDamperVal(int damperValue);

    void        powerCycleBoard      ();
public:
    MeasureAndControl                (const MeasureAndControl& other) = delete;
    MeasureAndControl& operator=     (const MeasureAndControl& other) = delete;
    MeasureAndControl                (SemaphoreHandle_t mutex, ControlValues *controlValues);
    ~MeasureAndControl               ();
    void update                      ();
    void updateControl               ();
    MeasurementData* getMeasurements ();
    static float readTemperature     (ThermoCouple& thermo);
    
};
