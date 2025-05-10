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

    // Call the unified movement function with default safety parameters
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

    // Call the unified movement function with default safety parameters
    Result result = translate(movementParams);
    return result.toJSON();
}

String handleTurnLeftRight(String params)
{
    // Parse turn-specific parameters
    String direction = "left"; // Default direction
    int speed = 100;           // Default speed
    float angleDeg = 0;        // Angle in degrees to turn

    // TODO: Parse direction, speed, and angleDeg from params

    // Validate turn parameters
    if (direction != "left" && direction != "right")
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid direction. Must be 'left' or 'right'.";
        return result.toJSON();
    }

    if (speed <= 0 || speed > 255)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result.toJSON();
    }

    if (angleDeg <= 0)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid angle. Must be greater than 0.";
        return result.toJSON();
    }

    // Call the rotate function with the new parameters
    Result result = rotate(direction, speed, angleDeg);
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