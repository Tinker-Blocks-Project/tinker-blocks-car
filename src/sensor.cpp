#include "include/sensor.h"

const int TRIGGER_PIN = 9;
const int ECHO_PIN = 10;

void setupUltrasonic()
{
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

// Returns distance in centimeters
float getDistance()
{
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    // Read echo time
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout at 30ms (~500cm)

    // Convert to distance in cm
    float distanceCm = duration * 0.0343 / 2.0;

    // If no valid reading, return a high value
    if (duration == 0)
    {
        return 500.0;
    }

    return distanceCm;
}

// Checks if an obstacle is closer than a given threshold (in cm)
bool isObstacleDetected(float threshold)
{
    float distance = getDistance();
    return distance <= threshold;
}
