#include "SetTemperature.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "Fonts/FreeSansBold32pt7b.h"
//
SetTemperature* pointerToSetTemperature = nullptr;

SetTemperature::SetTemperature(Screen* display) :
    _display(display),
    _posX(25), _posY(80),
    _x(0), _y(0), _w(0),_h(0),
    _temp(20)
{
    Serial.print("SetTemperature:ctor");
    _rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);
    if (pointerToSetTemperature!=nullptr)
    {
        Log.errorln(F("Already a SetTemperature class instantiated. There can be only one "));
        return; // throw;
    }
     pointerToSetTemperature = this;
    _rotaryEncoder.reset        (_temp);
    _rotaryEncoder.setup        ([] {pointerToSetTemperature->_rotaryEncoder.readEncoder_ISR(); });
    _rotaryEncoder.setBoundaries(0, 400, false);
    _rotaryEncoder.begin        ();
}

void SetTemperature::init()
{
    // Create writing area
    _display->setFont      (&FreeSansBold32pt7b);
    _display->setCursor    (_posX, _posY);
    _display->getTextBounds("000", _posX, _posY, &_x, &_y, &_w, &_h);
    _x--; _y--; _w += 2; _h += 2;  // 1 pixel border
}

void SetTemperature::update()
{
    rotaryInput();

    _display->setFont      (&FreeSansBold32pt7b);
    _display->setTextColor (GxEPD_BLACK);
    _display->setCursor    (_posX, _posY);
    _display->fillRect     (_x, _y, _w, _h, GxEPD_WHITE);
    _display->setCursor    (_posX, _posY);
    _display->print        (_temp);
    _display->updateRequest(Screen::partial);
}

void SetTemperature::rotaryInput() {
	//first lets handle rotary encoder button click
	//if (rotaryEncoder.currentButtonState() == BUT_RELEASED) {
	//	//we can process it here or call separate function like:
	//	rotary_onButtonClick();
	//}

	//lets see if anything changed
    const int16_t encoderDelta = _rotaryEncoder.encoderChanged();
	//optionally we can ignore whenever there is no change
	if (encoderDelta == 0) return;

    _temp = _rotaryEncoder.readEncoder();
}

void SetTemperature::draw()
{
    // initial drawing same as update
    update();
}

int16_t SetTemperature::getTargetTemperature() const
{
    return _temp;
}
