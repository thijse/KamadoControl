#include "Screen.h"

void Screen::updateRequest(UpdateType updateType)
{
    _updateType = max(_updateType, updateType);
}

void Screen::update()
{

	if (xSemaphoreTake(*_mutex, (TickType_t)portMAX_DELAY))
	{
	    switch (_updateType)
	    {
	    case Screen::none:
		    // Do nothing
		    break;
	    case Screen::partial:
		    display(true); // partial update
		    break;
	    case Screen::full:
		    display(false); // full update
		    break;
	    default:
		    break;
	    }
	    xSemaphoreGive(*_mutex);
	}
	_updateType = UpdateType::none;
}

void Screen::init(uint32_t serial_diag_bitrate)
{
	GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT>::init(serial_diag_bitrate);
}