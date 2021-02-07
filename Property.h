#pragma once
#include "arduino.h"

template <class T>
class Property
{
public:
    Property()
    {
        _mutex = xSemaphoreCreateMutex();
    }

    void set(T& value) {
        if (xSemaphoreTake(_mutex, (TickType_t)0))
        {
            _value = value;
            xSemaphoreGive(_mutex);
        }
    }
    void get(T& value) {
        if (xSemaphoreTake(_mutex, (TickType_t)0))
        {
            value = _value;
            xSemaphoreGive(_mutex);
        }
    }

    T get() {
        T value;
        if (xSemaphoreTake(_mutex, (TickType_t)0))
        {
            value = _value;
            xSemaphoreGive(_mutex);
        }
        return value;
    }

private:
    T                 _value;
    SemaphoreHandle_t _mutex = NULL;
};