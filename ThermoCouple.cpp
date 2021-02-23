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
bool ThermoCouple::getSensorConfiguration(TCSensorConfig &config)
{
  if (!this->readRegister(TCRegister::SensorConfiguration, 1)) return false;
  config.config = Wire.read();
  return true;
};

/// <summary>
/// Set sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::setSensorConfiguration(TCSensorConfig &config)
{
  return this->writeRegister(TCRegister::SensorConfiguration, config.config);
};

/// <summary>
/// Get device configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::getDeviceConfiguration(TCDeviceConfig &config)
{
  if (!this->readRegister(TCRegister::DeviceConfiguration, 1)) return false;
  config.config = Wire.read();
  return true;
}

/// <summary>
/// Set sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::setDeviceConfiguration(TCDeviceConfig &config)
{
  return this->writeRegister(TCRegister::DeviceConfiguration, config.config);
}

/// <summary>
/// Get status
/// </summary>
/// <param name="status">Returns thermocouple status</param>
/// <returns>True on success, false on failure</returns> 
bool ThermoCouple::getStatus(TCStatus &status)
{  
  if (!this->readRegister(TCRegister::Status, 1)) return false;
  status.status = Wire.read();
  return true;
};

/// <summary>
/// Clear Status
/// </summary>
/// <returns>Clear status. Returns true on success, false on failure</returns>
bool ThermoCouple::clearStatus()
{  
  return this->writeRegister(TCRegister::Status, 0);
};

/// <summary>
/// Get device ID (0x41 for our MCP96L01)
/// </summary>
/// <param name="id"></param>
/// <returns>True on success</returns>
bool ThermoCouple::getDeviceID(byte &id)
{
  if (!this->readRegister(TCRegister::DeviceID, 1)) return false;
  id = Wire.read();
  return true;
}

bool ThermoCouple::getTemperature(float& temperature)
{
    int n       = 0;
    temperature = 0;
    // Setup for single 16 bit conversion
    TCDeviceConfig config{};
    config.setShutdownMode         (TCShutdownMode::Burst);
    config.setBurstModeSamples     (1);
    config.setMeasurementResolution(TCMeasurementResolution::Bit16);

    if (!clearStatus() || !setDeviceConfiguration(config))
    {
        Log.errorln(F("Failed starting conversion"));
        return false;
    }
    else
    {
        TCStatus stat{};
        delay(110);
        for (n = 0; n < 2; n++)
        {
            if (!getStatus(stat) || stat.burstComplete()) break;
            delay(10);
        }
        if (!stat.burstComplete())
        {
            Log.errorln(F("Failed while waiting for conversion (%d of 2 attempts)"), n);
        }
        else
        {
            int temp = 0;
            if (!getTemperatureRegister(temp))
            {
                Log.errorln(F("Failed reading temperature"));
                return false;
            }
            else
            {
                //sprintf(buf, "Temperature (1/16th of a degree): %d", temp);
                temperature = 6.25E-2f * (float)temp;
                return true;
            }
        }
    }
    return false;
}

/// <summary>
/// Get temperature
/// </summary>
/// <param name="temperature">Return measured temperature</param>
/// <returns>Returns true on success</returns>
bool ThermoCouple::getTemperatureRegister(int &temperature)
{
    if (!this->readRegister(TCRegister::THigh, 2)) return false;
    temperature = (Wire.read()<<8) | Wire.read();
    return true;
}
  
/// <summary>
/// Get cold junction temperature
/// </summary>
/// <param name="temperature">Return cold junction temperature</param>
/// <returns>True on success</returns>
bool ThermoCouple::getColdJunctionTemperature(int &temperature) const
{
    if (!this->readRegister(TCRegister::TCold, 2)) return false;
    temperature = (Wire.read()<<8) | Wire.read();
    return true;
}

/// <summary>
/// Write Register
/// </summary>
bool ThermoCouple::writeRegister(TCRegister reg, byte value) const
{
  Wire.beginTransmission(this->address);   // talk to TC 
  Wire.write((byte) reg);   
  Wire.write(value);
  return Wire.endTransmission(true) == 0;  // send stop
};  

/// <summary>
/// read Register
/// </summary>
bool ThermoCouple::readRegister(TCRegister reg, int count) const
{
  Wire.beginTransmission(this->address);   // talk to TC
  Wire.write((byte) reg);   // status register
  Wire.endTransmission(true);             // send stop
  return (Wire.requestFrom((uint8_t)this->address, (uint8_t)count) == count);
};
