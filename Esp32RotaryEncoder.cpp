// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
//
//

#include "Esp32RotaryEncoder.h"

void IRAM_ATTR Esp32RotaryEncoder::readEncoder_ISR()
{

	unsigned long now = millis();
	portENTER_CRITICAL_ISR(&(this->mux));

	if (this->isEnabled)
	{
		int8_t currentDirection = (digitalRead(this->encoderAPin)) ? 1 : -1;

		this->encoderPos += currentDirection;

		if (rotaryAccelerationCoef > 1)
		{
			//additional movements cause acceleration?
			// at X ms, there should be no acceleration.
			unsigned long accelerationLongCutoffMillis = 200;
			// at Y ms, we want to have maximum acceleration
			unsigned long accelerationShortCutffMillis = 4;

			// compute linear acceleration
			if (currentDirection == lastMovementDirection &&
				currentDirection != 0 &&
				lastMovementDirection != 0)
			{
				// ... but only of the direction of rotation matched and there
				// actually was a previous rotation.
				unsigned long millisAfterLastMotion = now - lastMovementAt;

				if (millisAfterLastMotion < accelerationLongCutoffMillis)
				{
					if (millisAfterLastMotion < accelerationShortCutffMillis)
					{
						millisAfterLastMotion = accelerationShortCutffMillis; // limit to maximum acceleration
					}
					if (currentDirection > 0)
					{
						this->encoderPos += rotaryAccelerationCoef / millisAfterLastMotion;
					}
					else
					{
						this->encoderPos -= rotaryAccelerationCoef / millisAfterLastMotion;
					}
				}
			}
			this->lastMovementAt = now;
			this->lastMovementDirection = currentDirection;			

			//respect limits
			if (this->encoderPos > (this->_maxEncoderValue))
				this->encoderPos = this->_circleValues ? this->_minEncoderValue : this->_maxEncoderValue;
			if (this->encoderPos < (this->_minEncoderValue))
				this->encoderPos = this->_circleValues ? this->_maxEncoderValue : this->_minEncoderValue;
		}
	}
	portEXIT_CRITICAL_ISR(&(this->mux));
}


void IRAM_ATTR Esp32RotaryEncoder::readButton_ISR()
{
	if (!digitalRead(this->encoderButtonPin))
	{
		click_occured = true;		// down, so true!
	}
}


Esp32RotaryEncoder::Esp32RotaryEncoder(uint8_t encoder_APin, uint8_t encoder_BPin, uint8_t encoder_ButtonPin)
{
	this->encoderAPin = encoder_APin;
	this->encoderBPin = encoder_BPin;
	this->encoderButtonPin = encoder_ButtonPin;

	pinMode(this->encoderAPin, INPUT);
	pinMode(this->encoderBPin, INPUT);
}

void Esp32RotaryEncoder::setBoundaries(long minEncoderValue, long maxEncoderValue, bool circleValues)
{
	this->_minEncoderValue = minEncoderValue;
	this->_maxEncoderValue = maxEncoderValue;

	this->_circleValues = circleValues;
}

bool Esp32RotaryEncoder::ClickOccured(bool peek)
{
	bool co = this->click_occured;
	if (co && !peek)
	{	// yes, some multithreading / ISR risk here. By only clearing when true we don't (really) miss a click
		this->click_occured = false;
	}
	return co;
}

ButtonState Esp32RotaryEncoder::currentButtonState()
{
	if (!this->isEnabled)
	{
		return BUT_DISABLED;
	}
	return !digitalRead(this->encoderButtonPin) ? BUT_DOWN : BUT_UP;
}

long Esp32RotaryEncoder::readEncoderValue()
{
	return (this->encoderPos);
}

void Esp32RotaryEncoder::setEncoderValue(long newValue)
{
	reset(newValue);
}


void Esp32RotaryEncoder::setup(void (*ISR_callback)(void), void (*ISR_button)(void))
{
	attachInterrupt(digitalPinToInterrupt(this->encoderBPin), ISR_callback, FALLING);
	attachInterrupt(digitalPinToInterrupt(this->encoderButtonPin), ISR_button, CHANGE);
}

void Esp32RotaryEncoder::begin()
{
	// Initialize rotary encoder reading and decoding
	if (this->encoderButtonPin >= 0)
	{
		pinMode(this->encoderButtonPin, INPUT);
	}
}


void Esp32RotaryEncoder::reset(long newValue_)
{
	newValue_ = newValue_;
	this->encoderPos = newValue_;
	if (this->encoderPos > this->_maxEncoderValue)
		this->encoderPos = this->_circleValues ? this->_minEncoderValue : this->_maxEncoderValue;
	if (this->encoderPos < this->_minEncoderValue)
		this->encoderPos = this->_circleValues ? this->_maxEncoderValue : this->_minEncoderValue;
}

void Esp32RotaryEncoder::enable()
{
	this->isEnabled = true;
}

void Esp32RotaryEncoder::disable()
{
	this->isEnabled = false;
}
