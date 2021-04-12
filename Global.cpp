#include "Global.h"

SemaphoreHandle_t wireMutex = xSemaphoreCreateMutex();

Screen            display(wireMutex, ELINK_SS, ELINK_DC, ELINK_RESET, ELINK_BUSY);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN);