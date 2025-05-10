#ifndef MOVEMENT_PARAMS_H
#define MOVEMENT_PARAMS_H

#include <Arduino.h>

struct MovementParams
{
    // Only set the parameters you want to control
    int speed = -1;           // -255 to 255, negative means backward
    unsigned long timeMs = 0; // Time in milliseconds
    float distance = 0;       // Distance in centimeters

    // Static factory methods declarations
    static MovementParams fromSpeedAndDistance(int s, float d);
    static MovementParams fromSpeedAndTime(int s, unsigned long t);
    static MovementParams fromDistanceAndTime(float d, unsigned long t);

    // Helper method declarations
    bool isValid() const;
    String toJSON() const;
};

#endif // MOVEMENT_PARAMS_H