#include "../include/movement.h"

void moveMotor(int motorIndex, int speed)
{
    if (motorIndex == 3)
    { // motor 4 is reversed
        speed = -speed;
    }

    bool reverse = speed < 0;

    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];

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
    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];

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

void moveSteeringServo(int servoIndex, int angle)
{
    if (servoIndex != FRONT_LEFT_SERVO_INDEX && servoIndex != FRONT_RIGHT_SERVO_INDEX)
    {
        throw std::invalid_argument("Invalid servo index");
    }

    servos[servoIndex].write(angle);
}