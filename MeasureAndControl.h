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
    float targetTemperature;
};

class MeasureAndControl
{
private:
    PID_v2      pidControl;
    ADC          adc;
    ThermoCouple thermo1;
    ThermoCouple thermo2;
    Thermistor   thermistor;
    TripleBuffer<MeasurementData> measurements;
    State<float> targetTemperature;
    QueueHandle_t *_mutex;
    static void ConfigureThermoSensor(ThermoCouple& thermo);
    static void setServo(float servoTarget);
public:
    MeasureAndControl(const MeasureAndControl& other)            = delete;
    MeasureAndControl& operator=(const MeasureAndControl& other) = delete;
    void init(QueueHandle_t *mutex);
    MeasureAndControl();
    ~MeasureAndControl();
    void update();
    MeasurementData* GetMeasurements();
    static float ReadTemperature(ThermoCouple& thermo);

    void SetTargetTemperature(float targetTemp);
};
