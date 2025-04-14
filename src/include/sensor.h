#ifndef SENSOR_H
#define SENSOR_H
#define TRIG_PIN 9
#define ECHO_PIN 10
#include "globals.h"

// Ultrasonic sensor operations
void setupUltrasonic();
float getDistance();
bool isObstacleDetected(float threshold = 20.0); // Default threshold at 20cm

#endif // SENSOR_H