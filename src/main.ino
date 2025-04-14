#include "include/globals.h"
#include "include/movement.h"
#include "include/pen.h"
#include "include/sensor.h"
#include "include/api.h"

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

    Serial.println("Setting up API...");
    setupAPI();

    Serial.println("Initialization complete!");
}

void loop()
{
    // Handle API requests
    handleAPIRequests();

    // Add any other continuous operations here

    // Small delay to prevent overwhelming the processor
    delay(10);
}