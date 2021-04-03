#include "RangedRotaryEncoder.h"

RangedRotaryEncoder::RangedRotaryEncoder(RotaryEncoder* rotaryEncoder) :
	Encoder(rotaryEncoder)
{
}

void RangedRotaryEncoder::setBoundaries(int16_t minValue, int16_t maxValue, bool circleValues)
{
	_minEncoderValue = minValue;
	_maxEncoderValue = maxValue;
	_circleValues    = circleValues;
}

void RangedRotaryEncoder::reset(int16_t startValue) {
	_encoderPosition = startValue;

	if (startValue > _maxEncoderValue) startValue = _circleValues ? _minEncoderValue : _maxEncoderValue;
	if (startValue < _minEncoderValue) startValue = _circleValues ? _maxEncoderValue : _minEncoderValue;
}

void RangedRotaryEncoder::enable() {
	_isEnabled = true;
}

void RangedRotaryEncoder::disable() {
	_isEnabled = false;
}

int16_t RangedRotaryEncoder::getValue() 
{
	_encoderDelta = 0;
	if (_isEnabled) {
		_encoderDelta     = Encoder->getValue();
		_encoderPosition += _encoderDelta* abs(_encoderDelta);
		if (_encoderPosition > (_maxEncoderValue)) _encoderPosition = _circleValues ? _minEncoderValue : _maxEncoderValue;
		if (_encoderPosition < (_minEncoderValue)) _encoderPosition = _circleValues ? _maxEncoderValue : _minEncoderValue;
	}

	return _encoderPosition;
}

