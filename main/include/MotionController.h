#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "Motor.h"
#include "Result.h"
#include "MovementParams.h"

class MotionController
{
private:
    Motor frontLeftMotor;
    Motor frontRightMotor;
    Motor rearLeftMotor;
    Motor rearRightMotor;

    float accumulatedAngle;

public:
    MotionController(
        const Motor &frontLeft,
        const Motor &frontRight,
        const Motor &rearLeft,
        const Motor &rearRight);

    void setup();

    // Helpers
    static float normalizeAngle(float angle);

    // Basic motor controls
    void moveAllMotors(int speed);
    void stopAllMotors();

    // Movement operations
    Result translate(const MovementParams &params, bool checkUltrasonic = true, bool enableYawCorrection = true);

    // Rotation operations - Using signed angles
    // Positive angles = counterclockwise/left, negative angles = clockwise/right
    Result rotate(float angleDeg, int speed, bool absolute = false); // Main rotation function
    Result rotateToAbsolute(float targetAngleDeg, int speed);        // Rotate to absolute angle from north
    Result rotateRelative(float angleDeg, int speed);                // Rotate by relative angle (relative to current heading)

    // Rotation tracking for relative rotations
    void resetRotationTracking(); // Reset the accumulated angle tracking
    float getAccumulatedAngle();  // Get current accumulated angle
};

#endif // MOTION_CONTROLLER_H