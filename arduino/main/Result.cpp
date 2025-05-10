#include "include/Result.h"

String Result::toJSON()
{
    String json = "{";
    json += "\"success\":" + String(success ? "true" : "false");

    if (!success)
    {
        json += ",\"reason\":\"" + failure_reason + "\"";
    }
    else if (success_result.length() > 0)
    {
        json += ",\"result\":\"" + success_result + "\"";
    }

    json += "}";
    return json;
}