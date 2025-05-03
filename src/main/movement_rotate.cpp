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

Result rotate(int speed, float radius, float angleDeg)
{
    Result result;

    // Validate radius
    if (radius <= 0)
    {
        result.success = false;
        result.failure_reason = "Invalid radius. Must be greater than 0.";
        return result;
    }

    if (speed == 0)
    {
        result.success = false;
        result.failure_reason = "Speed must be nonzero.";
        return result;
    }

    // Store the initial relative yaw at the start of the rotation
    float startRelativeYaw = getRelativeYaw(true);

    // Calculate steering angles based on turning radius
    const int MAX_STEERING_ANGLE = 30;                 // Maximum steering angle from center
    float steeringFactor = min(1.0f, 100.0f / radius); // 100cm as reference radius
    int steeringAngle = steeringFactor * MAX_STEERING_ANGLE;

    // Determine direction based on sign of angleDeg
    bool isLeft = angleDeg > 0;
    int absSteeringAngle = abs(steeringAngle);

    // Apply steering to servos based on direction
    if (isLeft)
    {
        steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE - absSteeringAngle);
        steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE - absSteeringAngle);
    }
    else
    {
        steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE + absSteeringAngle);
        steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE + absSteeringAngle);
    }

    // For sharper turns, adjust motor speeds
    auto setMotorSpeeds = [&](int spd)
    {
        if (radius < 50)
        {
            if (isLeft)
            {
                moveMotor(FRONT_LEFT_MOTOR_INDEX, spd * 0.7);
                moveMotor(REAR_LEFT_MOTOR_INDEX, spd * 0.7);
                moveMotor(FRONT_RIGHT_MOTOR_INDEX, spd);
                moveMotor(REAR_RIGHT_MOTOR_INDEX, spd);
            }
            else
            {
                moveMotor(FRONT_LEFT_MOTOR_INDEX, spd);
                moveMotor(REAR_LEFT_MOTOR_INDEX, spd);
                moveMotor(FRONT_RIGHT_MOTOR_INDEX, spd * 0.7);
                moveMotor(REAR_RIGHT_MOTOR_INDEX, spd * 0.7);
            }
        }
        else
        {
            moveAllMotors(spd);
        }
    };

    // Calculate target relative yaw
    float targetRelativeYaw = normalizeAngle(startRelativeYaw + angleDeg);

    float lastYawError = normalizeAngle(targetRelativeYaw - startRelativeYaw);

    while (true)
    {
        float currentRelativeYaw = getRelativeYaw(true);
        float yawError = normalizeAngle(targetRelativeYaw - currentRelativeYaw);
        float absError = abs(yawError);

        // Gradual slowdown logic using a harder (quadratic) equation
        // Speed scales quadratically from minSpeed to maxSpeed as absError goes from 0 to maxError
        const int minSpeed = 40; // Minimum speed for control
        const int maxSpeed = abs(speed);
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
        setMotorSpeeds(currentSpeed);

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
        lastYawError = yawError;
        delay(0); // Increase frequency of control loop
    }

    // Stop all motors
    stopAllMotors();

    // Reset steering to straight position
    steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE);
    steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE);

    // Prepare success result
    result.success = true;
    String successData = "{";
    successData += "\"angle_turned\":" + String(angleDeg) + ",";
    successData += "\"final_relative_yaw\":" + String(getRelativeYaw(true), 2);
    successData += "}";
    result.success_result = successData;

    return result;
}
