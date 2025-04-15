#include "include/movement.h"

const int MOTOR_PINS[] = {22, 23, 24, 25, 30, 31, 32, 33};
const int ENABLE_PINS[] = {4, 6, 3, 8};
const int SERVO_PINS[] = {2, 11};

Servo frontLeftServo;
Servo frontRightServo;

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

    for (int i = 0; i < 2; i++)
    {
        pinMode(SERVO_PINS[i], OUTPUT);
    }

    frontLeftServo.attach(SERVO_PINS[FRONT_LEFT_SERVO_INDEX]);
    frontRightServo.attach(SERVO_PINS[FRONT_RIGHT_SERVO_INDEX]);

    steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE);
    steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE);
}

void moveMotor(int motorIndex, int speed)
{
    if (motorIndex != 3)
    { // motor 4 is the only one not reversed in the car
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

void steerServo(int servoIndex, int angle)
{
    if (servoIndex == FRONT_LEFT_SERVO_INDEX)
    {
        frontLeftServo.write(angle);
    }
    else if (servoIndex == FRONT_RIGHT_SERVO_INDEX)
    {
        frontRightServo.write(angle);
    }
}

void steerAllServos(int angle)
{
    steerServo(FRONT_LEFT_SERVO_INDEX, angle);
    steerServo(FRONT_RIGHT_SERVO_INDEX, angle);
}