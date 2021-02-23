#pragma once 
#include "arduino.h"

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

      bool BurstComplete() { return (status&128) != 0; };
      bool OpenCircuit()   { return (status&32 ) != 0; };
      bool ShortCircuit()  { return (status&16 ) != 0; };   
};


// Device configuration
struct TCDeviceConfig {
    public:
      byte config;

      TCColdJunctionResolution ColdJunctionResolution() { return (TCColdJunctionResolution) (config & (byte) TCColdJunctionResolution::Mask);};
      TCMeasurementResolution  MeasurementResolution()  { return (TCMeasurementResolution)  (config & (byte) TCMeasurementResolution::Mask );};
      TCShutdownMode           ShutdownMode()           { return (TCShutdownMode)           (config & (byte) TCShutdownMode::Mask          );};
      byte                     BurstModeSamples()       { return                           ((config & 0x1C) >> 2) + 1                       ;};

      void                   SetBurstModeSamples(byte count)                         { config = (config & (~0x1C))                                  | ((count - 1) << 2); };
      void                   SetColdJunctionResolution(TCColdJunctionResolution res) { config = (config & (~(byte) TCColdJunctionResolution::Mask)) | (byte) res;         };
      void                   SetMeasurementResolution(TCMeasurementResolution res)   { config = (config & (~(byte) TCMeasurementResolution::Mask))  | (byte) res;         };
      void                   SetShutdownMode(TCShutdownMode mode)                    { config = (config & (~(byte) TCShutdownMode::Mask))           | (byte) mode;        };
};


// Sensor configuration
struct TCSensorConfig {
  public:
      byte config;

      byte FilterCoefficients()                       { return (config & 0x07); };
      TCThermoCoupleType ThermoCoupleType()           { return (TCThermoCoupleType) (config & (byte) TCThermoCoupleType::Mask);};
  
      void SetFilterCoefficients(byte n)              { config = (config &  ~0x07) | n; };
      void SetThermoCoupleType(TCThermoCoupleType tc) { config = (config & (~(byte) TCThermoCoupleType::Mask)) | (byte) tc; };
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
      uint8_t address;
      
  public:
      ThermoCouple(const ThermoCouple& other)            = delete;
      ThermoCouple& operator=(const ThermoCouple& other) = delete;

      // Constructor
      ThermoCouple(uint8_t _address);
      // Destructor
      ~ThermoCouple();
      // Get status
      bool GetStatus(TCStatus &status);      
      bool ClearStatus();
      // Get ID and version
      bool GetDeviceID(byte &id);
      // Get temperature in 1/16th degrees
      bool GetTemperature(int &temperature);
      // Get cold junction temperature
      bool GetColdJunctionTemperature(int &temperature) const;
      // Get/set device configuration
      bool GetDeviceConfiguration(TCDeviceConfig &config);
      bool SetDeviceConfiguration(TCDeviceConfig &config);
      // Get/set sensor configuration
      bool GetSensorConfiguration(TCSensorConfig &config);
      bool SetSensorConfiguration(TCSensorConfig &config);

  private:
      bool ReadRegister (TCRegister reg, int count) const;
      bool WriteRegister(TCRegister reg, byte value) const;
};

