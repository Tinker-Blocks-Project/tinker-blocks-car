#include "include/movement.h"
#include "include/sensor.h"
#include "include/gyro.h"

// Constants for yaw-based correction
const float YAW_THRESHOLD = 0.2;     // degrees (reduced from 0.5)
const int BASE_CORRECTION = 50;      // Base PWM adjustment (increased from 20)
const int MIN_SPEED_DIFF = 80;       // Minimum speed difference (increased from 40)
const float CORRECTION_FACTOR = 5.0; // Multiplier for yaw error (increased from 2.0)

Result translate(const MovementParams &params)
{
    Result result;

    // Validate parameters
    if (!params.isValid())
    {
        result.success = false;
        result.failure_reason = "Invalid movement parameters";
        return result;
    }

    // Check direction and obstacle
    bool isForward = params.speed > 0;

    // If moving forward, check for obstacles
    if (isForward)
    {
        float distance = getDistanceToObstacle();
        if (distance > 0 && distance < params.distance)
        {
            result.success = false;
            result.failure_reason = "Obstacle detected at " + String(distance) + " cm";
            return result;
        }
    }

    // Calculate base motor speeds
    int baseSpeed = abs(params.speed);
    int leftSpeed = baseSpeed;
    int rightSpeed = baseSpeed;

    // Store the initial relative yaw at the start of the movement
    float initialRelativeYaw = getRelativeYaw(true);

    // Start movement
    unsigned long startTime = millis();
    unsigned long endTime = startTime + params.timeMs;

    while (millis() < endTime)
    {
        // Get current yaw relative to reference
        float currentYaw = getRelativeYaw(true);
        // Calculate yaw error relative to the initial yaw at the start of this movement
        float yawError = currentYaw - initialRelativeYaw;

        // Adjust motor speeds based on yaw error
        if (abs(yawError) > YAW_THRESHOLD)
        {
            // Calculate correction based on error magnitude
            int correction = BASE_CORRECTION + abs(yawError) * CORRECTION_FACTOR;

            if (yawError > 0)
            {                             // Drifting right
                leftSpeed -= correction;  // Inverted: decrease left speed
                rightSpeed += correction; // Inverted: increase right speed
            }
            else
            {                             // Drifting left
                leftSpeed += correction;  // Inverted: increase left speed
                rightSpeed -= correction; // Inverted: decrease right speed
            }

            // Ensure minimum speed difference
            int speedDiff = abs(leftSpeed - rightSpeed);
            if (speedDiff < MIN_SPEED_DIFF)
            {
                if (leftSpeed > rightSpeed)
                {
                    leftSpeed += MIN_SPEED_DIFF - speedDiff;
                }
                else
                {
                    rightSpeed += MIN_SPEED_DIFF - speedDiff;
                }
            }

            // Ensure speeds stay within bounds
            leftSpeed = constrain(leftSpeed, 0, 255);
            rightSpeed = constrain(rightSpeed, 0, 255);
        }

        // Apply direction
        if (!isForward)
        {
            leftSpeed = -leftSpeed;
            rightSpeed = -rightSpeed;
        }

        // Move motors with corrected speeds
        moveMotor(FRONT_LEFT_MOTOR_INDEX, leftSpeed);
        moveMotor(REAR_LEFT_MOTOR_INDEX, leftSpeed);
        moveMotor(FRONT_RIGHT_MOTOR_INDEX, rightSpeed);
        moveMotor(REAR_RIGHT_MOTOR_INDEX, rightSpeed);

        // Smaller delay for more responsive corrections
        delay(5); // Reduced from 10ms
    }

    // Stop all motors
    stopAllMotors();

    // Prepare success result
    result.success = true;
    String successData = "{";
    successData += "\"distance_traveled\":" + String(params.distance) + ",";
    successData += "\"time_taken\":" + String(params.timeMs) + ",";
    successData += "\"final_yaw\":" + String(getRelativeYaw(true), 2);
    successData += "}";
    result.success_result = successData;

    return result;
}