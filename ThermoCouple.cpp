#include <Wire.h>
#include "ThermoCouple.h"

/// <summary>
/// Initialize Thermocouple
/// </summary>
/// <param name="address">I2C Address</param>
ThermoCouple::ThermoCouple(uint8_t address)
{
  _address = address;
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
bool ThermoCouple::getSensorConfiguration(TCSensorConfig &config) const
{
  if (!readRegister(TCRegister::SensorConfiguration, 1)) return false;
  config.config = Wire.read();
  return true;
};

/// <summary>
/// Set sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::setSensorConfiguration(TCSensorConfig &config) const
{
  return writeRegister(TCRegister::SensorConfiguration, config.config);
};

/// <summary>
/// Get device configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::getDeviceConfiguration(TCDeviceConfig &config) const
{
  if (!readRegister(TCRegister::DeviceConfiguration, 1)) return false;
  config.config = Wire.read();
  return true;
}

/// <summary>
/// Set sensor configuration
/// </summary>
/// <param name="config"></param>
/// <returns></returns>
bool ThermoCouple::setDeviceConfiguration(TCDeviceConfig &config) const
{
  return writeRegister(TCRegister::DeviceConfiguration, config.config);
}

/// <summary>
/// Get status
/// </summary>
/// <param name="status">Returns thermocouple status</param>
/// <returns>True on success, false on failure</returns> 
bool ThermoCouple::getStatus(TCStatus &status) const
{  
  if (!readRegister(TCRegister::Status, 1)) return false;
  status.status = Wire.read();
  return true;
};

/// <summary>
/// Clear Status
/// </summary>
/// <returns>Clear status. Returns true on success, false on failure</returns>
bool ThermoCouple::clearStatus() const
{  
  return writeRegister(TCRegister::Status, 0);
};

/// <summary>
/// Get device ID (0x41 for our MCP96L01)
/// </summary>
/// <param name="id"></param>
/// <returns>True on success</returns>
bool ThermoCouple::getDeviceID(byte &id) const
{
  if (!readRegister(TCRegister::DeviceID, 1)) return false;
  id = Wire.read();
  return true;
}

bool ThermoCouple::readTemperature(float& temperature)
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
        vTaskDelay(120.0 / portTICK_PERIOD_MS); // should be just enough to finish conversion
        for (n = 0; n < 3; n++)
        {
            if (!getStatus(stat) || stat.burstComplete()) break;
            vTaskDelay(10);
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

void ThermoCouple::readTemperature(int no, TemperatureResults& temperatureResults)
{
    temperatureResults.success[no] = readTemperature(temperatureResults.temperature[no]);
}

/// <summary>
/// Get temperature
/// </summary>
/// <param name="temperature">Return measured temperature</param>
/// <returns>Returns true on success</returns>
bool ThermoCouple::getTemperatureRegister(int &temperature) const
{
    if (!readRegister(TCRegister::THigh, 2)) return false;
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
    if (!readRegister(TCRegister::TCold, 2)) return false;
    temperature = (Wire.read()<<8) | Wire.read();
    return true;
}

/// <summary>
/// Write Register
/// </summary>
bool ThermoCouple::writeRegister(TCRegister reg, byte value) const
{
  Wire.beginTransmission(_address);   // talk to TC 
  Wire.write((byte) reg);   
  Wire.write(value);
  return Wire.endTransmission(true) == 0;  // send stop
};  

/// <summary>
/// read Register
/// </summary>
bool ThermoCouple::readRegister(TCRegister reg, int count) const
{
  Wire.beginTransmission(_address);   // talk to TC
  Wire.write((byte) reg);   // status register
  Wire.endTransmission(true);             // send stop
  return (Wire.requestFrom((uint8_t)_address, (uint8_t)count) == count);
};
