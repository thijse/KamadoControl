#pragma once 
#include "arduino.h"
#include "ADC.h"
#include "ArduinoLog.h"

struct ThermistorsResult
{
    float temperature[4]{};
    bool success     [4]{};
};

struct ThermistorResult
{
    float temperature;
    bool success;
};

class Thermistor {
      
  public:
      Thermistor(const Thermistor& other)            = delete;
      Thermistor& operator=(const Thermistor& other) = delete;

      // Constructor
      Thermistor(ADC &adc);
      // Destructor
      ~Thermistor();
      ThermistorsResult readTemperature();
      ThermistorResult  readTemperature(byte channel);
      bool              readTemperature(byte channel, float& value);            

  private:
      ADC              &_adc;
      uint8_t           _channel;
      ADCConfig         _adcConfig{};
      bool              setupChannel(byte channel);
      float             adcToTemperature(uint8_t adcValue);
};

