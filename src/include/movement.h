#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "globals.h"

// Physics and wheel constants
#define WHEEL_DIAMETER_CM 6.6
#define WHEEL_CIRCUMFERENCE_CM (WHEEL_DIAMETER_CM * PI)
#define MAX_SPEED_CM_PER_MS 0.01 // Calibrate this value with real measurements

// Movement Parameters Structure
// This structure allows passing any combination of movement parameters
// while maintaining a consistent interface
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
void moveMotor(int motorIndex, int speed, bool forward);
void stopMotor(int motorIndex);
void stopAllMotors();
void moveServo(int servoIndex, int angle);

// Movement operations
Result translate(const MovementParams &params);
Result rotate(const String &direction, float radius, const MovementParams &params);

#endif // MOVEMENT_H