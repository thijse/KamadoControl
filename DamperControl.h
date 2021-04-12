#pragma once
#include <ESP32Servo.h>

class DamperControl
{
public:
    DamperControl(const DamperControl& other) = delete;
    DamperControl& operator=(const DamperControl& other) = delete;

    // Constructor
    DamperControl ();
    // Destructor
    ~DamperControl();
    void setActive(bool isActive);
    void setOpen  (int angleValue);

private:
    Servo _servo;
    bool _active; 
    int _previousValue = - 1;
};

