#include <Wire.h>
#include "ThermoCouple.h"

/// <summary>
/// Initialize Thermocouple
/// </summary>
/// <param name="_address">I2C Address</param>
ThermoCouple::ThermoCouple(uint8_t _address)
{
  this->address = _address;
};

/// <summary>
/// Destructor
/// </summary>
ThermoCouple::~ThermoCouple()
{  
};

/// <summary>
/// Get sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::GetSensorConfiguration(TCSensorConfig &config)
{
  if (!this->ReadRegister(TCRegister::SensorConfiguration, 1)) return false;
  config.config = Wire.read();
  return true;
};

/// <summary>
/// Set sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::SetSensorConfiguration(TCSensorConfig &config)
{
  return this->WriteRegister(TCRegister::SensorConfiguration, config.config);
};

/// <summary>
/// Get device configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::GetDeviceConfiguration(TCDeviceConfig &config)
{
  if (!this->ReadRegister(TCRegister::DeviceConfiguration, 1)) return false;
  config.config = Wire.read();
  return true;
}

/// <summary>
/// Set sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::SetDeviceConfiguration(TCDeviceConfig &config)
{
  return this->WriteRegister(TCRegister::DeviceConfiguration, config.config);
}

/// <summary>
/// Get status
/// </summary>
/// <param name="config"></param>
/// <returns>True on success, false on failure</returns> 
bool ThermoCouple::GetStatus(TCStatus &status)
{  
  if (!this->ReadRegister(TCRegister::Status, 1)) return false;
  status.status = Wire.read();
  return true;
};

/// <summary>
/// Clear Status
/// </summary>
/// <returns>Clear status. Returns true on success, false on failure</returns>
bool ThermoCouple::ClearStatus()
{  
  return this->WriteRegister(TCRegister::Status, 0);
};

/// <summary>
/// Get device ID (0x41 for our MCP96L01)
/// </summary>
/// <param name="id"></param>
/// <returns>True on success</returns>
bool ThermoCouple::GetDeviceID(byte &id)
{
  if (!this->ReadRegister(TCRegister::DeviceID, 1)) return false;
  id = Wire.read();
  return true;
}

/// <summary>
/// Get temperature
/// </summary>
/// <param name="temperature">Return measured temperature</param>
/// <returns>Returns true on success</returns>
bool ThermoCouple::GetTemperature(int &temperature)
{
    if (!this->ReadRegister(TCRegister::THigh, 2)) return false;
    temperature = (Wire.read()<<8) | Wire.read();
    return true;
}
  
/// <summary>
/// Get cold junction temperature
/// </summary>
/// <param name="temperature">Return cold junction temperature</param>
/// <returns>True on success</returns>
bool ThermoCouple::GetColdJunctionTemperature(int &temperature)
{
    if (!this->ReadRegister(TCRegister::TCold, 2)) return false;
    temperature = (Wire.read()<<8) | Wire.read();
    return true;
}

/// <summary>
/// Write Register
/// </summary>
bool ThermoCouple::WriteRegister(TCRegister reg, byte value)
{
  Wire.beginTransmission(this->address);   // talk to TC 
  Wire.write((byte) reg);   
  Wire.write(value);
  return Wire.endTransmission(true) == 0;  // send stop
};  

/// <summary>
/// Read Register
/// </summary>
bool ThermoCouple::ReadRegister(TCRegister reg, int count)
{
  Wire.beginTransmission(this->address);   // talk to TC
  Wire.write((byte) reg);   // status register
  Wire.endTransmission(true);             // send stop
  return (Wire.requestFrom((uint8_t)this->address, (uint8_t)count) == count);
};
