#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
private:
    int dirPin1;
    int dirPin2;
    int enablePin;
    bool inverted;

public:
    Motor(int pin1, int pin2, int enPin, bool invert = false);

    void setup();
    void move(int speed); // speed is a value between -255 and 255
    void stop();
};

#endif // MOTOR_H