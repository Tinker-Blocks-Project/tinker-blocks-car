#include "../include/movement.h"

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

    // Implementation will go here
    // This will handle the movement parameters for turning

    return result;
}
