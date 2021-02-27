#pragma once
#include "arduino.h"
#include <Wire.h>

enum class ADCCommand : byte
{
    Reset                      = 0x06,
    Start                      = 0x08,
    PowerDown                  = 0x02,
    ReadData                   = 0x10,
    ReadConfigurationRegister  = 0x20,
    ReadStatusRegister         = 0x24,
    WriteConfigurationRegister = 0x40
};

enum class ADCMux : byte
{
    C0C1        = 0x00,
    C2C3        = 0x20,
    C1C2        = 0x40,
    C0Gnd       = 0x60,
    C1Gnd       = 0x80,
    C2Gnd       = 0xA0,
    C3Gnd       = 0xC0,
    Calibration = 0xE0,
    Mask        = 0xE0
};

enum class ADCGain : byte
{
  G1   = 0x00,
  G4   = 0x10,
  Mask = 0x10
};

enum class ADCConversionRate : byte
{
  SPS20   = 0x00,   // 16 bit resolution, 50 ms conversion time, 
  SPS90   = 0x04,   // 16 bit, 12 ms conversion time
  SPS330  = 0x08,   // 15 bit, 3 ms conversion time
  SPS1000 = 0x0C,   // 14 bit, 1 ms conversion time
  Mask    = 0x0C
};

enum class ADCConversionMode : byte
{
    SingleShot = 0x00,
    Continuous = 0x02,
    Mask       = 0x02
};

enum class ADCVoltageReference : byte
{
    Internal = 0x00,
    External = 0x01,
    Mask     = 0x01
};

// ADC configuration
struct ADCConfig {
  public:
  byte config;

  ADCVoltageReference VoltageReference   ()                      { return (ADCVoltageReference) (config & (byte) ADCVoltageReference::Mask); };  
  ADCConversionMode   ConversionMode     ()                      { return (ADCConversionMode)   (config & (byte) ADCConversionMode::Mask);   };
  ADCConversionRate   ConversionRate     ()                      { return (ADCConversionRate)   (config & (byte) ADCConversionRate::Mask);   };
  ADCGain             Gain               ()                      { return (ADCGain)             (config & (byte) ADCGain::Mask);             };
  ADCMux              Mux                ()                      { return (ADCMux)              (config & (byte) ADCMux::Mask);              };
  void                SetVoltageReference(ADCVoltageReference r) { config = (config & ~(byte) ADCVoltageReference::Mask) | (byte) r;         };
  void                SetConversionMode  (ADCConversionMode m)   { config = (config & ~(byte) ADCConversionMode::Mask)   | (byte) m;         };
  void                SetConversionRate  (ADCConversionRate m)   { config = (config & ~(byte) ADCConversionRate::Mask)   | (byte) m;         };
  void                SetGain            (ADCGain m)             { config = (config & ~(byte) ADCGain::Mask)             | (byte) m;         };
  void                SetMux             (ADCMux m)              { config = (config & ~(byte) ADCMux::Mask)              | (byte) m;         };
  bool                SetMux             (byte n)
  {
    switch (n)
    {
      case 0: SetMux(ADCMux::C0Gnd); break;
      case 1: SetMux(ADCMux::C1Gnd); break;
      case 2: SetMux(ADCMux::C2Gnd); break;
      case 3: SetMux(ADCMux::C3Gnd); break;
      default: return false;
    };    
    return true;
  }
};


// Class doing the work
class ADC {
  private:
      byte _address;
      
  public:

      ADC(const ADC& other)            = delete;
      ADC& operator=(const ADC& other) = delete;

      // Constructor
      ADC(byte _address);
      // Destructor
      ~ADC();

      // Set configuration
      bool configure(ADCConfig ac);
      // read current configuration
      bool readConfiguration(ADCConfig &result);
      // Reset device
      bool reset();
      // Start a single or continuous conversion (depending on configuration register)
      bool start();
      // Power down. Start() can start a conversion from power down
      bool powerDown();
     // Test whether a new ADC conversion is ready. Flag is reset after reading value.
      bool conversionReady(bool &result);
      // read conversion value
      bool read(int16_t &value);
};

