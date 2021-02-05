#pragma once
#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>

#define ELINK_SS     5
#define ELINK_BUSY   4
#define ELINK_RESET 16
#define ELINK_DC    17


class Screen : public GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT>
{
    public:
        enum UpdateType {
            none,
            partial,
            full,
        };
    private:
        UpdateType _updateType;
    public:
        Screen(int8_t cs, int8_t dc, int8_t rst, int8_t busy) :  GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT>(GxEPD2_213_B72(cs, dc, rst, busy)) {};
        void updateRequest(UpdateType updateType);
        void update();
};

