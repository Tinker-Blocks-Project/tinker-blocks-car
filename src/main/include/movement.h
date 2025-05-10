#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "globals.h"

// Motor indexes
const int FRONT_LEFT_MOTOR_INDEX = 3; // reversed
const int FRONT_RIGHT_MOTOR_INDEX = 0;
const int REAR_LEFT_MOTOR_INDEX = 1;
const int REAR_RIGHT_MOTOR_INDEX = 2;

struct MovementParams
{
    // Only set the parameters you want to control
    int speed = -1;           // -255 to 255, negative means backward
    unsigned long timeMs = 0; // Time in milliseconds
    float distance = 0;       // Distance in centimeters

    // Static factory methods declarations
    static MovementParams fromSpeedAndDistance(int s, float d);
    static MovementParams fromSpeedAndTime(int s, unsigned long t);
    static MovementParams fromDistanceAndTime(float d, unsigned long t);

    // Helper method declarations
    bool isValid() const;
    String toJSON() const;
};

// Basic motor controls
void setupMovement();
void moveMotor(int motorIndex, int speed); // speed is a value between -255 and 255
void moveAllMotors(int speed);
void stopMotor(int motorIndex);
void stopAllMotors();

// Movement operations
Result translate(const MovementParams &params, bool checkUltrasonic = true, bool enableYawCorrection = true);
Result rotate(const String &direction, int speed, float angleDeg);

#endif // MOVEMENT_H