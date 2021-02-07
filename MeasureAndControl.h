// MeasureAndControl.h

#pragma once

#include "arduino.h"
#include <PID_v2.h>
#include "ThermoCouple.h"
#include "TripleBuffer.h"
#include "Property.h"


struct MeasurementData {
public:
    float Temperature[4];
    float targetTemperature;
};

class MeasureAndControl
{
private:
    PID_v2 pidControl;
    ThermoCouple thermo1;
    ThermoCouple thermo2;
    TripleBuffer<MeasurementData> measurements;
    Property<float> targetTemperature;
    void ConfigureThermoSensor(ThermoCouple& thermo);
    void setServo(float servoTarget);
public:
    MeasureAndControl();
    ~MeasureAndControl();
    void update();
    MeasurementData* GetMeasurements();
    float ReadTemperature(ThermoCouple& thermo);
    void SetTargetTemperature(float targetTemp);
};
