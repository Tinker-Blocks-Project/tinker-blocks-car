#include "include/api.h"
#include "include/movement.h"
#include "include/pen.h"
#include "include/sensor.h"
#include "include/gyro.h"

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

// Legacy handler renamed and updated to use signed angles directly
String handleRotateLegacy(String params)
{
    // Parse turn-specific parameters
    String direction = "left"; // Default direction
    int speed = 100;           // Default speed
    float angleDeg = 0;        // Angle in degrees to turn
    bool absolute = false;     // Flag to indicate absolute rotation

    // TODO: Parse direction, speed, angleDeg, and absolute from params

    // Convert to the new signed angle format
    float signedAngle = (direction == "left") ? abs(angleDeg) : -abs(angleDeg);

    // Validate parameters
    if (speed <= 0 || speed > 255)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result.toJSON();
    }

    if (angleDeg <= 0 && !absolute)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid angle. Must be greater than 0 for relative rotation.";
        return result.toJSON();
    }

    // Use the simplified rotate function
    Result result = rotate(signedAngle, speed, absolute);
    return result.toJSON();
}

// Handle rotation to absolute angle
String handleRotateToAngle(String params)
{
    // Parse parameters
    int speed = 100;       // Default speed
    float targetAngle = 0; // Target angle in degrees (-180 to 180)

    // TODO: Parse speed and targetAngle from params

    if (speed <= 0 || speed > 255)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result.toJSON();
    }

    // Use the simplified rotate function with absolute=true
    Result result = rotate(targetAngle, speed, true);
    return result.toJSON();
}

// Primary rotation function using signed angles
String handleRotate(String params)
{
    // Parse parameters
    int speed = 100;       // Default speed
    float angle = 0;       // Angle in degrees (positive = CCW, negative = CW)
    bool absolute = false; // Default to relative rotation

    // TODO: Parse speed, angle, and absolute from params

    if (speed <= 0 || speed > 255)
    {
        Result result;
        result.success = false;
        result.failure_reason = "Invalid speed. Must be between 1 and 255.";
        return result.toJSON();
    }

    // Use the simplified rotate function
    Result result = rotate(angle, speed, absolute);
    return result.toJSON();
}

// Reset the rotation tracking
String handleResetRotationTracking(String params)
{
    resetRotationTracking();

    Result result;
    result.success = true;
    result.success_result = "Rotation tracking reset";
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

// Get the current absolute angle
String handleGetCurrentAngle(String params)
{
    Result result;
    result.success = true;
    float currentAngle = getYaw(true);
    result.success_result = String(currentAngle);
    return result.toJSON();
}

// Get the accumulated relative rotation angle
String handleGetAccumulatedAngle(String params)
{
    Result result;
    result.success = true;
    float angle = getAccumulatedAngle();
    result.success_result = String(angle);
    return result.toJSON();
}