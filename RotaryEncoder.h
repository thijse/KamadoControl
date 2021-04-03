// ClickEncoder extension using the esp32 interupt timer

#pragma once
#include "arduino.h"
#include "Constants.h"
#include <ClickEncoder.h>

class RotaryEncoder: public ClickEncoder
{
 protected:
	hw_timer_t* timer = nullptr;

 public:
	 RotaryEncoder(const RotaryEncoder& other) = delete;
	 RotaryEncoder& operator=(const RotaryEncoder& other) = delete;

	RotaryEncoder(int8_t A, int8_t B, int8_t BTN = -1, uint8_t stepsPerNotch = 4, bool active = LOW) : ClickEncoder(A, B, BTN, stepsPerNotch, active) {};
	void init();
	void update();

	

};

extern RotaryEncoder rotaryEncoder;
