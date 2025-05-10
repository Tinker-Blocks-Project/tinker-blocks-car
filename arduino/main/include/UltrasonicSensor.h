#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor
{
private:
    int trigPin;
    int echoPin;
    float lastDistance;

public:
    UltrasonicSensor(int trig, int echo);

    void setup();
    float getDistance();                             // Returns distance in centimeters
    bool isObstacleDetected(float threshold = 10.0); // Returns true if obstacle detected within threshold cm
};

#endif // ULTRASONIC_SENSOR_H