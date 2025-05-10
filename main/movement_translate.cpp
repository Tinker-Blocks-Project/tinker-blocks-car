#include "include/movement.h"
#include "include/sensor.h"
#include "include/gyro.h"

// Constants for yaw-based correction - reduced aggressiveness
const float YAW_THRESHOLD = 0.5;        // degrees (increased from 0.2 to be less sensitive)
const int BASE_CORRECTION = 20;         // Base PWM adjustment (reduced from 50)
const int MIN_SPEED_DIFF = 40;          // Minimum speed difference (reduced from 80)
const float CORRECTION_FACTOR = 2.0;    // Multiplier for yaw error (reduced from 5.0)
const float CORRECTION_SMOOTHING = 0.7; // Smoothing factor for gradual correction (0-1, higher = smoother)

Result translate(const MovementParams &params, bool checkUltrasonic, bool enableYawCorrection)
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

    // If moving forward and ultrasonic checks are enabled, check for obstacles
    if (isForward && checkUltrasonic)
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

    // Previous iteration speeds for smoothing
    int prevLeftSpeed = leftSpeed;
    int prevRightSpeed = rightSpeed;

    // Store the initial relative yaw at the start of the movement
    float initialRelativeYaw = getRelativeYaw(true);

    // Start movement
    unsigned long startTime = millis();
    unsigned long endTime = startTime + params.timeMs;

    // Apply direction to base speed for moveAllMotors
    int directedSpeed = isForward ? baseSpeed : -baseSpeed;

    while (millis() < endTime)
    {
        if (!enableYawCorrection)
        {
            // When yaw correction is disabled, use moveAllMotors to ensure all motors run
            // at the same speed without any correction
            moveAllMotors(directedSpeed);
        }
        else
        {
            // Get current yaw relative to reference
            float currentYaw = getRelativeYaw(true);
            // Calculate yaw error relative to the initial yaw at the start of this movement
            float yawError = currentYaw - initialRelativeYaw;

            // Debug output
            // Serial.print("Yaw Error: ");
            // Serial.println(yawError);

            // Reset target speeds to base for this iteration
            int targetLeftSpeed = baseSpeed;
            int targetRightSpeed = baseSpeed;

            // Adjust motor speeds based on yaw error
            if (abs(yawError) > YAW_THRESHOLD)
            {
                // Calculate correction based on error magnitude
                // Use a quadratic scaling for smoother response to small errors
                float errorRatio = min(abs(yawError) / 10.0, 1.0); // Cap at 10 degrees max error
                int correction = BASE_CORRECTION + (int)(errorRatio * errorRatio * (MIN_SPEED_DIFF - BASE_CORRECTION));

                // Apply non-linear scaling for more gentle corrections
                correction = (int)(correction * CORRECTION_FACTOR * sqrt(abs(yawError)));

                if (yawError > 0)
                {
                    // We're drifting right (positive yaw error)
                    // To counter this, we need to steer left
                    targetLeftSpeed += correction;  // Increase left speed
                    targetRightSpeed -= correction; // Decrease right speed
                }
                else
                {
                    // We're drifting left (negative yaw error)
                    // To counter this, we need to steer right
                    targetLeftSpeed -= correction;  // Decrease left speed
                    targetRightSpeed += correction; // Increase right speed
                }

                // Ensure speeds stay within bounds
                targetLeftSpeed = constrain(targetLeftSpeed, 0, 255);
                targetRightSpeed = constrain(targetRightSpeed, 0, 255);

                // Apply smoothing between current and target speeds
                leftSpeed = prevLeftSpeed * CORRECTION_SMOOTHING + targetLeftSpeed * (1 - CORRECTION_SMOOTHING);
                rightSpeed = prevRightSpeed * CORRECTION_SMOOTHING + targetRightSpeed * (1 - CORRECTION_SMOOTHING);

                // Store current speeds for next iteration
                prevLeftSpeed = leftSpeed;
                prevRightSpeed = rightSpeed;

                // Debug output
                // Serial.print("Left: ");
                // Serial.print(leftSpeed);
                // Serial.print(" | Right: ");
                // Serial.println(rightSpeed);
            }
            else
            {
                // Reset to base speeds if error is small (with smoothing)
                leftSpeed = prevLeftSpeed * CORRECTION_SMOOTHING + baseSpeed * (1 - CORRECTION_SMOOTHING);
                rightSpeed = prevRightSpeed * CORRECTION_SMOOTHING + baseSpeed * (1 - CORRECTION_SMOOTHING);

                // Store current speeds for next iteration
                prevLeftSpeed = leftSpeed;
                prevRightSpeed = rightSpeed;
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
        }

        // Small delay for more responsive corrections
        delay(5);
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