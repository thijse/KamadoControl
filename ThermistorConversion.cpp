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
	  +Vin       o----------------|_Rt_|---+---|_R0__|---o GND
								   NTC(10K)|    (10K)
					   ___                 |
	  adcValue   o----|ADC|  ---  V0 ----- +
						|
	  +Vref      o------+
*/



ThermistorConversion::ThermistorConversion(float vIn, float vRef, uint16_t adcMax) :
	_vIn   (vIn      ),
    _vRef  (vRef     ),
    _adcMax(adcMax   ),
    _r0    (10000.0f ),
	_a  (1.129148e-3f),
	_b  (2.34125e-4f ),
	_c  (8.76741e-8f ),
    _k  (9.5f        )
{
}

void ThermistorConversion::setNTC(
    float r0 = 10000.0f    ,
    float a  = 1.129148e-3f,
    float b  = 2.34125e-4f ,
    float c  = 8.76741e-8f ,
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
	float log_r  = log(rt);
	float log_r3 = log_r * log_r * log_r;
	return 1.0f / (_a + _b * log_r + _c * log_r3);
}


float ThermistorConversion::getTempKelvin(uint16_t adcValue)
{
	// Calculate measured voltage
	float v0    = ((float)adcValue / (float)_adcMax) * _vRef;
	// Calculate resistance of NTC
	float rt    = _r0 * ((_vIn / v0) - 1.0f);
	// Account for dissipation factor K
	rt         -=  v0 * v0 / (_k * _r0);

	Log.noticeln(F("adcValue = %i, adcMax = %i, _vRef = %F, v0 = %F, rt = %F"), adcValue, _adcMax, _vRef, v0,rt);

	return steinhartHart(rt);
}


float ThermistorConversion::getTempCelsius(uint16_t adcValue)
{
	return getTempKelvin(adcValue) - 273.15f;
}


float ThermistorConversion::getTempFahrenheit(uint16_t adcValue)
{
	return getTempKelvin(adcValue) * 1.8f + 459.67f;
}