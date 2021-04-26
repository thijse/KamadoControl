#pragma once 
#include "arduino.h"
#include "TemperatureResults.h"
#include "ArduinoLog.h"

enum class TCShutdownMode : byte
{
    Active   = 0x00,
    Shutdown = 0x01,
    Burst    = 0x02,
    Mask     = 0x03
};

enum class TCMeasurementResolution : byte
{
    Bit18    = 0,
    Bit16    = 0x20,
    Bit14    = 0x40,
    Bit12    = 0x60,
    Mask     = 0x60
};

enum class TCColdJunctionResolution : byte
{
    R00625   = 0x00,
    R025     = 0x80,
    Mask     = 0x80
};

enum class TCThermoCoupleType : byte
{
    K        = 0x00,
    J        = 0x10,
    T        = 0x20,
    N        = 0x30,
    S        = 0x40,
    E        = 0x50,
    B        = 0x60,
    R        = 0x70,
    Mask     = 0x70
};

// Status
struct TCStatus {
    public:
      byte status;

      bool burstComplete() { return (status&128) != 0; };
      bool openCircuit  () { return (status&32 ) != 0; };
      bool shortCircuit () { return (status&16 ) != 0; };   
};


// Device configuration
struct TCDeviceConfig {
    public:
      byte config;

      TCColdJunctionResolution coldJunctionResolution() const                          { return (TCColdJunctionResolution) (config & (byte) TCColdJunctionResolution::Mask);};
      TCMeasurementResolution  measurementResolution()  const                          { return (TCMeasurementResolution)  (config & (byte) TCMeasurementResolution::Mask );};
      TCShutdownMode           shutdownMode()           const                          { return (TCShutdownMode)           (config & (byte) TCShutdownMode::Mask          );};
      byte                     burstModeSamples()       const                          { return                           ((config & 0x1C) >> 2) + 1                       ;};

      void                     setBurstModeSamples(byte count)                         { config = (config & (~0x1C))                                  | ((count - 1) << 2); };
      void                     setColdJunctionResolution(TCColdJunctionResolution res) { config = (config & (~(byte) TCColdJunctionResolution::Mask)) | (byte) res;         };
      void                     setMeasurementResolution(TCMeasurementResolution res)   { config = (config & (~(byte) TCMeasurementResolution::Mask))  | (byte) res;         };
      void                     setShutdownMode(TCShutdownMode mode)                    { config = (config & (~(byte) TCShutdownMode::Mask))           | (byte) mode;        };
};

// Sensor configuration
struct TCSensorConfig {
  public:
      byte config;

      byte filterCoefficients() const                 { return (config & 0x07); };
      TCThermoCoupleType thermoCoupleType() const     { return (TCThermoCoupleType) (config & (byte) TCThermoCoupleType::Mask);};
  
      void setFilterCoefficients(byte n)              { config = (config &  ~0x07) | n; };
      void setThermoCoupleType(TCThermoCoupleType tc) { config = (config & (~(byte) TCThermoCoupleType::Mask)) | (byte) tc; };
};

// Registers
enum class TCRegister : byte
{
    THigh               = 0,
    TDelta              = 1,
    TCold               = 2,
    RawADC              = 3,
    Status              = 4,
    SensorConfiguration = 5,
    DeviceConfiguration = 6,
    DeviceID            = 32    
};


// Class doing the work
class ThermoCouple {
  private:
      uint8_t            _address;
      SemaphoreHandle_t  _mutex;
      
  public:
      ThermoCouple(const ThermoCouple& other)            = delete;
      ThermoCouple& operator=(const ThermoCouple& other) = delete;

      // Constructor
      ThermoCouple(uint8_t address, SemaphoreHandle_t mutex);
      // Destructor
      ~ThermoCouple();
      // Get status
      bool getStatus(TCStatus &status) const;      
      bool clearStatus() const;
      // Get ID and version
      bool getDeviceID(byte &id) const;
      // Read temperature multiple times
      bool readTemperature(volatile float& temperature);
      // Get temperature in 1/16th degrees
      void readTemperature(int no, TemperatureResults& temperatureResults);

      bool getTemperatureRegister(int &temperature) const;
      // Get cold junction temperature
      bool getColdJunctionTemperature(int &temperature) const;
      // Get/set device configuration
      bool getDeviceConfiguration(TCDeviceConfig &config) const;
      bool setDeviceConfiguration(TCDeviceConfig &config) const;
      // Get/set sensor configuration
      bool getSensorConfiguration(TCSensorConfig &config) const;
      bool setSensorConfiguration(TCSensorConfig &config) const;

  private:
      bool readRegister (TCRegister reg, int count) const;
      bool writeRegister(TCRegister reg, byte value) const;
};