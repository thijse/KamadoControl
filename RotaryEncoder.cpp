// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
// 
// 

#include "RotaryEncoder.h"

void IRAM_ATTR RotaryEncoder::readEncoder_ISR()
{		
	portENTER_CRITICAL_ISR(&(_mux));

	if (_isEnabled) {

	    _oldAB <<= 2;        //remember previous state
        const int8_t ENC_PORT = ((digitalRead(_encoderBPin)) ? (1 << 1) : 0) | ((digitalRead(_encoderAPin)) ? (1 << 0) : 0);				
		_oldAB               |= ( ENC_PORT & 0x03 );  //add current state
		_encoder0Pos         += ( _encStates[( _oldAB & 0x0f )]);

		if (_encoder0Pos > (_maxEncoderValue)) _encoder0Pos = _circleValues ? _minEncoderValue : _maxEncoderValue;
		if (_encoder0Pos < (_minEncoderValue)) _encoder0Pos = _circleValues ? _maxEncoderValue : _minEncoderValue;
		if (xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY))
		{
			_encoderPosition = _encoder0Pos;
		    xSemaphoreGive(_mutex);
		}
	}
	portEXIT_CRITICAL_ISR(&(_mux));
}

RotaryEncoder::RotaryEncoder(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t encoderButtonPin, uint8_t encoderVccPin, uint8_t encoderSteps):
    _encoder0Pos        (0)      ,
    _encoderPosition    (0)      ,
	_circleValues       (false)  ,
	_isEnabled          (true)   ,
	_oldAB              (0)      ,
    _lastReadEncoder0Pos(0)      ,
    _previousButtState  (false)  ,
    _ISRCallback        (nullptr)
{
    _mutex = xSemaphoreCreateMutex();

    _encoderAPin      = encoderAPin;
    _encoderBPin      = encoderBPin;
    _encoderButtonPin = encoderButtonPin;
    _encoderVccPin    = encoderVccPin;
    _encoderSteps     = encoderSteps;

    pinMode(_encoderAPin, INPUT);
    pinMode(_encoderBPin, INPUT);
}

void RotaryEncoder::setBoundaries(int16_t minValue, int16_t maxValue, bool circleValues)
{
	_minEncoderValue = minValue * _encoderSteps;
	_maxEncoderValue = maxValue * _encoderSteps;
	_circleValues    = circleValues;
}

int16_t RotaryEncoder::readEncoder() const
{
	int16_t value = 0;
	if (xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY))
	{
        value =(_encoderPosition / _encoderSteps);
	    xSemaphoreGive(_mutex);
	}
	return value;
}

int16_t RotaryEncoder::encoderChanged() {
    const int16_t currentEncoderPos = readEncoder();
    const int16_t encoder0Diff      = currentEncoderPos - _lastReadEncoder0Pos;
	_lastReadEncoder0Pos            = currentEncoderPos;
	
	return encoder0Diff;
}

void RotaryEncoder::setup(void (*ISR_callback)()) const
{
	//ESP_LOGD(LOG_TAG, "Enabling rotary encoder ISR.");
	
	attachInterrupt(digitalPinToInterrupt(_encoderAPin), ISR_callback, CHANGE);
	attachInterrupt(digitalPinToInterrupt(_encoderBPin), ISR_callback, CHANGE);
}

void RotaryEncoder::begin()
{
	_lastReadEncoder0Pos = 0;	
	if (_encoderVccPin >= 0) {
		pinMode(_encoderVccPin, OUTPUT);	
		digitalWrite(_encoderVccPin, 1);//Vcc for encoder 
	}
	
	// Initialize rotary encoder reading and decoding
	_previousButtState = false;
	if (_encoderButtonPin >= 0) {
		pinMode(_encoderButtonPin, INPUT_PULLUP);
	}
}

ButtonState RotaryEncoder::currentButtonState()
{
	if (!_isEnabled)
		return BUT_DISABLED;

    const uint8_t butt_state = !digitalRead(_encoderButtonPin);

	if (butt_state && !_previousButtState)
	{
		_previousButtState = true;
		//ESP_LOGD(LOG_TAG, "Button Pushed");
		return BUT_PUSHED;
	}
	if (!butt_state && _previousButtState)
	{
		_previousButtState = false;
		//ESP_LOGD(LOG_TAG, "Button Released");
		return BUT_RELEASED; 
	}
	return (butt_state ? BUT_DOWN : BUT_UP);
}

void RotaryEncoder::reset(int16_t startValue) {
	startValue = startValue * _encoderSteps;

	if (startValue > _maxEncoderValue) startValue = _circleValues ? _minEncoderValue : _maxEncoderValue;
	if (startValue < _minEncoderValue) startValue = _circleValues ? _maxEncoderValue : _minEncoderValue;

	if (xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY))
	{
		_encoderPosition = startValue;
		_encoder0Pos     = startValue;
		xSemaphoreGive(_mutex);
	}	
}

void RotaryEncoder::enable() {
	_isEnabled = true;
}

void RotaryEncoder::disable() {
	_isEnabled = false;
}
