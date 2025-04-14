#include "../include/movement.h"

// Basic motor controls
void moveMotor(int motorIndex, int speed, bool forward)
{
    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];

    analogWrite(enPin, speed); // Set motor speed

    if (forward) {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
    } else {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    }
}

void stopMotor(int motorIndex)
{
    int pin1 = motorPins[motorIndex * 2];
     int pin2 = motorPins[motorIndex * 2 + 1];
     int enPin = enablePins[motorIndex];
     
     digitalWrite(pin1, LOW);
     digitalWrite(pin2, LOW);
     analogWrite(enPin, 0);
}

void stopAllMotors()
{
    stopMotor(0);
    stopMotor(1);
    stopMotor(2);
    stopMotor(3);
}

void moveServo(int servoIndex, int angle)
{
    if (servoIndex >= 0 && servoIndex < 2) {
        servos[servoIndex].write(angle);
    }
}

Result moveForward(int speed, float distance_cm)
{
    Result result;

    if (distance_cm <= 0 || speed <= 0 || speed > 255) {
        result.success = false;
        result.failure_reason = "Invalid speed or distance.";
        return result;
    }

    // Start all motors
    moveMotor(0, speed, false);
    moveMotor(1, speed, false);
    moveMotor(2, speed, false);
    moveMotor(3, speed, true);

    // Calculate time to move
    int moveTime = distance_cm / 0.01;
    delay(moveTime);

    // Stop the motors
    stopAllMotors();

    // Return success
    result.success = true;
    result.failure_reason = "";
    return result;
}


Result moveForwardTime(int speed, unsigned long timeMs)
{
    Result result;

    // Validate input
    if (speed < 0 || speed > 255) {
        result.success = false;
        result.failure_reason = "Speed must be between 0 and 255.";
        return result;
    }

    if (timeMs == 0) {
        result.success = false;
        result.failure_reason = "Time must be greater than 0 ms.";
        return result;
    }

    // Move all motors forward
    moveMotor(0, speed, false);
    moveMotor(1, speed, false);
    moveMotor(2, speed, false);
    moveMotor(3, speed, true); // Reverse polarity for mirrored motor

    // Wait for the specified duration
    delay(timeMs);

    // Stop all motors
    stopAllMotors();

    // Return success
    result.success = true;
    result.failure_reason = "";
    // You can optionally fill in success_result data here

    return result;
}



Result moveForwardRotations(int speed, float rotations)
{
    Result result;

    if (speed < 0 || speed > 255) {
        result.success = false;
        result.failure_reason = "Speed must be between 0 and 255.";
        return result;
    }

    if (rotations <= 0) {
        result.success = false;
        result.failure_reason = "Rotations must be greater than 0.";
        return result;
    }

    float wheelCircumference = PI * WHEEL_DIAMETER_CM; // ~20.74 cm
    float distance_cm = rotations * wheelCircumference;

    unsigned long moveTimeMs = distance_cm / CM_PER_MS;

    // Start motors
    moveMotor(0, speed, false);
    moveMotor(1, speed, false);
    moveMotor(2, speed, false);
    moveMotor(3, speed, true);

    delay(moveTimeMs);

    stopAllMotors();

    result.success = true;
    result.failure_reason = "";
    return result;
}


// Rotation operations
Result turnLeftRight(String direction, float radius, int speed, float distance)
{
    Result result;
    // Implementation will go here
    return result;
}

Result turnLeftRightTime(String direction, float radius, int speed, unsigned long timeMs)
{
    Result result;
    // Implementation will go here
    return result;
}

Result turnLeftRightRotations(String direction, float radius, int speed, float rotations)
{
    Result result;
    // Implementation will go here
    return result;
}