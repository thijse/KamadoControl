#pragma once
struct TemperatureResults
{
    volatile float temperature[6]{};
    volatile bool  success    [6]{};
};
