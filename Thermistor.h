#pragma once 
#include "arduino.h"
#include "ADC.h"
#include "ArduinoLog.h"
#include "TemperatureResults.h"
#include "ThermistorConversion.h"


class Thermistor {
      
  public:
      Thermistor           (const Thermistor& other) = delete;
      Thermistor& operator=(const Thermistor& other) = delete;

      // Constructor
      Thermistor(ADC &adc);
      // Destructor
      ~Thermistor();
      void              readTemperature(int no,TemperatureResults& temperatureResults);
      bool              readTemperature(byte channel, volatile float& value);            

  private:
      ADC                 &_adc;
      uint8_t              _channel;
      ADCConfig            _adcConfig{};
      ThermistorConversion _conversion;
      bool              setupChannel(byte channel);
      float             adcToTemperature(uint8_t adcValue);
};

