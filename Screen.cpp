#include "Screen.h"


void Screen::updateRequest(UpdateType updateType)
{
    _updateType = max(_updateType, updateType);
}

void Screen::update()
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
	_updateType = UpdateType::none;
}
