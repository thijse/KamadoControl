#include <Wire.h>
#include "Thermistor.h"

/// <summary>
/// Initialize Thermocouple
/// </summary>
/// <param name="adc">ADC to be used</param>
/// <param name="channel">Channel on ADC to use</param>
Thermistor::Thermistor(ADC *adc, uint8_t channel) :
    _adc(adc),
    _channel(channel)    
{    
};

/// <summary>
/// Destructor
/// </summary>
Thermistor::~Thermistor()
{  
};

bool Thermistor::setupAndStartADC()
{   
    _adcConfig.SetVoltageReference(ADCVoltageReference::External);
    _adcConfig.SetConversionMode  (ADCConversionMode::SingleShot);
    _adcConfig.SetConversionRate  (ADCConversionRate::SPS90);
    _adcConfig.SetGain            (ADCGain::G1);
    _adcConfig.SetMux             (_channel);
    if (!_adc->configure(_adcConfig) && _adc->start())
    {
        Log.errorln(F("Error configuring ADC (%02X)"), _adcConfig.config);
        return false;
    }
    else
    {
        if (!_adc->readConfiguration(_adcConfig))
        {
            Log.errorln(F("Error reading ADC configuration"));
            return false;
        }
    }
    return true;
};


/// <summary>
/// Get temperature
/// </summary>
/// <param name="temperature">Return measured temperature</param>
/// <returns>Returns true on success</returns>
bool Thermistor::getTemperature(float &temperature)
{
    bool result          = true;
    int16_t value        = 0;
    bool success         = setupAndStartADC();

    if (!success) { return false; }

    vTaskDelay(11.0 / portTICK_PERIOD_MS); // should be just enough to finish conversion

    do
    {
        success = _adc->conversionReady(result);
        if (!result)
        {
            vTaskDelay(1);
        }
    } while (success && !result);

    // If stopped, either unsuccessful or a result was achieved`

    if (!success)
    {
        Log.errorln(F("Error waiting for conversion"));
        _adc->powerDown();
        return false;
    }
    success = _adc->read(value);
    if (!success)
    {
        Log.errorln(F("Error reading  value"));
        _adc->powerDown();
        return false;
    }

    // todo: convert ADC value to temperature
    temperature = (float)value;

    if (!_adc->powerDown())
    {
        Log.errorln(F("Error placing ADC in shutdown mode"));
    }

    return true;
}