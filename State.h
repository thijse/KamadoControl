#pragma once
#include "arduino.h"

/// <summary>
/// Class that can copy state variables from one task to another
/// Note that the variable should be copyable, such as a primitive or a class with a copy constructor
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class State
{
public:
    State(const State& other) = delete;
    State& operator=(const State& other) = delete;

    State() : _mutex(nullptr), _pending(false)
    {        
        _mutex = xSemaphoreCreateMutex();
    }

    void set(T& value) {
        if (xSemaphoreTake(_mutex, (TickType_t)0))
        {
            _value   = value;
            _pending = true;
            xSemaphoreGive(_mutex);
        }
    }
    void get(T& value) {
        if (xSemaphoreTake(_mutex, (TickType_t)0))
        {
            value = _value;
            _pending = false;
            xSemaphoreGive(_mutex);
        }
    }

    bool isPending() const
    {
        return _pending;
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
    SemaphoreHandle_t _mutex;
    bool             _pending;
};