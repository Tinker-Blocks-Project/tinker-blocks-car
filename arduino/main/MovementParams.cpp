#include "include/MovementParams.h"

const float MAX_SPEED_CM_PER_MS = 0.072;

MovementParams MovementParams::fromSpeedAndDistance(int s, float d)
{
    MovementParams params;
    params.speed = s;
    params.distance = d;

    // Calculate time needed based on speed and distance
    float speedPercentage = abs(s) / 255.0;
    float cmPerMs = MAX_SPEED_CM_PER_MS * speedPercentage;

    // Avoid division by zero
    if (cmPerMs == 0)
    {
        params.timeMs = 0;
    }
    else
    {
        params.timeMs = (unsigned long)(d / cmPerMs);
    }

    return params;
}

MovementParams MovementParams::fromSpeedAndTime(int s, unsigned long t)
{
    MovementParams params;
    params.speed = s;
    params.timeMs = t;

    // Calculate distance based on speed and time
    float speedPercentage = abs(s) / 255.0;
    float cmPerMs = MAX_SPEED_CM_PER_MS * speedPercentage;
    params.distance = cmPerMs * t;

    return params;
}

MovementParams MovementParams::fromDistanceAndTime(float d, unsigned long t)
{
    MovementParams params;
    params.distance = d;
    params.timeMs = t;

    // Calculate speed based on distance and time
    if (t > 0)
    {
        float cmPerMs = d / t;
        float speedPercentage = cmPerMs / MAX_SPEED_CM_PER_MS;

        // Ensure speed is between -1 and 1 before scaling to 255
        speedPercentage = min(max(speedPercentage, 0.0f), 1.0f);

        params.speed = (int)(255.0 * speedPercentage);
    }
    else
    {
        params.speed = 0; // Can't move if time is 0
    }

    return params;
}

bool MovementParams::isValid() const
{
    return (speed != 0) && (timeMs > 0) && (distance > 0);
}

String MovementParams::toJSON() const
{
    String json = "{";
    json += "\"speed\":" + String(speed) + ",";
    json += "\"time_ms\":" + String(timeMs) + ",";
    json += "\"distance_cm\":" + String(distance, 2);
    json += "}";
    return json;
}