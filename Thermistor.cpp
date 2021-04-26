#include <Wire.h>
#include "Thermistor.h"

/// <summary>
/// Initialize Thermocouple
/// </summary>
/// <param name="adc">ADC to be used</param>
/// <param name="channel">Channel on ADC to use</param>
Thermistor::Thermistor(ADC &adc) :
    _adc(adc),
    _conversion(3.3, 3.3, 65535)
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

void Thermistor::readTemperature(int no, TemperatureResults &temperatureResults)
{
    
    // Use last 4 slots (2..5)
    for (byte channel = 0; channel < 4; channel++)
    {
        temperatureResults.success[channel+ no] = readTemperature(channel, temperatureResults.temperature[channel+ no]);
    }

    if (!_adc.powerDown())
    {
        Log.warningln(F("Error placing ADC in shutdown mode"));
    }
    return;
}

bool Thermistor::readTemperature(byte channel, volatile float& temperature)
{
    bool result, success = true;
    int16_t value = 0;
    if (!setupChannel(channel))
    {
        Log.warningln(F("Error starting conversion"));
        return false;
    }
    vTaskDelay(12.0 / portTICK_PERIOD_MS); // should be just enough to finish conversion
    
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
    temperature = _conversion.getTempCelsius(value);
    return true;
}



