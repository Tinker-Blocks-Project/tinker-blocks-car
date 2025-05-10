#include "include/GyroSensor.h"
#include "include/MotionController.h"

// External reference to motion controller
extern MotionController motionController;

GyroSensor::GyroSensor()
    : currentYaw(0.0), lastYawUpdate(0), referenceYaw(0.0)
{
}

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

bool GyroSensor::setup()
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

void GyroSensor::calibrate()
{
    // Warm up period (reduced from 100 to 20 readings)
    for (int i = 0; i < 20; i++)
    {
        getData();
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
        GyroData data = getData();
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

    // Reset yaw tracking
    resetYaw();

    // Reset the reference north to the current orientation
    referenceYaw = 0.0;

    Serial.println("Gyro calibrated, reference north set to current orientation");
}

GyroData GyroSensor::getData()
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

// Get absolute yaw angle (from calibrated "north" reference)
float GyroSensor::getYaw(bool degrees)
{
    GyroData data = getData();
    unsigned long now = millis();
    float dt = (now - lastYawUpdate) / 1000.0; // Convert to seconds
    lastYawUpdate = now;

    // Integrate gyro Z (yaw) rate to get angle in radians
    currentYaw += data.gyroZ * dt;

    float yawValue = currentYaw;

    // Convert to degrees if needed
    if (degrees)
    {
        yawValue = yawValue * (180.0 / PI);
    }

    // Normalize the returned angle to be between -180 and 180 degrees
    if (degrees)
    {
        while (yawValue > 180.0f)
            yawValue -= 360.0f;
        while (yawValue < -180.0f)
            yawValue += 360.0f;
    }

    return yawValue;
}

// Reset the integrated yaw value to zero
void GyroSensor::resetYaw()
{
    currentYaw = 0.0;
    lastYawUpdate = millis();
    Serial.println("Yaw reset to 0");
}

// Set the current direction as the reference "north" (0 degrees)
void GyroSensor::setReferenceYaw(bool degrees)
{
    referenceYaw = getYaw(degrees);
    Serial.print("Reference yaw set to: ");
    Serial.println(referenceYaw);
}

// Get the previously stored reference yaw
float GyroSensor::getReferenceYaw()
{
    return referenceYaw;
}

// Get yaw relative to the stored reference yaw (useful for relative movements)
float GyroSensor::getRelativeYaw(bool degrees)
{
    float yaw = getYaw(degrees) - referenceYaw;

    // Normalize the result if working in degrees
    if (degrees)
    {
        while (yaw > 180.0f)
            yaw -= 360.0f;
        while (yaw < -180.0f)
            yaw += 360.0f;
    }

    return yaw;
}

// Automatically establish a reference direction by moving forward briefly
bool GyroSensor::establishReferenceYaw(int speed, int durationMs)
{
    // Reset yaw tracking
    resetYaw();

    // Move forward to establish direction
    motionController.moveAllMotors(speed);
    delay(durationMs);
    motionController.stopAllMotors();

    // Wait for the car to settle
    delay(200);

    // Set the reference yaw to the current yaw
    setReferenceYaw();

    return true;
}