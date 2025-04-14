#include "../include/movement.h"

// Create params from speed and distance, calculate time
MovementParams MovementParams::fromSpeedAndDistance(int s, float d)
{
    MovementParams params;
    params.speed = s;
    params.distance = d;

    // Calculate time based on speed and distance
    // Time = distance / speed_cm_per_ms
    float speedPct = abs(s) / 255.0;                // Convert speed to percentage
    float cmPerMs = MAX_SPEED_CM_PER_MS * speedPct; // Adjust for actual speed

    // Avoid division by zero
    if (cmPerMs > 0)
    {
        params.timeMs = static_cast<unsigned long>(d / cmPerMs);
    }
    else
    {
        params.timeMs = 0; // Can't move if speed is 0
    }

    return params;
}

// Create params from speed and time, calculate distance
MovementParams MovementParams::fromSpeedAndTime(int s, unsigned long t)
{
    MovementParams params;
    params.speed = s;
    params.timeMs = t;

    // Calculate distance based on speed and time
    // Distance = speed_cm_per_ms * time
    float speedPct = abs(s) / 255.0;                // Convert speed to percentage
    float cmPerMs = MAX_SPEED_CM_PER_MS * speedPct; // Adjust for actual speed

    params.distance = cmPerMs * t;

    return params;
}

// Create params from distance and time, calculate speed
MovementParams MovementParams::fromDistanceAndTime(float d, unsigned long t)
{
    MovementParams params;
    params.distance = d;
    params.timeMs = t;

    // Calculate speed based on distance and time
    // Speed = distance / time * max_speed / max_distance_per_ms
    if (t > 0)
    {
        float cmPerMs = d / t;
        float speedPct = cmPerMs / MAX_SPEED_CM_PER_MS;

        // Ensure speed is between -1 and 1 before scaling to -255 to 255
        speedPct = min(max(speedPct, 0.0f), 1.0f);

        params.speed = static_cast<int>(speedPct * 255.0);
    }
    else
    {
        params.speed = 0; // Can't move in 0 time
    }

    return params;
}

// Helper method to validate the parameter combination
bool MovementParams::isValid() const
{
    // All three parameters should now be calculated,
    // so the main validation is checking that speed isn't 0
    // and distance and time are positive
    return (speed != 0) && (distance > 0) && (timeMs > 0);
}

String MovementParams::toJSON() const
{
    String json = "{";

    // Always include all parameters since they're all calculated now
    json += "\"speed\":" + String(speed) + ",";
    json += "\"time_ms\":" + String(timeMs) + ",";
    json += "\"distance_cm\":" + String(distance);

    json += "}";
    return json;
}