#ifndef GYRO_SENSOR_H
#define GYRO_SENSOR_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

struct GyroData
{
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float temperature;

    String toJSON() const;
    void applyCalibration(const GyroData &cal);
};

class GyroSensor
{
private:
    Adafruit_MPU6050 mpu;
    GyroData calibration;
    float currentYaw;
    unsigned long lastYawUpdate;
    float referenceYaw;

public:
    GyroSensor();

    bool setup();
    void calibrate();
    GyroData getData();

    // Yaw-related methods
    float getYaw(bool degrees = true);
    void resetYaw();
    void setReferenceYaw(bool degrees = true);
    float getReferenceYaw();
    float getRelativeYaw(bool degrees = true);
    bool establishReferenceYaw(int speed, int durationMs);
};

#endif // GYRO_SENSOR_H