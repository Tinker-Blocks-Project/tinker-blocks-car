#ifndef MOVEMENT_H
#define MOVEMENT_H

#define WHEEL_DIAMETER_CM 6.6
#define CM_PER_MS 0.01 // adjust after calibration

#include "globals.h"

// Basic motor controls
void moveMotor(int motorIndex, int speed, bool forward);
void stopMotor(int motorIndex);
void stopAllMotors();
void moveServo(int servoIndex, int angle);

// Movement operations
Result moveForward(int speed, float distance);
Result moveForwardTime(int speed, unsigned long timeMs);
Result moveForwardRotations(int speed, float rotations);

// Currently not supported due to lack of backward ultrasonic sensor
// Result moveBackward(int speed, float distance);

// Rotation operations
Result turnLeftRight(String direction, float radius, int speed, float distance);
Result turnLeftRightTime(String direction, float radius, int speed, unsigned long timeMs);
Result turnLeftRightRotations(String direction, float radius, int speed, float rotations);

#endif // MOVEMENT_H