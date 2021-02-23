#include <Wire.h>
#include "ADC.h"


// Constructor: _address is I2C address
ADC::ADC(byte address) 
{
  _address = address;
};

// Destructor
ADC::~ADC(){  };

// Set configuration
bool ADC::configure(ADCConfig ac)
{
    Wire.beginTransmission(_address);
    Wire.write((byte) ADCCommand::WriteConfigurationRegister);
    Wire.write((byte) ac.config);
    return (Wire.endTransmission(true) == 0);
}

// Test whether a new ADC conversion is ready. Flag is reset after reading value!
bool ADC::readConfiguration(ADCConfig &result)
{
  Wire.beginTransmission(_address);   
  Wire.write((byte)ADCCommand::ReadConfigurationRegister);  
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom(_address, (byte)1) != 1)
  {
    return false;    
  }   
  result.config = Wire.read();
  return true;
}

// Reset device
bool ADC::reset()
{
  Wire.beginTransmission(_address);   
  Wire.write((byte)ADCCommand::Reset);  
  return (Wire.endTransmission(true) == 0);
}


// Start a single or continuous conversion (depending on configuration register). Also 'reverses' shutdown.
bool ADC::start()
{
  Wire.beginTransmission(_address);   
  Wire.write((byte)ADCCommand::Start);  
  return (Wire.endTransmission(true) == 0);
}

// Power down. Start() can start a conversion from power down
bool ADC::powerDown()
{
  Wire.beginTransmission(_address);   
  Wire.write((byte)ADCCommand::PowerDown);  
  return (Wire.endTransmission(true) == 0);
}

// Test whether a new ADC conversion is ready. Flag is reset after reading value!
bool ADC::conversionReady(bool &result)
{
  Wire.beginTransmission(_address);   
  Wire.write((byte)ADCCommand::ReadStatusRegister);  
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom(_address, (byte)1) != 1)
  {
    return false;    
  }   
  result = (Wire.read()&0x80) != 0;
  return true;
}

// read conversion value
bool ADC::read(int16_t &value)
{
  Wire.beginTransmission(_address);   
  Wire.write((byte)ADCCommand::ReadData);  
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom(_address, (byte)2) != 2)
  {
    return false;    
  }   
  value = (Wire.read() << 8) + Wire.read();
  return true;
}
