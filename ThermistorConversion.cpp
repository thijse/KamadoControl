#include "ThermistorConversion.h"
#include <math.h>	
#include "ArduinoLog.h"

/*
	https://www.jameco.com/Jameco/workshop/TechTip/temperature-measurement-ntc-thermistors.html


	The Steinhart - Hart equation is the most widely used tool to interpolate
	  the NTC thermistor resistance / temperature curve.It is a third order
	  polynomial which provides very good curve fitting.

	  1 / t = A + B * log(r) + C * log(r) ^ 3

	  A, B and C are manufacturing coefficients for a certain temperature range.
	  These can usually be found in the data sheet.
	  If you don't specify them this library will use the coefficients for the
	  most common type of NTC thermistor.
	  Thermistor values denote their resistance at 25°C.A popular type would
	  be an NTC 10K which would give roughly 10 kOhms at that temperature point.

	  Although there are ways to calculate the coefficients yourself experimentally
	  it might be cheaper and easier to just buy a thermistor with known specs.

	  To get readings from a thermistor into your Arduino you will have to use
	  a conventional voltage divider circuit.

								   ____         ____
	  +Vin       o----------------|_R0_|---+---|_Rt__|---o GND
								   (10K)   |    NTC(10K)
					   ___                 |
	  adcValue   o----|ADC|  ---  Vt ----- +
						|
	  +Vref      o------+
*/



ThermistorConversion::ThermistorConversion(float vIn, float vRef, uint16_t adcMax) :
	_vIn   (vIn          ),
    _vRef  (vRef         ),
    _adcMax(adcMax       ),
    _r0    (10000.0f     ),
	_a     (0.0022841105304199),
	_b     (0            ),
	_c     (0.0000151906113893158),
    _k     (9.5f         )
{
}

void ThermistorConversion::setNTC(
    float r0 = 10000.0f    ,
    float a  = 0.0022841105304199,
    float b  = 0           ,
    float c  = 0.0000151906113893158,
	float k  = 9.5f
	)
{
	_r0 = r0;
	_a  = a;
	_b  = b;
	_c  = c;
	_k  = k;
};

 /**
  * Returns the temperature in kelvin for the given resistance value
  * using the Steinhart-Hart polynom.
  */
float ThermistorConversion::steinhartHart(float rt)
{
	float ln_r  = log(rt);
	float ln_r3 = ln_r * ln_r * ln_r;
	return 1.0f / (_a + _b * ln_r + _c * ln_r3);
}


float ThermistorConversion::getTempCelsius(uint16_t adcValue)
{
	// Calculate measured voltage
	float vt    = ((float)adcValue / (float)_adcMax) * _vRef;
	// Calculate resistance of NTC
	float rt    = _r0 / ((_vIn / vt) - 1.0f);
	// Account for dissipation factor K
	//rt         -=  vt * vt / (_k * _r0);

	Log.noticeln(F("adcValue = %i, adcMax = %i, _vRef = %F, v0 = %F, rt = %F"), adcValue, _adcMax, _vRef, vt,rt);

	return steinhartHart(rt);
}


float ThermistorConversion::getTempKelvin(uint16_t adcValue)
{
	return getTempCelsius(adcValue) + 273.15f;
}


float ThermistorConversion::getTempFahrenheit(uint16_t adcValue)
{
	return getTempKelvin(adcValue) * 1.8f + 459.67f;
}