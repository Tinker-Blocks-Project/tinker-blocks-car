#include "../include/globals.h"

// Motor and Pin Configuration
const int MOTOR_PINS[] = {22, 23, 24, 25, 30, 31, 32, 33};
const int ENABLE_PINS[] = {4, 6, 3, 8};
const int SERVO_PINS[] = {2, 11};
const int ULTRASONIC_TRIGGER_PIN = 9;
const int ULTRASONIC_ECHO_PIN = 10;
const int PEN_SERVO_PIN = 7;

// Speed and Movement Constants
const int FORWARD_SPEED = 100;
const int BACKWARD_SPEED = 100;
const int STEERING_SPEED = 999; // Should be as high as possible to avoid calculation errors
const int REF_ANGLE1 = 105;     // the angle for the left front wheel to be straight forward
const int REF_ANGLE2 = 90;      // the angle for the right front wheel to be straight forward

// Servo Objects
Servo frontLeftServo;
Servo frontRightServo;
Servo penServo;

// Pen States
const int PEN_UP_ANGLE = 0;
const int PEN_DOWN_ANGLE = 90;
bool isPenDown = false;