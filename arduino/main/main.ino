#include "include/IRSensor.h"
#include "include/Motor.h"
#include "include/MotionController.h"
#include "include/GyroSensor.h"
#include "include/PenController.h"
#include "include/UltrasonicSensor.h"
#include "include/API.h"

// Pin definitions
const int MOTOR_PINS[] = {22, 23, 30, 31, 32, 33, 24, 25};
const int ENABLE_PINS[] = {6, 7, 8, 4};
const int PEN_SERVO_PIN = 10;
const int ULTRASONIC_TRIG_PIN = 40;
const int ULTRASONIC_ECHO_PIN = 41;
const int IR_SENSOR_PIN = 48;


// Create motor instances
Motor frontLeftMotor(MOTOR_PINS[6], MOTOR_PINS[7], ENABLE_PINS[3], true);
Motor frontRightMotor(MOTOR_PINS[0], MOTOR_PINS[1], ENABLE_PINS[0]);
Motor rearLeftMotor(MOTOR_PINS[2], MOTOR_PINS[3], ENABLE_PINS[1]);
Motor rearRightMotor(MOTOR_PINS[4], MOTOR_PINS[5], ENABLE_PINS[2]);

// Create controller instances
MotionController motionController(frontLeftMotor, frontRightMotor, rearLeftMotor, rearRightMotor);
GyroSensor gyroSensor;
PenController penController(PEN_SERVO_PIN);
UltrasonicSensor ultrasonicSensor(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
IRSensor irSensor(IR_SENSOR_PIN);

// Create API instance
API api(motionController, gyroSensor, penController, ultrasonicSensor,irSensor);

void setup()
{
    Serial.begin(115200); // USB Serial Monitor
    Serial.println("Tinker Blocks Car - Starting up...");

    // Use Serial1 for ESP32 comms (pins 18/19 on Mega)
    Serial1.begin(9600);
    delay(1000); // Make sure Serial1 initializes properly
    Serial.println("Serial1 initialized for ESP32 communication");

    // Send a ready message to ESP32
    // Serial1.println("{\"status\":\"ready\"}");
    // Serial1.flush();

    Serial.println("Setting up motors and motion controller...");
    frontLeftMotor.setup();
    frontRightMotor.setup();
    rearLeftMotor.setup();
    rearRightMotor.setup();
    motionController.setup();

    Serial.println("Setting up pen controller...");
    penController.setup();

    Serial.println("Setting up ultrasonic sensor...");
    ultrasonicSensor.setup();

    Serial.println("Setting up ir sensor...");
    irSensor.setup();

    Serial.println("Setting up gyro sensor...");
    if (!gyroSensor.setup())
    {
        Serial.println("Failed to initialize gyro sensor!");
    }

    Serial.println("Calibrating gyro...");
    gyroSensor.calibrate();

    Serial.println("Establishing reference yaw...");
    if (gyroSensor.establishReferenceYaw(100, 200))
    {
        Serial.println("Reference yaw established: " + String(gyroSensor.getReferenceYaw(), 2) + "°");
    }

    Serial.println("Setting up API...");
    api.setup();

    Serial.println("Initialization complete!");
}

void loop()
{
    // Process any incoming API commands
    api.processCommands();

}