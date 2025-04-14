#include "../include/globals.h"


const int ULTRASONIC_TRIGGER_PIN = 9;
const int ULTRASONIC_ECHO_PIN = 10;

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