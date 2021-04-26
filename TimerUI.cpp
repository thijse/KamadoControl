#include "TimerUI.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Fonts/FreeSans8pt7b.h"
#include "Fonts/FreeSansBold8pt7b.h"
#include <Fonts/Picopixel.h>



TimerUI::TimerUI(Screen* display) :
    _display(display),
    _posX(175), _posY(10),
    _x(0), _y(0), _w(0), _h(0)
{
}

void TimerUI::init()
{
    _time_start_ms = millis();
    // Create writing area
    _display->setFont      (&FreeSansBold8pt7b);
    _display->setCursor    (_posX, _posY);
    _display->getTextBounds("00:00", _posX, _posY, &_x, &_y, &_w, &_h);
    _x--; _y--; _w+=2; _h+=2;  // 1 pixel border
}

void TimerUI::draw()
{
    // initial drawing same as update
    update();
}

void TimerUI::update()
{
    _display->setFont(&FreeSansBold8pt7b);
    //Serial.print("Width  "); Serial.println(_display->width());
    //Serial.print("Height "); Serial.println(_display->height());
    //Serial.print("area   "); Serial.print(_x); Serial.print("  "); Serial.print(_y); Serial.print("  "); Serial.print(_w); Serial.print("  "); Serial.print(_h);


    unsigned long time_now_s = (millis() - _time_start_ms) / 1000;
    char time_string[]       = { '0', '0', ':', '0', '0', '\0' };
    time_string[0]           = time_now_s / 60 / 10 + '0';
    time_string[1]           = time_now_s / 60 % 10 + '0';
    time_string[3]           = time_now_s % 60 / 10 + '0';
    time_string[4]           = time_now_s % 60 % 10 + '0';
    
    //_display->setFont(&FreeSans8pt7b);
    _display->setTextColor(GxEPD_BLACK);
    _display->setCursor(_posX, _posY);
    _display->fillRect(_x, _y, _w, _h, GxEPD_WHITE);
    _display->setCursor(_posX, _posY);
    _display->println(time_string);

    if (time_string[0] != _prevTime[0]) {
        _display->updateRequest(Screen::full); // Always a full update on last digit change
    }
    else {
        _display->updateRequest((time_string[1] == _prevTime[1] && time_string[3] == _prevTime[3] && time_string[4] == _prevTime[4]) ? Screen::none : Screen::partial); // partial update on other items
    }
    strncpy(_prevTime, time_string, 6);
}


