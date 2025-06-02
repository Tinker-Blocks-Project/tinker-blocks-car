#include "include/IRSensor.h"

IRSensor::IRSensor(int recv_pin)
    : RECV_PIN(recv_pin)
{
}

void IRSensor::setup()
{
    pinMode(RECV_PIN, INPUT);
}


bool IRSensor::isBlackCircleDetected()
{
    int value = digitalRead(RECV_PIN);

    return (value == HIGH);
}