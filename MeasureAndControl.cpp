#include "MeasureAndControl.h"
#include <PID_v2.h>
#include "ArduinoLog.h"


MeasureAndControl::MeasureAndControl(SemaphoreHandle_t mutex, ControlValues *controlValues) :
    _mutex        (mutex),
    _controlValues(controlValues),
    _pidControl   (1.0,60.0, 0.0, PID::Direct, PID::P_On::Error),
    _adc          (0x40, mutex),
    _thermo1      (0x60, mutex),
    _thermo2      (0x60, mutex),
    _thermistor   (_adc)
{
    configureThermoSensor      (_thermo1);
    configureThermoSensor      (_thermo2);
    _damper.setActive          (true);
    _pidControl.SetOutputLimits(
        15,        // Minimum output
        125);      // Maximum output
    _pidControl.Start          (
        20.0f,     // input
        0,         // current output
        100.0f);   // set-point

}

MeasureAndControl::~MeasureAndControl()
{
}

/// <summary>
/// Run loop to measure and control
/// </summary>
void MeasureAndControl::update()
{
    // Update control values from main thread
    updateControl();
   
    // Control damper
    if (_temperatureMeasure) {
        // Fill measurement buffer and release
        MeasurementData* measurement = _measurements.getWriteBuffer();
        _thermo1.   readTemperature(0, measurement->temperatureResults);
        _thermo1.   readTemperature(1, measurement->temperatureResults);
        _thermistor.readTemperature(2, measurement->temperatureResults);
        measurement->targetTemperature = _targetTemperature;
        _measurements.releaseWriteBuffer();

        if (_temperatureControl) {
            const float damperValue = (float)_pidControl.Run(measurement->temperatureResults.temperature[_tempControlSource]);
            Log.noticeln(F("current value=%F, damper value+%F, P=%F, I=%F, D=%F"), measurement->temperatureResults.temperature[_tempControlSource], damperValue, _pidControl.GetLastP(), _pidControl.GetLastI(), _pidControl.GetLastD());
            _damper.setOpen(damperValue);
        }
    }
    else {
        // set manually defined damper
        _damper.setOpen(_damperVal);
    }
    

//    vTaskDelay((_delayNextMeasurement?2000:100) / portTICK_PERIOD_MS);
    
}

void MeasureAndControl::updateControl()
{
    // Locking values, but this is all very fast so no delay in the main thread)
    _controlValues->lock();    
    if (_controlValues->P != _pidP || _controlValues->I != _pidI || _controlValues->D != _pidD) { _pidP = _controlValues->P; _pidI = _controlValues->I; _pidD = _controlValues->D; _pidControl.SetTunings(_pidP, _pidI, _pidD); Serial.println("Setting PID"); }
    if (_controlValues->damperMin         != _damperMin)                                        { _damperMin = _controlValues->damperMin;                           setDamperMin(); Serial.println("Setting damperMin");    }
    if (_controlValues->damperMax         != _damperMax)                                        { _damperMax = _controlValues->damperMax;                           setDamperMax(); Serial.println("Setting damperMax");    }
    if (_controlValues->damperVal         != _damperVal)                                        { _damperVal = _controlValues->damperVal;                                          Serial.println("Setting damperVal"); }
    if (_controlValues->targetTemperature != _targetTemperature)                                { _targetTemperature = _controlValues->targetTemperature;          _pidControl.Setpoint(_targetTemperature); Serial.println("Setting target temp");    }
    if (_controlValues->tempControlSource != _tempControlSource)                                { _tempControlSource = _controlValues->tempControlSource;   Serial.println("Setting tempControlSource");    }   
    
    
    _temperatureControl   =  _controlValues->temperatureControl && !_controlValues->calibrateDamperMinMax;  // Update temperatureControl
    _temperatureMeasure   = !_controlValues->calibrateDamperMinMax;
    _delayNextMeasurement = !_controlValues->calibrateDamperMinMax;

    _controlValues->unlock();
}

void MeasureAndControl::setDamperMin()
{
    _pidControl.SetOutputLimits(_damperMin, _damperMax);
    Serial.println("Setting damper min");
}

void MeasureAndControl::setDamperMax()
{
    _pidControl.SetOutputLimits(_damperMin, _damperMax);
    Serial.println("Setting damper max");
    
}

void MeasureAndControl::setDamperVal(int damperValue)
{
    Serial.println("Setting damper :"); Serial.println(damperValue);
    _damper.setOpen(damperValue);
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

    if (!thermo.setSensorConfiguration(sensorConfig)) { Log.errorln("Failed setting sensor configuration"); }
    else { Log.noticeln("Sensor configured"); }
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
