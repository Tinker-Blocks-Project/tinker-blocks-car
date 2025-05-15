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
        // Check if the success_result is already a JSON object (starts with '{')
        if (success_result.startsWith("{") && success_result.endsWith("}"))
        {
            // If it's already JSON, include it directly without quotes
            json += ",\"result\":" + success_result;
        }
        else
        {
            // If it's not JSON, treat it as a string value
            json += ",\"result\":\"" + success_result + "\"";
        }
    }

    json += "}";
    return json;
}