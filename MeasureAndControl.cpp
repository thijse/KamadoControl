#include "MeasureAndControl.h"
#include "ArduinoLog.h"

void MeasureAndControl::init(QueueHandle_t *mutex)
{
    _mutex = mutex;
}

MeasureAndControl::MeasureAndControl() :
    pidControl(2, 5, 1, PID::Direct, PID::P_On::Measurement),
    adc        (0x40   ),
    thermo1    (0x60   ),
    thermo2    (0x60   ),
    thermistor (adc    )
{
    ConfigureThermoSensor(thermo1);
    ConfigureThermoSensor(thermo2);    

    pidControl.Start(
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
    const float targetTemp = targetTemperature.get();

    // Todo: more fine grained lock
    MeasurementData* measurement = measurements.getWriteBuffer();
    if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
    {
        thermo1   .readTemperature(0, measurement->temperatureResults);
        thermo1   .readTemperature(1, measurement->temperatureResults);
        thermistor.readTemperature(2, measurement->temperatureResults);        
        xSemaphoreGive(*_mutex);
    }

    measurement->targetTemperature = targetTemp;
    measurements.releaseWriteBuffer();

    pidControl.Setpoint(targetTemp);
    const float servoTarget = (float)pidControl.Run(measurement->temperatureResults.temperature[3]);
    setServo(servoTarget);   
}

void MeasureAndControl::setServo(float servoTarget)
{
    // todo set servo
}

/// <summary>
///  Returns buffered measurement data. Will return nullptr if no new data since last request.
/// </summary>
/// <returns></returns>
MeasurementData* MeasureAndControl::GetMeasurements()
{
    return measurements.getNewReadBuffer();
}

void MeasureAndControl::ConfigureThermoSensor(ThermoCouple& thermo)
{
    TCSensorConfig sensorConfig{};
    sensorConfig.setFilterCoefficients(0);
    sensorConfig.setThermoCoupleType(TCThermoCoupleType::K);

    if (!thermo.setSensorConfiguration(sensorConfig)) {Log.errorln("Failed setting sensor configuration"); } else { Log.noticeln("Sensor configured"); }
}

float MeasureAndControl::ReadTemperature(ThermoCouple& thermoCouple)
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

void MeasureAndControl::SetTargetTemperature(float targetTemp)
{
    targetTemperature.set(targetTemp);
}
