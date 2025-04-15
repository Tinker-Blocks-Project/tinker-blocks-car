#ifndef SENSOR_H
#define SENSOR_H

#include "globals.h"

void setupUltrasonic();
float getDistanceToObstacle();
bool isObstacleDetected(float threshold = 20.0); // Default threshold at 20cm

#endif // SENSOR_H