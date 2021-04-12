#include "SetTemperature.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "Fonts/FreeSansBold32pt7b.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Fonts/FreeSans8pt7b.h"
#include "Fonts/FreeSansBold8pt7b.h"

// SetTemperature* pointerToSetTemperature = nullptr;
// todo: rename class to SetValues;

SetTemperature::SetTemperature(Screen* display) :
    _display(display),
    _posX(25), _posY(80),
    _x(0), _y(0), _w(0),_h(0),
    _posXDamper(5), _posYDamper(10),
    _xDamper(0), _yDamper(0), _wDamper(0), _hDamper(0),

    _currentTemperature (20.0f),
    _targetTemperature  (20)
{

}

void SetTemperature::init()
{
    // Create writing area Temperature
    _display->setFont      (&FreeMonoBold24pt7b);
    _display->setCursor    (_posX, _posY);
    _display->getTextBounds("000", _posX, _posY, &_x, &_y, &_w, &_h);
    _x--; _y--; _w += 2; _h += 2;  // 1 pixel border

    // Create writing area damper value
    _display->setFont(&FreeSansBold8pt7b);
    _display->setCursor(_posXDamper, _posYDamper);
    _display->getTextBounds("000", _posXDamper, _posYDamper, &_xDamper, &_yDamper, &_wDamper, &_hDamper);

}

void SetTemperature::update(int16_t new_target_temperature)
{        
	_targetTemperature = new_target_temperature;

     // Set damper value in header
    _display->setFont(&FreeSansBold8pt7b);
    _display->setTextColor(GxEPD_BLACK);
    _display->setCursor(_posXDamper, _posYDamper);
    _display->fillRect(_xDamper, _yDamper, _wDamper, _hDamper, GxEPD_WHITE);
    _display->setCursor(_posXDamper, _posYDamper);
    _display->println(_damperValue);


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


void SetTemperature::draw()
{
    // initial drawing same as update
    //update();
}

int16_t SetTemperature::getTargetTemperature() const
{
    return _targetTemperature;
}

void SetTemperature::setCurrentTemperature(float currentTemperature, int damperValue)
{
    _currentTemperature = currentTemperature;
    _damperValue        = damperValue;
}
