#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Servo.h>

struct Result
{
    bool success;
    String failure_reason;
    String success_result;

    String toJSON();
};

#endif // GLOBALS_H