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

    Serial.println("Setting up API...");
    setupAPI();

    Serial.println("Initialization complete!");
}

void loop()
{
    // translate(MovementParams::fromSpeedAndDistance(100, 5));

    // Get and print gyro data
    GyroData data = getGyroData();
    Serial.println("Gyro Data:");
    Serial.println(data.toJSON());

    delay(500); // Print every second
}