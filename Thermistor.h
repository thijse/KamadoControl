#pragma once 
#include "arduino.h"
#include "ADC.h"
#include "ArduinoLog.h"


// Class doing the work
class Thermistor {
      
  public:
      Thermistor(const Thermistor& other)            = delete;
      Thermistor& operator=(const Thermistor& other) = delete;

      // Constructor
      Thermistor(ADC *adc,uint8_t channel);
      // Destructor
      ~Thermistor();
      // Setup and start ADC
      bool setupAndStartADC();      
      // Get temperature in degrees
      bool getTemperature(float &temperature);
     
  private:
      ADC      *_adc;
      uint8_t   _channel;
      ADCConfig _adcConfig{};
};

