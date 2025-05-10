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

    float lastYawError = normalizeAngle(targetRelativeYaw - startRelativeYaw);
    unsigned long startTime = millis();

    while (true)
    {
        float currentRelativeYaw = getRelativeYaw(true);
        float yawError = normalizeAngle(targetRelativeYaw - currentRelativeYaw);
        float absError = abs(yawError);

        // Gradual slowdown logic
        const int minSpeed = 40; // Minimum speed to maintain rotation
        const int maxSpeed = speed;
        const float maxError = 30.0f; // Angle at which we use maxSpeed

        float errorRatio = absError / maxError;
        if (errorRatio > 1.0f)
            errorRatio = 1.0f;

        // Quadratic scaling: drops off faster as error decreases
        int currentSpeed = minSpeed + (int)((maxSpeed - minSpeed) * (errorRatio * errorRatio));
        if (currentSpeed > maxSpeed)
            currentSpeed = maxSpeed;
        if (currentSpeed < minSpeed)
            currentSpeed = minSpeed;

        setDifferentialSteering(currentSpeed);

        // Debug prints
        Serial.print("Current Relative Yaw: ");
        Serial.print(currentRelativeYaw);
        Serial.print(" | Target Relative Yaw: ");
        Serial.print(targetRelativeYaw);
        Serial.print(" | Yaw Error: ");
        Serial.print(yawError);
        Serial.print(" | Speed: ");
        Serial.println(currentSpeed);

        // Stop if within threshold
        if (abs(yawError) <= 1.0f)
            break;

        // Stop if over-extended (sign of error changed)
        if ((lastYawError > 0 && yawError < 0) || (lastYawError < 0 && yawError > 0))
            break;

        // Safety timeout - 5 seconds max for a turn
        if (millis() - startTime > 5000)
        {
            Serial.println("Rotation timeout!");
            break;
        }

        lastYawError = yawError;
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
