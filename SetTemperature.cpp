#include "SetTemperature.h"
#include "RotaryEncoder.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "Fonts/FreeSansBold32pt7b.h"

// SetTemperature* pointerToSetTemperature = nullptr;

SetTemperature::SetTemperature(Screen* display, RotaryEncoder* rotaryEncoder) :
    _display(display),
    _rotaryEncoder(rotaryEncoder),
    _rangedRotaryEncoder(_rotaryEncoder),
    _posX(25), _posY(80),
    _x(0), _y(0), _w(0),_h(0),
    
    _currentTemperature (20.0f),
    _targetTemperature  (20)
{
    //_rotaryEncoder->getButton();
    _rangedRotaryEncoder.reset        (_targetTemperature);
    _rangedRotaryEncoder.setBoundaries(0, 400, false);
    _rangedRotaryEncoder.enable       ();
    ClickEncoder::Button b = _rangedRotaryEncoder.Encoder->getButton();
}

void SetTemperature::init()
{
    // Create writing area
    _display->setFont      (&FreeMonoBold24pt7b);
    _display->setCursor    (_posX, _posY);
    _display->getTextBounds("000", _posX, _posY, &_x, &_y, &_w, &_h);
    _x--; _y--; _w += 2; _h += 2;  // 1 pixel border
}

void SetTemperature::update(MenuState &menuState)
{
    if (menuState != MenuState::menuIdle) return;
    rotaryInput(menuState);

    _display->setFont      (&FreeMonoBold24pt7b);
    _display->setTextColor (GxEPD_BLACK);
    _display->setCursor    (_posX, _posY);
    _display->fillRect     (_x, _y, _w, _h, GxEPD_WHITE);
    _display->setCursor    (_posX, _posY);
    _display->print        (_targetTemperature);
    _display->setCursor    (_posX+100, _posY); // todo: make font dependent
    _display->fillRect     (_x+100, _y, _w+40, _h, GxEPD_WHITE);
    _display->setCursor    (_posX+100, _posY);
    _display->print        (_currentTemperature,1);
    _display->updateRequest(Screen::partial);
}

void SetTemperature::rotaryInput(MenuState& menuState) {
    ClickEncoder::Button b = _rangedRotaryEncoder.Encoder->getButton();
    //first lets handle rotary encoder button click
    if (b != ClickEncoder::Open) {
        switch (b) {
        case ClickEncoder::Clicked:
        case ClickEncoder::DoubleClicked:
            Log.noticeln(F("Button pressed!"));
            menuState = MenuState::menuWaking;
            break;
        }
    }

    _targetTemperature = _rangedRotaryEncoder.getValue();
    
}

void SetTemperature::draw()
{
    // initial drawing same as update
    //update();
}

int16_t SetTemperature::getTargetTemperature() const
{
    return _targetTemperature;
}

void SetTemperature::setCurrentTemperature(float currentTemperature)
{
    _currentTemperature = currentTemperature;
}
