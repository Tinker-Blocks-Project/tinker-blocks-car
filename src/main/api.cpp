#include "include/api.h"
#include "include/movement.h"
#include "include/pen.h"
#include "include/sensor.h"

void setupAPI()
{
    // Implementation will go here - set up ESP32 WiFi, etc.
}

void handleAPIRequests()
{
    // Implementation will go here - process incoming API requests
}

// Helper function to parse movement parameters
MovementParams parseMovementParams(String params)
{
    // Implementation will go here - parse JSON parameters
    // For now returning a default MovementParams
    MovementParams movementParams;

    // TODO: Parse speed, timeMs, distance from params

    return movementParams;
}

String handleMoveForward(String params)
{
    // Parse parameters into a MovementParams object
    MovementParams movementParams = parseMovementParams(params);

    // Ensure speed is positive (forward direction)
    if (movementParams.speed < 0)
    {
        movementParams.speed = -movementParams.speed;
    }

    // Call the unified movement function
    Result result = translate(movementParams);
    return result.toJSON();
}

String handleMoveBackward(String params)
{
    // Parse parameters into a MovementParams object
    MovementParams movementParams = parseMovementParams(params);

    // Ensure speed is negative (backward direction)
    if (movementParams.speed > 0)
    {
        movementParams.speed = -movementParams.speed;
    }

    // Call the unified movement function
    Result result = translate(movementParams);
    return result.toJSON();
}

String handleTurnLeftRight(String params)
{
    // Parse movement parameters
    MovementParams movementParams = parseMovementParams(params);

    // Parse turn-specific parameters
    String direction = "left"; // Default direction
    float radius = 0;

    // TODO: Parse direction and radius from params

    // Validate turn parameters
    if (direction != "left" && direction != "right")
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid direction. Must be 'left' or 'right'.";
        return result.toJSON();
    }

    if (radius <= 0)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid radius. Must be greater than 0.";
        return result.toJSON();
    }

    // Call the unified turn function
    // Result result = turnWithParams(direction, radius, movementParams);
    Result result;
    return result.toJSON();
}

String handlePenUp()
{
    return liftPenUp().toJSON();
}

String handlePenDown()
{
    return putPenDown().toJSON();
}

String handleTogglePen()
{
    return togglePen().toJSON();
}

String handleGetPenState()
{
    Result result;
    result.success = true;
    result.success_result = "\"" + getPenState() + "\"";
    return result.toJSON();
}

String handleGetDistance()
{
    Result result;
    result.success = true;
    result.success_result = String(getDistanceToObstacle());
    return result.toJSON();
}