// ClickEncoder extension using the esp32 interupt timer

#pragma once
#include "arduino.h"
#include "Global.h"
#include "RotaryEncoder.h"


class RangedRotaryEncoder
{
 private:
	 
	 int16_t           _minEncoderValue = -1 << 15;
	 int16_t           _maxEncoderValue  = 1 << 15;
	 int16_t           _encoderPosition  = 0;
	 int16_t           _encoderDelta     = 0;
	 bool              _circleValues     = false;
	 bool              _isEnabled        = true;      

 public:
	 RangedRotaryEncoder(const RangedRotaryEncoder& other) = delete;
	 RangedRotaryEncoder& operator=(const RangedRotaryEncoder& other) = delete;

	 RotaryEncoder	       *Encoder;
	 
	RangedRotaryEncoder    (RotaryEncoder * rotaryEncoder);
	void    setBoundaries  (int16_t minValue, int16_t maxValue, bool circleValues);
	void    reset          (int16_t startValue);
	void    enable         ();
	void    disable        ();
	int16_t getValue       ();
};

