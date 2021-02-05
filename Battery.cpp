#include "Battery.h"

Battery::Battery(
    Screen* display,
    const int   batteryPin 
) :
    _display      (display)                              ,
    _batteryPin   (batteryPin)                           ,
    _vMin         (3.6)                                  ,
    _vMax         (4.2)                                  ,
    _posX         (250-22)                               ,
    _posY         (10)                                    ,
    _width        (20)                                   ,
    _height       (10)                                   ,
    _vSlope       (100.0f/(_vMax- _vMin))                ,       
    _vToBarSlope  (((float)_width - 2) / (_vMax - _vMin)),
    _prevBarLength(-1)
{
}

void Battery::init()
{
}

float Battery::getVoltage()
{
    return (float)(analogRead(_batteryPin)) * _adcScale;
}

int Battery::getPercentage()
{
    return (int)((getVoltage() - _vMin) * _vSlope);
}

void Battery::draw()
{
    // Draw outer lines
    _display->drawRect(_posX       , _posY  ,_width, _height  , GxEPD_BLACK); // outer walls battery
    _display->drawRect(_posX+_width, _posY+2,2     , _height-4, GxEPD_BLACK); // pin battery
    update();
}

void Battery::update()
{
    int voltage = getVoltage();
    Serial.print("voltage  "); Serial.println(voltage);

    // Draw percentage lines
    int barLength    = (int)((getVoltage() - _vMin) * _vToBarSlope);
    
    Serial.print("barLength  "); Serial.println(barLength);
    
    int remainLength = _width-barLength- 2;
    bool changed     = barLength != _prevBarLength;
    _display->fillRect(_posX+1          , _posY+2, barLength   , _height-4, GxEPD_BLACK); // black bar
    _display->fillRect(_posX+1+barLength, _posY+1, remainLength, _height-2, GxEPD_WHITE); // clear remaining  
    _display->updateRequest(changed ? Screen::partial : Screen::none);
}