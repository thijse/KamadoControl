#include "Global.h"

SemaphoreHandle_t wireMutex = xSemaphoreCreateMutex();

Screen            display(wireMutex, ELINK_SS, ELINK_DC, ELINK_RESET, ELINK_BUSY);