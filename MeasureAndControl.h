// MeasureAndControl.h

#pragma once

#include "arduino.h"
#include <ArduinoLog.h>
#include <PID_v2.h>

#include "ADC.h"
#include "ThermoCouple.h"
#include "TripleBuffer.h"
#include "State.h"
#include "Thermistor.h"


struct MeasurementData {
public:
    float Temperature[4];
    float targetTemperature;
};

class MeasureAndControl
{
private:
    PID_v2      pidControl;
    ADC          adc;
    ThermoCouple thermo1;
    ThermoCouple thermo2;
    Thermistor   thermistor1;
    Thermistor   thermistor2;
    TripleBuffer<MeasurementData> measurements;
    State<float> targetTemperature;
    static void ConfigureThermoSensor(ThermoCouple& thermo);
    static void setServo(float servoTarget);
public:
    MeasureAndControl(const MeasureAndControl& other)            = delete;
    MeasureAndControl& operator=(const MeasureAndControl& other) = delete;
    MeasureAndControl();
    ~MeasureAndControl();
    void update();
    MeasurementData* GetMeasurements();
    static float ReadTemperature(ThermoCouple& thermo);
    static float ReadTemperature(Thermistor& thermistor);
    void SetTargetTemperature(float targetTemp);
};
