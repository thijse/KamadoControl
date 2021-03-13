#include "MeasureAndControl.h"
#include "ArduinoLog.h"


MeasureAndControl::MeasureAndControl(SemaphoreHandle_t mutex) :
    _mutex(mutex),
    _pidControl(2, 0, 1, PID::Direct, PID::P_On::Measurement),
    _adc        (0x40, mutex),
    _thermo1    (0x60, mutex),
    _thermo2    (0x60, mutex),
    _thermistor (_adc    )
{
    configureThermoSensor(_thermo1);
    configureThermoSensor(_thermo2);    
    _damper.setActive(true);
    _pidControl.Start(
        20.0f,    // input
        0,        // current output
        20.0f);   // set-point
}

MeasureAndControl::~MeasureAndControl()
{
}

/// <summary>
/// Run loop to measure and control
/// </summary>
void MeasureAndControl::update()
{
    const float targetTemp = _targetTemperature.get();

    // Todo: more fine grained lock
    MeasurementData* measurement = _measurements.getWriteBuffer();
    _thermo1   .readTemperature(0, measurement->temperatureResults);
    _thermo1   .readTemperature(1, measurement->temperatureResults);
    _thermistor.readTemperature(2, measurement->temperatureResults);        


    measurement->targetTemperature = targetTemp;
    _measurements.releaseWriteBuffer();

    _pidControl.Setpoint(targetTemp);
    const float damperValue = (float)_pidControl.Run(measurement->temperatureResults.temperature[3]);
    _damper.setOpen(damperValue);
    
}

void MeasureAndControl::setServo(float servoTarget)
{
    // todo set servo
}

void MeasureAndControl::powerCycleBoard()
{

}

/// <summary>
///  Returns buffered measurement data. Will return nullptr if no new data since last request.
/// </summary>
/// <returns></returns>
MeasurementData* MeasureAndControl::getMeasurements()
{
    return _measurements.getNewReadBuffer();
}

void MeasureAndControl::configureThermoSensor(ThermoCouple& thermo)
{
    TCSensorConfig sensorConfig{};
    sensorConfig.setFilterCoefficients(0);
    sensorConfig.setThermoCoupleType(TCThermoCoupleType::K);

    if (!thermo.setSensorConfiguration(sensorConfig)) {Log.errorln("Failed setting sensor configuration"); } else { Log.noticeln("Sensor configured"); }
}

float MeasureAndControl::readTemperature(ThermoCouple& thermoCouple)
{
    float temperature;
    const bool success = thermoCouple.readTemperature(temperature);
    if (!success)
    {
        Log.errorln(F("Failed reading thermocouple"));
        return 0;
    }
    return temperature;
}

void MeasureAndControl::setTargetTemperature(float targetTemp)
{
    _targetTemperature.set(targetTemp);
}
