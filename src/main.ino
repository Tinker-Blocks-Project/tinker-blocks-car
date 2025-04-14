#include "include/globals.h"
#include "include/movement.h"
#include "include/pen.h"
#include "include/sensor.h"
#include "include/api.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("Tinker Blocks Car - Starting up...");

    // Initialize pins
    for (int i = 0; i < 8; i++)
    {
        pinMode(MOTOR_PINS[i], OUTPUT);
    }

    for (int i = 0; i < 4; i++)
    {
        pinMode(ENABLE_PINS[i], OUTPUT);
    }

    // Initialize ultrasonic sensor pins
    pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);

    // Attach servos
    frontLeftServo.attach(SERVO_PINS[0]);
    frontRightServo.attach(SERVO_PINS[1]);
    penServo.attach(PEN_SERVO_PIN);

    // Set initial positions
    moveSteeringServo(0, REF_ANGLE1);
    moveSteeringServo(1, REF_ANGLE2);
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