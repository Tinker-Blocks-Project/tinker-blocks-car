#include "include/movement.h"
#include "include/sensor.h"

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

    // Move all motors in the same direction
    moveAllMotors(params.speed);

    // Wait for the specified time
    delay(params.timeMs);

    // Stop all motors
    stopAllMotors();

    // Prepare success result
    result.success = true;
    String successData = "{";
    successData += "\"distance_traveled\":" + String(params.distance) + ",";
    successData += "\"time_taken\":" + String(params.timeMs);
    successData += "}";
    result.success_result = successData;

    return result;
}