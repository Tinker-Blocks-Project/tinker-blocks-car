#include "../include/movement.h"

// Basic motor controls
void moveMotor(int motorIndex, int speed, bool forward)
{
    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];

    analogWrite(enPin, speed); // Set motor speed

    if (forward)
    {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
    }
    else
    {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    }
}

void stopMotor(int motorIndex)
{
    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];

    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    analogWrite(enPin, 0);
}

void stopAllMotors()
{
    stopMotor(0);
    stopMotor(1);
    stopMotor(2);
    stopMotor(3);
}

void moveServo(int servoIndex, int angle)
{
    if (servoIndex >= 0 && servoIndex < 2)
    {
        servos[servoIndex].write(angle);
    }
}