#ifndef PEN_CONTROLLER_H
#define PEN_CONTROLLER_H

#include <Arduino.h>
#include <Servo.h>

class PenController
{
private:
    Servo penServo;
    int servoPin;
    int penUpPos;
    int penDownPos;
    bool isDown;

public:
    PenController(int pin, int upPosition = 90, int downPosition = 0);

    void setup();
    void liftUp();
    void putDown();
    bool isLowered() const;

    // For fine control
    void setPosition(int position);
    int getPosition() const;
};

#endif // PEN_CONTROLLER_H