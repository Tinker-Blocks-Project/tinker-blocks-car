#include "include/movement.h"

const int MOTOR_PINS[] = {22, 23, 30, 31, 32, 33, 24, 25};
const int ENABLE_PINS[] = {6, 7, 8, 4};

void setupMovement()
{
    for (int i = 0; i < 8; i++)
    {
        pinMode(MOTOR_PINS[i], OUTPUT);
    }

    for (int i = 0; i < 4; i++)
    {
        pinMode(ENABLE_PINS[i], OUTPUT);
    }
}

void moveMotor(int motorIndex, int speed)
{
    if (motorIndex == FRONT_LEFT_MOTOR_INDEX)
    {
        speed = -speed;
    }

    bool reverse = speed < 0;

    int pin1 = MOTOR_PINS[motorIndex * 2];
    int pin2 = MOTOR_PINS[motorIndex * 2 + 1];
    int enPin = ENABLE_PINS[motorIndex];

    analogWrite(enPin, abs(speed));

    digitalWrite(pin1, reverse ? LOW : HIGH);
    digitalWrite(pin2, reverse ? HIGH : LOW);
}

void moveAllMotors(int speed)
{
    moveMotor(FRONT_LEFT_MOTOR_INDEX, speed);
    moveMotor(FRONT_RIGHT_MOTOR_INDEX, speed);
    moveMotor(REAR_LEFT_MOTOR_INDEX, speed);
    moveMotor(REAR_RIGHT_MOTOR_INDEX, speed);
}

void stopMotor(int motorIndex)
{
    int pin1 = MOTOR_PINS[motorIndex * 2];
    int pin2 = MOTOR_PINS[motorIndex * 2 + 1];
    int enPin = ENABLE_PINS[motorIndex];

    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    analogWrite(enPin, 0);
}

void stopAllMotors()
{
    stopMotor(FRONT_LEFT_MOTOR_INDEX);
    stopMotor(FRONT_RIGHT_MOTOR_INDEX);
    stopMotor(REAR_LEFT_MOTOR_INDEX);
    stopMotor(REAR_RIGHT_MOTOR_INDEX);
}
