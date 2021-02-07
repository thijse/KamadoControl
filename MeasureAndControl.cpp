#include "MeasureAndControl.h"

MeasureAndControl::MeasureAndControl() :
    pidControl(2, 5, 1, PID::Direct, PID::P_On::Measurement),
    thermo1   (0x60),
    thermo2   (0x60)
{
    ConfigureThermoSensor(thermo1);
    ConfigureThermoSensor(thermo2);

    // todo: move to init ?
    pidControl.Start(
        20.0f,    // input
        0,        // current output
        20.0f);   // setpoint
}

MeasureAndControl::~MeasureAndControl()
{
}

/// <summary>
/// Run loop to measure and control
/// </summary>
void MeasureAndControl::update()
{
    float targetTemp = targetTemperature.get();

    // Todo: store per measurement status (success, failure)
    MeasurementData* measurement   = measurements.getWriteBuffer();
    measurement->Temperature[0]    = 1.0f; //ReadTemperature(thermo1);
    measurement->Temperature[1]    = 2.0f; //ReadTemperature(thermo2);
    measurement->Temperature[2]    = 3.0f;
    measurement->Temperature[3]    = 4.0f;
    measurement->targetTemperature = targetTemp;
    measurements.releaseWriteBuffer();

    pidControl.Setpoint(targetTemp);
    float servoTarget = pidControl.Run(measurement->Temperature[0]);
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
    TCSensorConfig sconfig;
    sconfig.SetFilterCoefficients(0);
    sconfig.SetThermoCoupleType(TCThermoCoupleType::K);

    if (!thermo.SetSensorConfiguration(sconfig)) {Serial.println("Failed setting sensor configuration"); } else { Serial.println("Sensor configured"); }
}

/// <summary>
/// Read temperature:
// - Clear status byte (burst mode bit anyway)
// - Configure device for a single conversion at 16 bits. Manual says 80 ms conversion, ~12 ms calculation resulting in 92 ms total conversion time. Actually seems to be a bit less than 110 ms...
// - Wait 110 ms (vxDelay in future, multitasking and such!)
// - Check status byte for 'burst complete'. Should be done 'instantly'.
// - Read temperature (degrees);
/// </summary>
/// <param name="thermo">return </param>
float MeasureAndControl::ReadTemperature(ThermoCouple& thermo)
{
    int n;

    // Setup for single 16 bit conversion
    TCDeviceConfig config;
    config.SetShutdownMode(TCShutdownMode::Burst);
    config.SetBurstModeSamples(1);
    config.SetMeasurementResolution(TCMeasurementResolution::Bit16);

    if (!thermo.ClearStatus() || !thermo.SetDeviceConfiguration(config))
    {
        Serial.println("Failed starting conversion");
    }
    else
    {
        TCStatus stat;
        delay(110);
        for (n = 0; n < 2; n++)
        {
            if (!thermo.GetStatus(stat) || stat.BurstComplete()) break;
            delay(10);
        }
        if (!stat.BurstComplete())
        {
            //sprintf(buf, "Failed while waiting for conversion (%d of 2 attempts)", n);
        }
        else
        {
            int temp;
            if (!thermo.GetTemperature(temp))
            {
                Serial.println("Failed reading temperature");
            }
            else
            {
                //sprintf(buf, "Temperature (1/16th of a degree): %d", temp);
                return temp*6.25E-2;
            }
        }
    }
}

void MeasureAndControl::SetTargetTemperature(float targetTemp)
{
    targetTemperature.set(targetTemp);
}
