#pragma once
#include "Arduino.h"
#include <inttypes.h>

class ThermistorConversion {
public:
	ThermistorConversion           (const ThermistorConversion& other) = delete;
	ThermistorConversion& operator=(const ThermistorConversion& other) = delete;

	ThermistorConversion(float vIn = 3.3f, float vRef = 3.3f, uint16_t adcMax = 65535);
	/// <summary>
	/// If no other parameters are given default values will be used.
	/// These values are for a NTC 10k thermistor with a 10k resistor
	///	put in parallel.
	/// </summary>
	/// <param name="r0"></param>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="c"></param>
	/// <param name="k">Dissipation factor(mW / °C)</param>
	void setNTC(
        float r0,
        float a,
        float b,
        float c,
		float k
    );


	float getTempKelvin    (uint16_t adcValue);
	float getTempCelsius   (uint16_t adcValue);
	float getTempFahrenheit(uint16_t adcValue);

private:
	float steinhartHart(float);

	// values determined by resistance measurement & digitization 
	float    _vIn;    // Voltage over voltage divider  
	float    _vRef;   // voltage as reference for ADC 
	uint16_t _adcMax; // Maximum adc value  (when input voltage is equal to reference voltage)

	// Value of the resistor put in parallel
	float  _r0;

	// Manufacturing constants
	float _a;
	float _b;
	float _c;
	float _k; // Dissipation factor(mW / °C)
};


