#include "../include/globals.h"

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