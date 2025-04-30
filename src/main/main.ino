#include "include/globals.h"
#include "include/movement.h"
#include "include/pen.h"
#include "include/sensor.h"
#include "include/api.h"
#include "include/gyro.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("Tinker Blocks Car - Starting up...");

    Serial.println("Setting up movement...");
    setupMovement();

    Serial.println("Setting up pen...");
    setupPen();
    liftPenUp();

    Serial.println("Setting up ultrasonic sensor...");
    setupUltrasonic();

    Serial.println("Setting up gyro...");
    setupGyro();

    Serial.println("Calibrating gyro...");
    calibrateGyro();

    Serial.println("Establishing reference yaw...");
    establishReferenceYaw(100, 500); // Move forward at speed 100 for 500ms
    Serial.println("Reference yaw established: " + String(getReferenceYaw(), 2) + "°");

    Serial.println("Setting up API...");
    setupAPI();

    Serial.println("Initialization complete!");
}

void loop()
{
    translate(MovementParams::fromSpeedAndDistance(150, 200));

    delay(5000);
}