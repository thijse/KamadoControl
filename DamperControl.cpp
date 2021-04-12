#include "DamperControl.h"
#include "Global.h"

DamperControl::DamperControl() :
_active(false)
{
	pinMode(O_50V2, OUTPUT);
    pinMode(O_50V1, OUTPUT);
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	_servo.setPeriodHertz(50);    // standard 50 hz servo
	_servo.attach(O_PWM1, 1000, 2000); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
}

DamperControl::~DamperControl()
{
	_servo.detach();
}

void DamperControl::setActive(bool isActive)
{
    digitalWrite(O_50V2, isActive?HIGH:LOW);
}

void DamperControl::setOpen(int angleValue)
{
	if (angleValue == _previousValue) return;
	_previousValue = angleValue;
	Serial.println("Set servo: "); Serial.println(angleValue);
	_servo.write(angleValue);
}
