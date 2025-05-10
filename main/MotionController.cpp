#include "include/MotionController.h"
#include "include/UltrasonicSensor.h"
#include "include/GyroSensor.h"

// External references to sensors
extern UltrasonicSensor ultrasonicSensor;
extern GyroSensor gyroSensor;

MotionController::MotionController(
    const Motor &frontLeft,
    const Motor &frontRight,
    const Motor &rearLeft,
    const Motor &rearRight)
    : frontLeftMotor(frontLeft),
      frontRightMotor(frontRight),
      rearLeftMotor(rearLeft),
      rearRightMotor(rearRight),
      accumulatedAngle(0.0)
{
}

void MotionController::setup()
{
    // Motors should already be set up individually
    stopAllMotors();
    resetRotationTracking();
}

void MotionController::moveAllMotors(int speed)
{
    frontRightMotor.move(speed);
    rearRightMotor.move(speed);
    delay(5); // to balance the motors, do not remove
    frontLeftMotor.move(speed);
    rearLeftMotor.move(speed);
}

void MotionController::stopAllMotors()
{
    frontLeftMotor.stop();
    frontRightMotor.stop();
    rearLeftMotor.stop();
    rearRightMotor.stop();
}
