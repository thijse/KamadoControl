// MeasureAndControl.h

#pragma once

#include "arduino.h"
#include <ArduinoLog.h>
#include <PID_v2.h>

#include "ADC.h"
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
    PID_v2                        _pidControl;
    ADC                           _adc;
    ThermoCouple                  _thermo1;
    ThermoCouple                  _thermo2;
    Thermistor                    _thermistor;
    TripleBuffer<MeasurementData> _measurements;
    State<float>                  _targetTemperature;
    SemaphoreHandle_t            *_mutex;
    static void configureThermoSensor(ThermoCouple& thermo);
    static void setServo             (float servoTarget);
    void        powerCycleBoard      ();
public:
    MeasureAndControl                (const MeasureAndControl& other)            = delete;
    MeasureAndControl& operator=     (const MeasureAndControl& other) = delete;
    MeasureAndControl                (SemaphoreHandle_t *mutex);
    ~MeasureAndControl               ();
    void update                      ();
    MeasurementData* getMeasurements ();
    static float readTemperature     (ThermoCouple& thermo);
    void setTargetTemperature        (float targetTemp);
};
