#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Servo.h>

extern const int ULTRASONIC_TRIGGER_PIN;
extern const int ULTRASONIC_ECHO_PIN;
extern const int PEN_SERVO_PIN;

// Servo Objects
extern Servo penServo;

// Pen States
extern const int PEN_UP_ANGLE;
extern const int PEN_DOWN_ANGLE;
extern bool isPenDown;

// Result Structure
struct Result
{
    bool success;
    String failure_reason;
    String success_result;

    // Method declaration
    String toJSON();
};

#endif // GLOBALS_H