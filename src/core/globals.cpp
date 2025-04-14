#include "../include/globals.h"

const int ULTRASONIC_TRIGGER_PIN = 9;
const int ULTRASONIC_ECHO_PIN = 10;
const int PEN_SERVO_PIN = 12;

// Pen States
const int PEN_UP_ANGLE = 0;
const int PEN_DOWN_ANGLE = 90;
bool isPenDown = false;

// =============== Result Implementation ===============

String Result::toJSON()
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