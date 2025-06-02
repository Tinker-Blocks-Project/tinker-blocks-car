#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <Arduino.h>

class IRSensor
{
private:
    int RECV_PIN;

public:
    IRSensor(int recv_pin);

    void setup();
    bool isBlackCircleDetected(); // Returns true if obstacle detected within threshold cm
};

#endif // IR_Sensor_H