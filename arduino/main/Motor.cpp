#include "include/Motor.h"

Motor::Motor(int pin1, int pin2, int enPin, bool invert)
    : dirPin1(pin1), dirPin2(pin2), enablePin(enPin), inverted(invert)
{
}

void Motor::setup()
{
    pinMode(dirPin1, OUTPUT);
    pinMode(dirPin2, OUTPUT);
    pinMode(enablePin, OUTPUT);
    stop();
}

void Motor::move(int speed)
{
    // Apply inversion if needed
    if (inverted)
    {
        speed = -speed;
    }

    bool reverse = speed < 0;

    // Set the motor direction
    digitalWrite(dirPin1, reverse ? LOW : HIGH);
    digitalWrite(dirPin2, reverse ? HIGH : LOW);

    // Set the speed
    analogWrite(enablePin, abs(speed));
}

void Motor::stop()
{
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, LOW);
    analogWrite(enablePin, 0);
}