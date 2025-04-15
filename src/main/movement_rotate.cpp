#include "include/movement.h"
#include "include/sensor.h"

Result rotate(const String &direction, float radius, const MovementParams &params)
{
    Result result;

    // Validate direction and radius
    if (direction != "left" && direction != "right")
    {
        result.success = false;
        result.failure_reason = "Invalid direction. Must be 'left' or 'right'.";
        return result;
    }

    if (radius <= 0)
    {
        result.success = false;
        result.failure_reason = "Invalid radius. Must be greater than 0.";
        return result;
    }

    // Validate movement parameters
    if (!params.isValid())
    {
        result.success = false;
        result.failure_reason = "Invalid movement parameters";
        return result;
    }

    if (params.speed <= 0)
    {
        result.success = false;
        result.failure_reason = "Rotation supports only forward movement";
        return result;
    }

    // Calculate steering angles based on turning radius
    // The tighter the turn, the more extreme the steering angle
    // Maximum steering angle would be at minimum radius
    const int MAX_STEERING_ANGLE = 30; // Maximum steering angle from center

    // Calculate a normalized steering factor (0-1) based on radius
    // Smaller radius = tighter turn = larger steering angle
    float steeringFactor = min(1.0f, 100.0f / radius); // 100cm as reference radius
    int steeringAngle = steeringFactor * MAX_STEERING_ANGLE;

    // Apply steering to servos based on direction
    if (direction == "left")
    {
        // Turn left: both wheels point left
        steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE - steeringAngle);
        steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE - steeringAngle);

        // For sharper turns, we can also adjust motor speeds to create differential turning
        if (radius < 50)
        { // For very tight turns
            // When turning left, right wheels go faster than left wheels
            moveMotor(FRONT_LEFT_MOTOR_INDEX, params.speed * 0.7);
            moveMotor(REAR_LEFT_MOTOR_INDEX, params.speed * 0.7);
            moveMotor(FRONT_RIGHT_MOTOR_INDEX, params.speed);
            moveMotor(REAR_RIGHT_MOTOR_INDEX, params.speed);
        }
        else
        {
            // For gentler turns, just use the same speed
            moveAllMotors(params.speed);
        }
    }
    else // direction == "right"
    {
        // Turn right: both wheels point right
        steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE + steeringAngle);
        steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE + steeringAngle);

        // For sharper turns, adjust motor speeds
        if (radius < 50)
        { // For very tight turns
            // When turning right, left wheels go faster than right wheels
            moveMotor(FRONT_LEFT_MOTOR_INDEX, params.speed);
            moveMotor(REAR_LEFT_MOTOR_INDEX, params.speed);
            moveMotor(FRONT_RIGHT_MOTOR_INDEX, params.speed * 0.7);
            moveMotor(REAR_RIGHT_MOTOR_INDEX, params.speed * 0.7);
        }
        else
        {
            // For gentler turns, just use the same speed
            moveAllMotors(params.speed);
        }
    }

    // Calculate the angle turned based on arc length formula
    // arc_length = radius * angle (in radians)
    // angle = arc_length / radius
    float distanceTraveled = params.distance;                       // The arc length
    float angleTurned = (distanceTraveled / radius) * (180.0 / PI); // Convert radians to degrees

    // Wait for the movement to complete
    delay(params.timeMs);

    // Stop all motors
    stopAllMotors();

    // Reset steering to straight position
    steerServo(FRONT_LEFT_SERVO_INDEX, FRONT_LEFT_REF_ANGLE);
    steerServo(FRONT_RIGHT_SERVO_INDEX, FRONT_RIGHT_REF_ANGLE);

    // Prepare success result
    result.success = true;
    String successData = "{";
    successData += "\"angle_turned\":" + String(angleTurned) + ",";
    successData += "\"distance_traveled\":" + String(distanceTraveled) + ",";
    successData += "\"time_taken\":" + String(params.timeMs);
    successData += "}";
    result.success_result = successData;

    return result;
}
