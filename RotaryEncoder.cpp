#include "RotaryEncoder.h"

namespace RotaryEncoderHelper
{
    RotaryEncoder* pointerRotaryEncoder = nullptr;

    void IRAM_ATTR onRotaryTimer()
    {
        if (pointerRotaryEncoder != nullptr) pointerRotaryEncoder->update();
    }
}

void RotaryEncoder::init()
{
   if (RotaryEncoderHelper::pointerRotaryEncoder != nullptr)
    {
        //Log.errorln(F("Already a SetTemperature class instantiated. There can be only one "));
        return; // throw;
    }
   RotaryEncoderHelper::pointerRotaryEncoder = this;

    // Setup timer
    timer = timerBegin  (0, 80, true);
    timerAttachInterrupt(timer, &RotaryEncoderHelper::onRotaryTimer, true);
    timerAlarmWrite     (timer, 1000, true);
    timerAlarmEnable    (timer);
}

void RotaryEncoder::update()
{
    service();
}

RotaryEncoder     rotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, 4);



//int16_t RotaryEncoder::getValue(void)
//{
//    int16_t value =  RotaryEncoder::getValue();
//    Serial.print("RotaryEncoder delta: "); Serial.println(value);
//    return value;
//}

