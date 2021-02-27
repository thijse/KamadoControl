#include <Wire.h>
#include "Thermistor.h"

/// <summary>
/// Initialize Thermocouple
/// </summary>
/// <param name="adc">ADC to be used</param>
/// <param name="channel">Channel on ADC to use</param>
Thermistor::Thermistor(ADC &adc) :
    _adc(adc)
{    
};

/// <summary>
/// Destructor
/// </summary>
Thermistor::~Thermistor()
{  
};

bool Thermistor::setupChannel(byte channel)
{
    ADCConfig adcConfig;
    adcConfig.SetVoltageReference(ADCVoltageReference::External);
    adcConfig.SetConversionMode  (ADCConversionMode::SingleShot);
    adcConfig.SetConversionRate  (ADCConversionRate::SPS90);
    adcConfig.SetGain            (ADCGain::G1);
    adcConfig.SetMux             (channel);
    return _adc.configure(adcConfig) && _adc.start();
}

float Thermistor::adcToTemperature(uint8_t adcValue)
{
}

ThermistorResult Thermistor::readTemperature(byte channel)
{
    ThermistorResult thermistorResult;    
    thermistorResult.success = readTemperature(channel, thermistorResult.temperature);
    return thermistorResult;
}


ThermistorsResult Thermistor::readTemperature()
{
    ThermistorsResult thermistorsResult;   

    for (byte channel = 0; channel < 4; channel++)
    {
        thermistorsResult.success[channel] = readTemperature(channel, thermistorsResult.temperature[channel]);
    }

    if (!_adc.powerDown())
    {
        Log.warningln(F("Error placing ADC in shutdown mode"));
    }
    return thermistorsResult;
}

bool Thermistor::readTemperature(byte channel, float& temperature)
{
    bool result, success = true;
    int16_t value = 0;
    if (!setupChannel(channel))
    {
        Log.warningln(F("Error starting conversion"));
        return false;
    }
    vTaskDelay(11.0 / portTICK_PERIOD_MS); // should be just enough to finish conversion
    
    do
    {
        success = _adc.conversionReady(result);
        if (!result) { vTaskDelay(1); } // continue checking until conversion is finalized
    } while (success && !result);

    // Stopped because of either result or no success in conversion, check which one
    if (!success)
    {
        Log.warningln(F("Error waiting for conversion"));
        return false;
    }
    success = _adc.read(value);
    if (!success)
    {
        Log.warningln(F("Error reading conversion value"));
        return false;
    }
    temperature = (float)value;
    return true;
}
