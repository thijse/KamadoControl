// AiEsp32RotaryEncoder.h
// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code

#ifndef _AIESP32ROTARYENCODER_h
#define _AIESP32ROTARYENCODER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Rotary Encocer
#define AIESP32ROTARYENCODER_DEFAULT_A_PIN 25
#define AIESP32ROTARYENCODER_DEFAULT_B_PIN 26
#define AIESP32ROTARYENCODER_DEFAULT_BUT_PIN 15

typedef enum
{
	BUT_DOWN = 0,
	BUT_UP = 2,
	BUT_DISABLED = 99,
} ButtonState;

class AiEsp32RotaryEncoder
{

private:
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
	volatile long encoderPos = 0;

	volatile int8_t lastMovementDirection = 0; //1 right; -1 left
	volatile unsigned long lastMovementAt = 0;
	unsigned long rotaryAccelerationCoef = 150;

	bool _circleValues = false;
	bool isEnabled = true;

	uint8_t encoderAPin = AIESP32ROTARYENCODER_DEFAULT_A_PIN;
	uint8_t encoderBPin = AIESP32ROTARYENCODER_DEFAULT_B_PIN;
	uint8_t encoderButtonPin = AIESP32ROTARYENCODER_DEFAULT_BUT_PIN;

	long _minEncoderValue = -1 << 15;
	long _maxEncoderValue = 1 << 15;

	bool click_occured;

	void (*ISR_callback)();
	void (*ISR_button)();

public:
	AiEsp32RotaryEncoder(
		uint8_t encoderAPin = AIESP32ROTARYENCODER_DEFAULT_A_PIN,
		uint8_t encoderBPin = AIESP32ROTARYENCODER_DEFAULT_B_PIN,
		uint8_t encoderButtonPin = AIESP32ROTARYENCODER_DEFAULT_BUT_PIN);
	void setBoundaries(long minValue = -100, long maxValue = 100, bool circleValues = false);
	void IRAM_ATTR readEncoder_ISR();
	void IRAM_ATTR readButton_ISR();

	void setup(void (*ISR_callback)(void));
	void setup(void (*ISR_callback)(void), void (*ISR_button)(void));
	void begin();
	void reset(long newValue = 0);
	void enable();
	void disable();
	long readEncoderValue();
	void setEncoderValue(long newValue);
	ButtonState currentButtonState();
	bool ClickOccured(bool peek = false);
	unsigned long getAcceleration() { return this->rotaryAccelerationCoef; }
	void setAcceleration(unsigned long acceleration) { this->rotaryAccelerationCoef = acceleration; }
	void disableAcceleration() { setAcceleration(0); }
};
#endif
