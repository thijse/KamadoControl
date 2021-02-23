// RotaryEncoder.h
// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code

#pragma once
#include "arduino.h"

// Rotary Encoder
constexpr int RotaryEncoder_DEFAULT_A_PIN   = -1;
constexpr int RotaryEncoder_DEFAULT_B_PIN   = -1;
constexpr int RotaryEncoder_DEFAULT_BUT_PIN = -1;
constexpr int RotaryEncoder_DEFAULT_VCC_PIN = -1;
constexpr int RotaryEncoder_DEFAULT_STEPS   =  2;

typedef enum {
	BUT_DOWN     = 0,
	BUT_PUSHED   = 1,
	BUT_UP       = 2,
	BUT_RELEASED = 3,
	BUT_DISABLED = 99,
} ButtonState;

class RotaryEncoder {
	
private:
	portMUX_TYPE      _mux = portMUX_INITIALIZER_UNLOCKED;
	volatile int16_t  _encoder0Pos;
	int16_t           _encoderPosition;
	bool              _circleValues;
	bool              _isEnabled;
	SemaphoreHandle_t _mutex  = nullptr;
							 
	uint8_t           _encoderAPin       = RotaryEncoder_DEFAULT_A_PIN;
	uint8_t           _encoderBPin       = RotaryEncoder_DEFAULT_B_PIN;
	uint8_t           _encoderButtonPin  = RotaryEncoder_DEFAULT_BUT_PIN;
	uint8_t           _encoderVccPin     = RotaryEncoder_DEFAULT_VCC_PIN;
	uint8_t           _encoderSteps      = RotaryEncoder_DEFAULT_STEPS;			          
	int16_t           _minEncoderValue  = -1 << 15;
	int16_t           _maxEncoderValue  =  1 << 15;			          
	uint8_t           _oldAB;
	int16_t           _lastReadEncoder0Pos;
	bool              _previousButtState;

	int8_t            _encStates[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
	void(*_ISRCallback)();

public: 
	RotaryEncoder(
		uint8_t encoderAPin      = RotaryEncoder_DEFAULT_A_PIN,
		uint8_t encoderBPin      = RotaryEncoder_DEFAULT_B_PIN,
		uint8_t encoderButtonPin = RotaryEncoder_DEFAULT_BUT_PIN,
		uint8_t encoderVccPin    = RotaryEncoder_DEFAULT_VCC_PIN,
		uint8_t encoderSteps     = RotaryEncoder_DEFAULT_STEPS
	);
	void        IRAM_ATTR readEncoder_ISR();
	void        setBoundaries     (int16_t minValue = -100, int16_t maxValue = 100, bool circleValues = false);			        
	void        setup             (void (*ISR_callback)()) const;
	void        begin             ();
	void        reset             (int16_t startValue = 0);
	void        enable            ();
	void        disable           ();
	int16_t     readEncoder       () const;
	int16_t     encoderChanged    ();
	ButtonState currentButtonState();

};


