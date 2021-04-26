#include "MainValuesUI.h"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "Fonts/FreeSansBold32pt7b.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Fonts/FreeSans8pt7b.h"
#include "Fonts/FreeSansBold8pt7b.h"

// todo: rename class to SetValues;

MainValuesUI::MainValuesUI(Screen* display, ControlValues* controlValues, MeasurementData* measurements) :
    _display            (display),
    _controlValues      (controlValues),
    _measurements       (measurements),
    _posTempX           (25) , _posTempY  (80),    
    _posXDamper         (5)  , _posYDamper(10),
    _xTemp              (0)   , _yTemp  (0)   , _wTemp  (0), _hTemp  (0),
    _xDamper            (0)   , _yDamper(0)   , _wDamper(0), _hDamper(0),
    _targetTemperature  (20),
    _currentTemperature (20.0f),
    _damperValue        (0)
{

}

void MainValuesUI::init()
{
    // Create writing area Temperature
    _display->setFont      (&FreeMonoBold24pt7b);
    _display->setCursor    (_posTempX, _posTempY);
    _display->getTextBounds("000", _posTempX, _posTempY, &_xTemp, &_yTemp, &_wTemp, &_hTemp);
    _xTemp--; _yTemp--; _wTemp += 2; _hTemp += 2;  // 1 pixel border

    // Create writing area damper value
    _display->setFont(&FreeSansBold8pt7b);
    _display->setCursor(_posXDamper, _posYDamper);
    _display->getTextBounds("000", _posXDamper, _posYDamper, &_xDamper, &_yDamper, &_wDamper, &_hDamper);

}

void MainValuesUI::update(int16_t targetTemperature)
{
	_targetTemperature                = targetTemperature;
    _currentTemperature               = _measurements->temperatureResults.temperature[_controlValues->tempControlSource];
    _damperValue                      = _measurements->damperValue;

    _controlValues->targetTemperature = _targetTemperature;

    setCurrentTemperature(_currentTemperature, _damperValue);

     // Set damper value in header
    _display->setFont(&FreeSansBold8pt7b);
    _display->setTextColor(GxEPD_BLACK);
    _display->setCursor(_posXDamper, _posYDamper);
    _display->fillRect(_xDamper, _yDamper, _wDamper, _hDamper, GxEPD_WHITE);
    _display->setCursor(_posXDamper, _posYDamper);
    _display->println(_damperValue);

    _display->setFont      (&FreeMonoBold24pt7b);
    _display->setTextColor (GxEPD_BLACK);
    _display->setCursor    (_posTempX, _posTempY);
    _display->fillRect     (_xTemp, _yTemp, _wTemp, _hTemp, GxEPD_WHITE);
    _display->setCursor    (_posTempX, _posTempY);
    _display->print        (_targetTemperature);
    _display->setCursor    (_posTempX+100, _posTempY); // todo: make font dependent
    _display->fillRect     (_xTemp+100, _yTemp, _wTemp+40, _hTemp, GxEPD_WHITE);
    _display->setCursor    (_posTempX+100, _posTempY);
    _display->print        (_currentTemperature,1);
    _display->updateRequest(Screen::partial);
}

void MainValuesUI::draw()
{
    // initial drawing same as update
    //update();
}

int16_t MainValuesUI::getTargetTemperature() const
{
    return _targetTemperature;
}

void MainValuesUI::setCurrentTemperature(float currentTemperature, int damperValue)
{
    _currentTemperature = currentTemperature;
    _damperValue        = damperValue;
}

void MainValuesUI::setDamperValue(int damperValue)
{
	_damperValue = damperValue;
}
