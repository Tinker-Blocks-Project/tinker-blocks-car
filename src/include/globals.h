#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Servo.h>

extern const int ULTRASONIC_TRIGGER_PIN;
extern const int ULTRASONIC_ECHO_PIN;

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