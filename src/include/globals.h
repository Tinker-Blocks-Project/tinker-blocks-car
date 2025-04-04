#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Servo.h>

// Motor and Pin Configuration
extern const int MOTOR_PINS[];
extern const int ENABLE_PINS[];
extern const int SERVO_PINS[];
extern const int ULTRASONIC_TRIGGER_PIN;
extern const int ULTRASONIC_ECHO_PIN;
extern const int PEN_SERVO_PIN;

// Speed and Movement Constants
extern const int FORWARD_SPEED;
extern const int BACKWARD_SPEED;
extern const int STEERING_SPEED;
extern const int REF_ANGLE1; // Left front wheel straight angle
extern const int REF_ANGLE2; // Right front wheel straight angle

// Servo Objects
extern Servo frontLeftServo;
extern Servo frontRightServo;
extern Servo penServo;

// Pen States
extern const int PEN_UP_ANGLE;
extern const int PEN_DOWN_ANGLE;
extern bool isPenDown;

// Result Structure
struct Result
{
    bool success;
    String failure_reason;
    String success_result;

    String toJSON()
    {
        String json = "{\"success\":" + String(success ? "true" : "false");

        if (!success)
        {
            json += ",\"failure_reason\":\"" + failure_reason + "\"";
        }
        else if (success_result.length() > 0)
        {
            json += ",\"success_result\":" + success_result;
        }

        json += "}";
        return json;
    }
};

#endif // GLOBALS_H