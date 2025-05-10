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
    establishReferenceYaw(100, 200); // Move forward at speed 100 for 200ms
    Serial.println("Reference yaw established: " + String(getReferenceYaw(), 2) + "°");

    Serial.println("Setting up API...");
    setupAPI();

    Serial.println("Initialization complete!");
}

void loop()
{
    // Example using the new signed angle rotation API

    // Rotate 50 degrees counterclockwise (positive angle = left)
    rotate(50, 100);
    delay(1000);

    // Rotate 50 degrees clockwise (negative angle = right)
    rotate(-50, 100);
    delay(1000);

    // Rotate 100 degrees counterclockwise (positive angle = left)
    rotate(-100, 100);
    delay(1000);

    // Rotate 100 degrees clockwise (negative angle = right)
    rotate(100, 100);
    delay(1000);

    // Example of absolute rotation to 90 degrees (east)
    // rotate(90, 100, true);
    // delay(2000);
}