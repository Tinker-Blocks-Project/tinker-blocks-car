#include "include/gyro.h"
#include "include/movement.h"

// Create MPU6050 object
Adafruit_MPU6050 mpu;

// Store calibration values
GyroData calibration;

// Yaw tracking
float currentYaw = 0.0;
unsigned long lastYawUpdate = 0;
float referenceYaw = 0.0; // Reference "north" yaw

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

    // Set gyro range to +- 250 deg/s (reduced from 500 for slower response)
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);

    // Set filter bandwidth to 44 Hz (increased from 21 Hz for smoother readings)
    mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

    return true;
}

void calibrateGyro()
{
    // Warm up period (reduced from 100 to 20 readings)
    for (int i = 0; i < 20; i++)
    {
        getGyroData();
        delay(5); // Reduced from 10ms
    }

    // Take multiple readings and average them (reduced from 1000 to 100)
    const int numReadings = 100;
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
        delay(5); // Reduced from 20ms
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

float getYaw(bool degrees)
{
    GyroData data = getGyroData();
    unsigned long now = millis();
    float dt = (now - lastYawUpdate) / 1000.0; // Convert to seconds
    lastYawUpdate = now;

    // Integrate gyro Z (yaw) rate to get angle in radians
    currentYaw += data.gyroZ * dt;
    if (degrees)
    {
        return currentYaw * (180.0 / PI);
    }
    else
    {
        return currentYaw;
    }
}

void resetYaw()
{
    currentYaw = 0.0;
    lastYawUpdate = millis();
}

void setReferenceYaw(bool degrees)
{
    referenceYaw = getYaw(degrees);
}

float getReferenceYaw()
{
    return referenceYaw;
}

float getRelativeYaw(bool degrees)
{
    return getYaw(degrees) - referenceYaw;
}

bool establishReferenceYaw(int speed, int durationMs)
{
    // Reset yaw before starting
    resetYaw();

    // Move forward to establish direction
    moveAllMotors(speed);
    delay(durationMs);
    stopAllMotors();

    // Wait for car to stop
    delay(500);

    // Set current yaw as reference
    setReferenceYaw(false);

    return true;
}
