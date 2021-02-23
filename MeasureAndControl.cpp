#include "MeasureAndControl.h"
#include "ArduinoLog.h"

MeasureAndControl::MeasureAndControl() :
    pidControl(2, 5, 1, PID::Direct, PID::P_On::Measurement),
    adc        (0x40   ),
    thermo1    (0x60   ),
    thermo2    (0x60   ),
    thermistor1(&adc, 0),
    thermistor2(&adc, 1)
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

    // Todo: store per measurement status (success, failure)
    MeasurementData* measurement   = measurements.getWriteBuffer();
    measurement->Temperature[0]    = ReadTemperature(thermo1    );
    measurement->Temperature[1]    = ReadTemperature(thermo2    );
    measurement->Temperature[2]    = ReadTemperature(thermistor1);
    measurement->Temperature[3]    = ReadTemperature(thermistor2);
    measurement->targetTemperature = targetTemp;
    measurements.releaseWriteBuffer();

    pidControl.Setpoint(targetTemp);
    const float servoTarget = (float)pidControl.Run(measurement->Temperature[0]);
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
    const bool success = thermoCouple.getTemperature(temperature);
    if (!success)
    {
        Log.errorln(F("Failed reading thermocouple"));
        return 0;
    }
    return temperature;
}

float MeasureAndControl::ReadTemperature(Thermistor& thermistor)
{
    float temperature;
    const bool success = thermistor.getTemperature(temperature);
    if (!success)
    {
        Log.errorln(F("Failed reading thermistor"));
        return 0;
    }
    return temperature;
}

void MeasureAndControl::SetTargetTemperature(float targetTemp)
{
    targetTemperature.set(targetTemp);
}
