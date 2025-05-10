#include "include/movement.h"
#include "include/sensor.h"
#include "include/gyro.h"

float normalizeAngle(float angle)
{
    while (angle > 180.0f)
        angle -= 360.0f;
    while (angle < -180.0f)
        angle += 360.0f;
    return angle;
}

Result rotate(const String &direction, int speed, float angleDeg)
{
    Result result;

    // Validate parameters
    if (speed <= 0 || speed > 255)
    {
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result;
    }

    if (angleDeg <= 0)
    {
        result.success = false;
        result.failure_reason = "Angle must be positive.";
        return result;
    }

    if (direction != "left" && direction != "right")
    {
        result.success = false;
        result.failure_reason = "Direction must be 'left' or 'right'.";
        return result;
    }

    // Store the initial relative yaw at the start of the rotation
    float startRelativeYaw = getRelativeYaw(true);

    // For differential steering, we move the wheels on opposite sides in opposite directions
    auto setDifferentialSteering = [&](int spd)
    {
        if (direction == "left")
        {
            // Left motors backward, right motors forward
            moveMotor(FRONT_LEFT_MOTOR_INDEX, -spd);
            moveMotor(REAR_LEFT_MOTOR_INDEX, -spd);
            moveMotor(FRONT_RIGHT_MOTOR_INDEX, spd);
            moveMotor(REAR_RIGHT_MOTOR_INDEX, spd);
        }
        else // right
        {
            // Left motors forward, right motors backward
            moveMotor(FRONT_LEFT_MOTOR_INDEX, spd);
            moveMotor(REAR_LEFT_MOTOR_INDEX, spd);
            moveMotor(FRONT_RIGHT_MOTOR_INDEX, -spd);
            moveMotor(REAR_RIGHT_MOTOR_INDEX, -spd);
        }
    };

    // Calculate target relative yaw
    float targetAngle = direction == "left" ? angleDeg : -angleDeg;
    float targetRelativeYaw = normalizeAngle(startRelativeYaw + targetAngle);

    // Simplified approach: use constant speed with minor adjustments
    int rotationSpeed = max(60, speed); // Ensure minimum speed is high enough to prevent stalling
    unsigned long startTime = millis();

    // Start rotation at full speed
    setDifferentialSteering(rotationSpeed);

    while (true)
    {
        float currentRelativeYaw = getRelativeYaw(true);
        float yawError = normalizeAngle(targetRelativeYaw - currentRelativeYaw);
        float absError = abs(yawError);

        // Debug prints
        Serial.print("Current Yaw: ");
        Serial.print(currentRelativeYaw);
        Serial.print(" | Target: ");
        Serial.print(targetRelativeYaw);
        Serial.print(" | Error: ");
        Serial.println(absError);

        // Stop if within threshold - using a slightly larger threshold
        if (absError <= 2.0f)
            break;

        // Safety timeout - 5 seconds max for a turn
        if (millis() - startTime > 5000)
        {
            Serial.println("Rotation timeout!");
            break;
        }

        delay(10); // Small delay to not overwhelm the processor
    }

    // Stop all motors
    stopAllMotors();

    // Calculate actual angle turned
    float finalRelativeYaw = getRelativeYaw(true);
    float actualAngleTurned = normalizeAngle(finalRelativeYaw - startRelativeYaw);
    unsigned long timeTaken = millis() - startTime;

    // Prepare success result
    result.success = true;
    String successData = "{";
    successData += "\"angle_turned\":" + String(abs(actualAngleTurned), 2) + ",";
    successData += "\"time_ms\":" + String(timeTaken);
    successData += "}";
    result.success_result = successData;

    return result;
}
