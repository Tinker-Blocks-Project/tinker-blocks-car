#ifndef GYRO_H
#define GYRO_H

#include "globals.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Gyro data and calibration structure
struct GyroData
{
    float accelX = 0;
    float accelY = 0;
    float accelZ = 0;
    float gyroX = 0;
    float gyroY = 0;
    float gyroZ = 0;
    float temperature = 0;

    String toJSON() const;
    void applyCalibration(const GyroData &cal);
};

// Initialize the MPU-6050
bool setupGyro();

// Calibrate the gyro (should be called when the car is stationary)
void calibrateGyro();

// Get current gyro readings
GyroData getGyroData();

// Get current yaw angle (optionally in degrees)
float getYaw(bool degrees = false);

// Reset yaw to zero
void resetYaw();

// Set the current yaw as the reference "north"
void setReferenceYaw(bool degrees = false);

// Get the reference "north" yaw
float getReferenceYaw();

// Get yaw relative to reference "north"
float getRelativeYaw(bool degrees = false);

// Establish reference yaw by moving forward
bool establishReferenceYaw(int speed = 100, int durationMs = 500);

#endif // GYRO_H