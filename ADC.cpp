#include <Wire.h>
#include "ADC.h"

// Constructor: _address is I2C address
ADC::ADC(byte address, SemaphoreHandle_t* mutex) :
    _address(address),
    _mutex  (mutex)
{
};

// Destructor
ADC::~ADC(){  };

// Set configuration
bool ADC::configure(ADCConfig ac)
{
    bool result;
    if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
    {
        Wire.beginTransmission(_address);
        Wire.write((byte)ADCCommand::WriteConfigurationRegister);
        Wire.write((byte)ac.config);
        result = Wire.endTransmission(true) == 0;
        xSemaphoreGive(*_mutex);
    }
    return result;
}

// Test whether a new ADC conversion is ready. Flag is reset after reading value!
bool ADC::readConfiguration(ADCConfig &result)
{
    if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
    {
        Wire.beginTransmission(_address);
        Wire.write((byte)ADCCommand::ReadConfigurationRegister);
        if (Wire.endTransmission(false) != 0 || Wire.requestFrom(_address, (byte)1) != 1)
        {
            xSemaphoreGive(*_mutex);
            return false;
        }
        result.config = Wire.read();
        xSemaphoreGive(*_mutex);
    }
    return true;
}

// Reset device
bool ADC::reset()
{
    return writeByte((byte)ADCCommand::Reset);
}

// Start a single or continuous conversion (depending on configuration register). Also 'reverses' shutdown.
bool ADC::start()
{
    return writeByte((byte)ADCCommand::Start);
}

// Power down. Start() can start a conversion from power down
bool ADC::powerDown()
{
    return writeByte((byte)ADCCommand::PowerDown);
}

// Test whether a new ADC conversion is ready. Flag is reset after reading value!
bool ADC::conversionReady(bool &result)
{

    if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
    {
        Wire.beginTransmission(_address);
        Wire.write((byte)ADCCommand::ReadStatusRegister);
        if (Wire.endTransmission(false) != 0 || Wire.requestFrom(_address, (byte)1) != 1)
        {
            xSemaphoreGive(*_mutex);
            return false;
        }
        result = (Wire.read() & 0x80) != 0;
        xSemaphoreGive(*_mutex);
    }
    return true;
}

// read conversion value
bool ADC::read(int16_t &value)
{
    if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
    {
        Wire.beginTransmission(_address);
        Wire.write((byte)ADCCommand::ReadData);
        if (Wire.endTransmission(false) != 0 || Wire.requestFrom(_address, (byte)2) != 2)
        {
            xSemaphoreGive(*_mutex);
            return false;
        }
        value = (Wire.read() << 8) + Wire.read();
        xSemaphoreGive(*_mutex);
    } 
    return true;
}

bool ADC::writeByte(byte value)
{
    bool result;
    if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
    {
        Wire.beginTransmission(_address);
        Wire.write((byte)value);
        result = Wire.endTransmission(true) == 0;
        xSemaphoreGive(*_mutex);
    }
    return result;
}