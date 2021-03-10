#pragma once
#include "arduino.h"
#include <GxEPD2_BW.h>

#define ELINK_SS     5
#define ELINK_BUSY   4
#define ELINK_RESET 16
#define ELINK_DC    17


class Screen : public GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT>
{
    public:
        enum UpdateType {
            none    = 0,
            partial = 1,
            full    = 2,
        };
    private:
        UpdateType    _updateType;
        QueueHandle_t *_mutex;
    public:        
        Screen           (const Screen& other) = delete;        
        Screen& operator=(const Screen& other) = delete;

        Screen(SemaphoreHandle_t *mutex, int8_t cs, int8_t dc, int8_t rst, int8_t busy) :
            _mutex(mutex),
            GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT>(GxEPD2_213_B72(cs, dc, rst, busy)),
            _updateType(UpdateType::none)
        {
        };
        void updateRequest(UpdateType updateType);
        void update();
        void init(uint32_t serial_diag_bitrate = 0);
};

