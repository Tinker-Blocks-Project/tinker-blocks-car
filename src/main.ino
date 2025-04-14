#include "include/globals.h"
#include "include/movement.h"
#include "include/pen.h"
#include "include/sensor.h"
#include "include/api.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("Tinker Blocks Car - Starting up...");

    setupMovement();

    // Initialize ultrasonic sensor pins
    pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);

    setupPen();
    liftPenUp();

    // Setup API connection
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