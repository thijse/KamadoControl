// BatteryUI
#pragma once 
#include "arduino.h"
#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include "Screen.h"

class BatteryUI
{
	protected:
		const float   _adcScale = 1.0 / 4095.0 * 2.0 * 3.3 * 1.1;
				
		Screen* _display;
		int           _batteryPin;
		float         _vMin;
		float         _vMax;
		float         _vSlope;
		
		int           _posX;
		int           _posY;
		int           _width;
		int           _height;
		int           _vToBarSlope;
		int           _prevBarLength;

	public:
		BatteryUI           (const BatteryUI& other) = delete;
		BatteryUI& operator=(const BatteryUI& other) = delete;
		BatteryUI(
			Screen* display,
			const int   batteryPin
		);

		void init();
		float getVoltage();
		int getPercentage();
		void update();
		void draw();
};