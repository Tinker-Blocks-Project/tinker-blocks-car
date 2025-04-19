#include "include/gyro.h"

// Create MPU6050 object
Adafruit_MPU6050 mpu;

// Store calibration values
GyroData calibration;

// GyroData method implementations
String GyroData::toJSON() const
{
    String json = "{";
    json += "\"accelX\":" + String(accelX, 6) + ",";
    json += "\"accelY\":" + String(accelY, 6) + ",";
    json += "\"accelZ\":" + String(accelZ, 6) + ",";
    json += "\"gyroX\":" + String(gyroX, 6) + ",";
    json += "\"gyroY\":" + String(gyroY, 6) + ",";
    json += "\"gyroZ\":" + String(gyroZ, 6) + ",";
    json += "\"temperature\":" + String(temperature, 2);
    json += "}";
    return json;
}

void GyroData::applyCalibration(const GyroData &cal)
{
    accelX -= cal.accelX;
    accelY -= cal.accelY;
    accelZ -= cal.accelZ;
    gyroX -= cal.gyroX;
    gyroY -= cal.gyroY;
    gyroZ -= cal.gyroZ;
}

bool setupGyro()
{
    if (!mpu.begin())
    {
        return false;
    }

    // Set accelerometer range to +-8G
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

    // Set gyro range to +- 500 deg/s
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);

    // Set filter bandwidth to 21 Hz
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    return true;
}

void calibrateGyro()
{
    // Warm up period
    for (int i = 0; i < 50; i++)
    {
        getGyroData();
        delay(10);
    }

    // Take multiple readings and average them
    const int numReadings = 1000;
    GyroData sum;
    sum.accelX = 0;
    sum.accelY = 0;
    sum.accelZ = 0;
    sum.gyroX = 0;
    sum.gyroY = 0;
    sum.gyroZ = 0;
    sum.temperature = 0;

    for (int i = 0; i < numReadings; i++)
    {
        GyroData data = getGyroData();
        sum.accelX += data.accelX;
        sum.accelY += data.accelY;
        sum.accelZ += data.accelZ;
        sum.gyroX += data.gyroX;
        sum.gyroY += data.gyroY;
        sum.gyroZ += data.gyroZ;
        delay(5);
    }

    // Calculate and store calibration values
    calibration.accelX = sum.accelX / numReadings;
    calibration.accelY = sum.accelY / numReadings;
    calibration.accelZ = sum.accelZ / numReadings;
    calibration.gyroX = sum.gyroX / numReadings;
    calibration.gyroY = sum.gyroY / numReadings;
    calibration.gyroZ = sum.gyroZ / numReadings;
}

GyroData getGyroData()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    GyroData data;
    data.accelX = a.acceleration.x;
    data.accelY = a.acceleration.y;
    data.accelZ = a.acceleration.z;
    data.gyroX = g.gyro.x;
    data.gyroY = g.gyro.y;
    data.gyroZ = g.gyro.z;
    data.temperature = temp.temperature;

    // Apply calibration
    data.applyCalibration(calibration);

    return data;
}
