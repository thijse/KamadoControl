#include "SDCard.h"


SDCard::SDCard(
        Screen* display,
        const int   sdCardPin) :
    _display  (display),
    _sdCardPin(sdCardPin),
    _prevCardSize(-2)
{
}

void SDCard::init(SPIClass* spiClass)
{
    _spiClass = spiClass;
    _posX     = _display->width() / 2 - 40;
    _posY     = _display->height() - 10;
}

void SDCard::draw()
{
    // initial drawing same as update
    update();
}

void SDCard::update()
{
    bool sdOK;
    if (!SD.begin(_sdCardPin, *_spiClass)) {
        sdOK = false;
    } else {
        sdOK = true;
    }

    _display->setTextColor(GxEPD_BLACK);
    _display->setCursor(_posX, _posY);    

    int32_t cardSize = -1;
    if (sdOK) {
        cardSize = SD.cardSize() / (1024 * 1024);
        _display->println("SDCard:" + String(cardSize) + "MB");
        Serial.println("SDCard"); 
    }
    else {
        _display->println("SDCard  None");
        Serial.println("SDCard None");
    }
    _display->updateRequest(cardSize != _prevCardSize ? Screen::partial : Screen::none);
    _prevCardSize = cardSize;
}

