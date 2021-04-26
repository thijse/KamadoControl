#include "MeasureAndControl.h"
#include <PID_v2.h>
#include "ArduinoLog.h"


MeasureAndControl::MeasureAndControl(SemaphoreHandle_t mutex, ControlValues *controlValues, MeasurementData *measurements) :
    _mutex            (mutex),
    _controlValues    (controlValues),
    _measurements     (measurements),
    _pidControl       (1.0, 60.0, 0.0, PID::Direct, PID::P_On::Error),
    _adc              (0x40, mutex),
    _thermo1          (0x60, mutex),
    _thermo2          (0x60, mutex),
    _thermistor       (_adc),
    _targetTemperature(0)
{
    configureThermoSensor(_thermo1);
    configureThermoSensor(_thermo2);
    _damper.setActive    (true);
    _pidControl.SetOutputLimits(
        15, // Minimum output
        125); // Maximum output
    _pidControl.Start(
        20.0, // input
        0, // current output
        100.0); // set-point
}

MeasureAndControl::~MeasureAndControl()
{}


/// <summary>
/// Run loop to measure and control
/// </summary>
void MeasureAndControl::update()
{
    // Update control values from main thread
    updateControl();

    
    if (_controlValues->damperMode == DamperMode::damperModeAutomatic) {
        _thermo1.readTemperature(0, _measurements->temperatureResults);
        //_thermo1.   readTemperature(1, _measurements->temperatureResults); // todo make menu setting to turn on & off settings
        _thermistor.readTemperature(2, _measurements->temperatureResults);
    }

    // Control damper
    int damperValue;
    switch (_controlValues->damperMode)
        {
        case DamperMode::damperModeAutomatic:
            _pidControl.SetMode(PID::Automatic);
            _pidControl.Setpoint(_controlValues->targetTemperature);
            damperValue = (int)_pidControl.Run((double)_measurements->temperatureResults.temperature[_controlValues->tempControlSource]);
            _controlValues->damperVal = damperValue;  // setting damper value back as control value. Can e.g. be used and changed in Manual mode
            Log.noticeln(F("Mode auto, damper value %d, current value=%F,  P=%F, I=%F, D=%F"), damperValue, _measurements->temperatureResults.temperature[_controlValues->tempControlSource], _pidControl.GetLastP(), _pidControl.GetLastI(), _pidControl.GetLastD());
            break;
        case DamperMode::damperModeManual:
            _pidControl.SetMode(PID::Manual);            
            damperValue = _controlValues->damperVal;            
            break;
        case DamperMode::damperModeMax:
            damperValue = _controlValues->damperMax;
            _pidControl.SetOutputLimits(_controlValues->damperMin, _controlValues->damperMax);
            break;
        case DamperMode::damperModeMin:            
            damperValue = _controlValues->damperMin;
            _pidControl.SetOutputLimits(_controlValues->damperMin, _controlValues->damperMax);
            break;
        default:
            break;
        }

        _damper.setOpen(damperValue);
    
        _measurements->damperValue       = damperValue;
        _measurements->targetTemperature = _controlValues->targetTemperature;

}

void MeasureAndControl::updateControl()
{
    if (_controlValues->P != _prevControlValues.P || 
        _controlValues->I != _prevControlValues.I || 
        _controlValues->D != _prevControlValues.D)                                 { _pidControl.SetTunings(_controlValues->P, _controlValues->I, _controlValues->D); Log.noticeln(F("Setting PID"));               }
    if (_controlValues->damperMin         != _prevControlValues.damperMin)         {                                                                                  Log.noticeln(F("Setting damperMin"));         }
    if (_controlValues->damperMax         != _prevControlValues.damperMax)         {                                                                                  Log.noticeln(F("Setting damperMax"));         }
    if (_controlValues->damperVal         != _prevControlValues.damperVal)         {                                                                                  Log.noticeln(F("Setting damperVal"));         }
    if (_controlValues->targetTemperature != _prevControlValues.targetTemperature) {                                                                                  Log.noticeln(F("Setting target temp"));       }
    if (_controlValues->tempControlSource != _prevControlValues.tempControlSource) {                                                                                  Log.noticeln(F("Setting tempControlSource")); }
        
    memcpy(&_prevControlValues, _controlValues, sizeof(_prevControlValues)); // copy to previous values for comparison    
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
