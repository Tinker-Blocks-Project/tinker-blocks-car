#include "include/UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(int trig, int echo)
    : trigPin(trig), echoPin(echo), lastDistance(0.0)
{
}

void UltrasonicSensor::setup()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

float UltrasonicSensor::getDistance()
{
    // Clear the trigger pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Set the trigger pin HIGH for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echo pin (time in microseconds)
    long duration = pulseIn(echoPin, HIGH);

    // Calculate distance in cm (speed of sound is 343m/s = 34300cm/s)
    // Distance = (time * speed) / 2 (because sound travels to object and back)
    lastDistance = duration * 0.034 / 2.0;

    return lastDistance;
}

bool UltrasonicSensor::isObstacleDetected(float threshold)
{
    float distance = getDistance();
    return distance < threshold && distance > 0;
}