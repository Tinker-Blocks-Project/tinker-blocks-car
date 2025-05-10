#include "include/movement.h"
#include "include/sensor.h"
#include "include/gyro.h"

// Static variable to track accumulated relative rotation angle
static float accumulatedAngle = 0.0f;

float normalizeAngle(float angle)
{
    while (angle > 180.0f)
        angle -= 360.0f;
    while (angle < -180.0f)
        angle += 360.0f;
    return angle;
}

// Reset the accumulated angle tracking
void resetRotationTracking()
{
    accumulatedAngle = 0.0f;
}

// Get the current accumulated angle from relative rotations
float getAccumulatedAngle()
{
    return accumulatedAngle;
}

// Rotate to an absolute angle from north
Result rotateToAbsolute(float targetAngleDeg, int speed)
{
    Result result;

    // Validate parameters
    if (speed <= 0 || speed > 255)
    {
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result;
    }

    // Get current absolute yaw (from north)
    float currentAngle = getYaw(true); // Get absolute angle in degrees

    // Calculate the shortest path to the target angle
    float angleDifference = normalizeAngle(targetAngleDeg - currentAngle);

    // Debug info
    Serial.print("Current Angle: ");
    Serial.print(currentAngle);
    Serial.print(" | Target Angle: ");
    Serial.print(targetAngleDeg);
    Serial.print(" | Difference: ");
    Serial.println(angleDifference);

    // Use our PID controller to rotate the calculated amount
    return rotateRelative(angleDifference, speed);
}

// Unified rotate function with absolute/relative flag
Result rotate(float angleDeg, int speed, bool absolute)
{
    // For absolute rotation, angle is the target angle
    if (absolute)
    {
        return rotateToAbsolute(angleDeg, speed);
    }

    // For relative rotation, update our accumulated angle and then use absolute rotation
    accumulatedAngle = normalizeAngle(accumulatedAngle + angleDeg);

    // Use the accumulated angle to determine absolute target
    return rotateToAbsolute(accumulatedAngle, speed);
}

// Rotate a relative amount (positive = left/CCW, negative = right/CW)
Result rotateRelative(float angleDeg, int speed)
{
    Result result;

    // Validate parameters
    if (speed <= 0 || speed > 255)
    {
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result;
    }

    if (angleDeg == 0)
    {
        result.success = true;
        result.success_result = "{\"angle_turned\":0,\"time_ms\":0,\"direction_changes\":0}";
        return result;
    }

    // Determine direction and angle magnitude
    bool isLeftTurn = angleDeg > 0;
    float absAngle = abs(angleDeg);

    // PID controller constants - more aggressive values
    const float Kp = 4.0f;                      // Increased proportional constant
    const float Ki = 0.2f;                      // Increased integral constant
    const float Kd = 0.3f;                      // Reduced derivative to allow faster response
    const int MIN_SPEED = 70;                   // Increased minimum speed
    const int MAX_SPEED = min(255, speed + 20); // Allow slightly higher max speed
    const float TARGET_THRESHOLD = 1.0f;        // Keep the same threshold
    const int MAX_DIRECTION_CHANGES = 5;        // Maximum number of direction changes allowed

    // Store the initial relative yaw at the start of the rotation
    float startRelativeYaw = getRelativeYaw(true);

    // Calculate target relative yaw
    float targetRelativeYaw = normalizeAngle(startRelativeYaw + angleDeg);

    // PID variables
    float integral = 0.0f;
    float prevError = 0.0f;
    float currentRelativeYaw = startRelativeYaw;
    unsigned long prevTime = millis();
    unsigned long startTime = prevTime;
    bool reachedTarget = false;
    int stableCycles = 0;

    // Direction change tracking
    int lastDirection = 0; // 0 = not set, 1 = positive, -1 = negative
    int directionChanges = 0;

    // Debug every 100ms
    unsigned long lastDebugTime = 0;

    // Loop until we reach the target angle
    while (!reachedTarget)
    {
        // Get current position
        currentRelativeYaw = getRelativeYaw(true);
        unsigned long currentTime = millis();
        float deltaTime = (currentTime - prevTime) / 1000.0f; // Convert to seconds
        prevTime = currentTime;

        // Calculate error
        float error = normalizeAngle(targetRelativeYaw - currentRelativeYaw);

        // Update integral with lower limits to allow faster accumulation
        integral += error * deltaTime;
        float integralLimit = 20.0f; // Reduced from 30.0f to allow more aggressive response
        integral = constrain(integral, -integralLimit, integralLimit);

        // Calculate derivative (rate of change of error)
        float derivative = (error - prevError) / max(deltaTime, 0.001f); // Avoid division by zero
        prevError = error;

        // Calculate PID output
        float pidOutput = (Kp * error) + (Ki * integral) + (Kd * derivative);

        // Determine direction and speed from PID output
        int currentDirection = (pidOutput >= 0) ? 1 : -1;

        // Track direction changes
        if (lastDirection != 0 && currentDirection != lastDirection)
        {
            directionChanges++;
            Serial.print("Direction change #");
            Serial.print(directionChanges);
            Serial.println(" detected");

            // Stop if too many direction changes (oscillation)
            if (directionChanges >= MAX_DIRECTION_CHANGES)
            {
                Serial.println("Too many direction changes - stopping rotation");
                break;
            }
        }
        lastDirection = currentDirection;

        // Convert PID output to motor speed with minimum threshold
        // Use a more aggressive scaling for speed calculation
        int motorSpeed = abs(pidOutput);

        // Boost small outputs to ensure motion
        if (motorSpeed < MIN_SPEED * 1.5 && motorSpeed > 5)
        {
            motorSpeed = MIN_SPEED * 1.5;
        }

        motorSpeed = constrain(motorSpeed, MIN_SPEED, MAX_SPEED);

        // Apply motor speeds based on current correction direction
        if (currentDirection > 0)
        {
            // Turn left/CCW
            moveMotor(FRONT_LEFT_MOTOR_INDEX, -motorSpeed);
            moveMotor(REAR_LEFT_MOTOR_INDEX, -motorSpeed);
            moveMotor(FRONT_RIGHT_MOTOR_INDEX, motorSpeed);
            moveMotor(REAR_RIGHT_MOTOR_INDEX, motorSpeed);
        }
        else
        {
            // Turn right/CW
            moveMotor(FRONT_LEFT_MOTOR_INDEX, motorSpeed);
            moveMotor(REAR_LEFT_MOTOR_INDEX, motorSpeed);
            moveMotor(FRONT_RIGHT_MOTOR_INDEX, -motorSpeed);
            moveMotor(REAR_RIGHT_MOTOR_INDEX, -motorSpeed);
        }

        // Log debug info every 100ms
        if (currentTime - lastDebugTime > 100)
        {
            Serial.print("Yaw: ");
            Serial.print(currentRelativeYaw);
            Serial.print(" | Target: ");
            Serial.print(targetRelativeYaw);
            Serial.print(" | Error: ");
            Serial.print(error);
            Serial.print(" | PID: ");
            Serial.print(pidOutput);
            Serial.print(" | Speed: ");
            Serial.print(motorSpeed);
            Serial.print(" | Dir Changes: ");
            Serial.println(directionChanges);
            lastDebugTime = currentTime;
        }

        // Check if we've reached the target
        if (abs(error) <= TARGET_THRESHOLD)
        {
            stableCycles++;
            if (stableCycles >= 3)
            { // Reduced from 5 to 3 for faster completion
                reachedTarget = true;
            }
        }
        else
        {
            stableCycles = 0;
        }

        // Safety timeout - 5 seconds max for a turn
        if (currentTime - startTime > 5000)
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
    successData += "\"time_ms\":" + String(timeTaken) + ",";
    successData += "\"direction_changes\":" + String(directionChanges);
    successData += "}";
    result.success_result = successData;

    return result;
}
