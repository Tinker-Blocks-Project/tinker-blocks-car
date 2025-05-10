#include "include/API.h"

API::API(
    MotionController &motion,
    GyroSensor &gyro,
    PenController &pen,
    UltrasonicSensor &ultrasonic)
    : motionController(motion),
      gyroSensor(gyro),
      penController(pen),
      ultrasonicSensor(ultrasonic)
{
}

void API::setup()
{
    // Nothing special to setup for the API itself currently
}

void API::processCommands()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n');
        input.trim();

        // Parse command and payload
        int separatorIndex = input.indexOf(':');
        if (separatorIndex == -1)
        {
            Serial.println("{\"success\":false,\"reason\":\"Invalid command format\"}");
            return;
        }

        String command = input.substring(0, separatorIndex);
        String payload = input.substring(separatorIndex + 1);

        Result result = executeCommand(command, payload);
        Serial.println(result.toJSON());
    }
}

Result API::executeCommand(const String &command, const String &payload)
{
    Result result;
    result.success = false;

    if (command.equals("move"))
    {
        return moveCommand(payload);
    }
    else if (command.equals("rotate"))
    {
        return rotateCommand(payload);
    }
    else if (command.equals("pen"))
    {
        return penCommand(payload);
    }
    else if (command.equals("gyro"))
    {
        return gyroCommand(payload);
    }
    else if (command.equals("sensor"))
    {
        return sensorCommand(payload);
    }
    else
    {
        result.failure_reason = "Unknown command: " + command;
    }

    return result;
}

Result API::moveCommand(const String &payload)
{
    Result result;
    result.success = false;

    // Parse movement parameters
    int speed = 0;
    float distance = 0;
    unsigned long timeMs = 0;
    bool checkUltrasonic = true;
    bool enableYawCorrection = true;

    if (!parseJsonInt(payload, "speed", speed))
    {
        result.failure_reason = "Missing or invalid 'speed' parameter";
        return result;
    }

    // These are optional parameters
    parseJsonFloat(payload, "distance", distance);
    parseJsonBool(payload, "checkUltrasonic", checkUltrasonic);
    parseJsonBool(payload, "enableYawCorrection", enableYawCorrection);

    // If time is provided and not distance, use time-based movement
    if (parseJsonULong(payload, "timeMs", timeMs) && distance == 0)
    {
        return motionController.translate(MovementParams::fromSpeedAndTime(speed, timeMs), checkUltrasonic, enableYawCorrection);
    }
    // Otherwise use distance-based movement
    else if (distance > 0)
    {
        return motionController.translate(MovementParams::fromSpeedAndDistance(speed, distance), checkUltrasonic, enableYawCorrection);
    }
    else
    {
        result.failure_reason = "Either 'distance' or 'timeMs' must be provided";
        return result;
    }
}

Result API::rotateCommand(const String &payload)
{
    Result result;
    result.success = false;

    float angle = 0;
    int speed = 100; // Default speed
    bool absolute = false;

    if (!parseJsonFloat(payload, "angle", angle))
    {
        result.failure_reason = "Missing or invalid 'angle' parameter";
        return result;
    }

    // These are optional parameters
    parseJsonInt(payload, "speed", speed);
    parseJsonBool(payload, "absolute", absolute);

    return motionController.rotate(angle, speed, absolute);
}

Result API::penCommand(const String &payload)
{
    Result result;
    result.success = false;

    String action;
    if (!parseJsonString(payload, "action", action))
    {
        result.failure_reason = "Missing or invalid 'action' parameter";
        return result;
    }

    if (action.equals("up"))
    {
        penController.liftUp();
        result.success = true;
        result.success_result = "Pen lifted up";
    }
    else if (action.equals("down"))
    {
        penController.putDown();
        result.success = true;
        result.success_result = "Pen put down";
    }
    else if (action.equals("position"))
    {
        int position;
        if (!parseJsonInt(payload, "position", position))
        {
            result.failure_reason = "Missing or invalid 'position' parameter for pen position";
            return result;
        }

        penController.setPosition(position);
        result.success = true;
        result.success_result = "Pen position set to " + String(position);
    }
    else
    {
        result.failure_reason = "Unknown pen action: " + action;
    }

    return result;
}

Result API::gyroCommand(const String &payload)
{
    Result result;
    result.success = false;

    String action;
    if (!parseJsonString(payload, "action", action))
    {
        result.failure_reason = "Missing or invalid 'action' parameter";
        return result;
    }

    if (action.equals("calibrate"))
    {
        gyroSensor.calibrate();
        result.success = true;
        result.success_result = "Gyro calibrated";
    }
    else if (action.equals("data"))
    {
        GyroData data = gyroSensor.getData();
        result.success = true;
        result.success_result = data.toJSON();
    }
    else if (action.equals("yaw"))
    {
        float yaw = gyroSensor.getYaw();
        result.success = true;
        result.success_result = String(yaw, 2);
    }
    else if (action.equals("reference"))
    {
        gyroSensor.setReferenceYaw();
        result.success = true;
        result.success_result = "Reference yaw set";
    }
    else
    {
        result.failure_reason = "Unknown gyro action: " + action;
    }

    return result;
}

Result API::sensorCommand(const String &payload)
{
    Result result;
    result.success = false;

    String action;
    if (!parseJsonString(payload, "action", action))
    {
        result.failure_reason = "Missing or invalid 'action' parameter";
        return result;
    }

    if (action.equals("distance"))
    {
        float distance = ultrasonicSensor.getDistance();
        result.success = true;
        result.success_result = String(distance, 2);
    }
    else if (action.equals("obstacle"))
    {
        float threshold = 10.0; // Default threshold
        parseJsonFloat(payload, "threshold", threshold);

        bool obstacle = ultrasonicSensor.isObstacleDetected(threshold);
        result.success = true;
        result.success_result = obstacle ? "true" : "false";
    }
    else
    {
        result.failure_reason = "Unknown sensor action: " + action;
    }

    return result;
}

// JSON parsing helper methods
bool API::parseJsonInt(const String &json, const String &key, int &value)
{
    String keyStr = "\"" + key + "\":";
    int keyIndex = json.indexOf(keyStr);
    if (keyIndex == -1)
    {
        return false;
    }

    int valueStart = keyIndex + keyStr.length();
    int valueEnd = json.indexOf(",", valueStart);
    if (valueEnd == -1)
    {
        valueEnd = json.indexOf("}", valueStart);
    }

    if (valueEnd == -1)
    {
        return false;
    }

    String valueStr = json.substring(valueStart, valueEnd);
    valueStr.trim();
    value = valueStr.toInt();
    return true;
}

bool API::parseJsonFloat(const String &json, const String &key, float &value)
{
    String keyStr = "\"" + key + "\":";
    int keyIndex = json.indexOf(keyStr);
    if (keyIndex == -1)
    {
        return false;
    }

    int valueStart = keyIndex + keyStr.length();
    int valueEnd = json.indexOf(",", valueStart);
    if (valueEnd == -1)
    {
        valueEnd = json.indexOf("}", valueStart);
    }

    if (valueEnd == -1)
    {
        return false;
    }

    String valueStr = json.substring(valueStart, valueEnd);
    valueStr.trim();
    value = valueStr.toFloat();
    return true;
}

bool API::parseJsonString(const String &json, const String &key, String &value)
{
    String keyStr = "\"" + key + "\":\"";
    int keyIndex = json.indexOf(keyStr);
    if (keyIndex == -1)
    {
        return false;
    }

    int valueStart = keyIndex + keyStr.length();
    int valueEnd = json.indexOf("\"", valueStart);

    if (valueEnd == -1)
    {
        return false;
    }

    value = json.substring(valueStart, valueEnd);
    return true;
}

bool API::parseJsonBool(const String &json, const String &key, bool &value)
{
    String keyStr = "\"" + key + "\":";
    int keyIndex = json.indexOf(keyStr);
    if (keyIndex == -1)
    {
        return false;
    }

    int valueStart = keyIndex + keyStr.length();
    int valueEnd = json.indexOf(",", valueStart);
    if (valueEnd == -1)
    {
        valueEnd = json.indexOf("}", valueStart);
    }

    if (valueEnd == -1)
    {
        return false;
    }

    String valueStr = json.substring(valueStart, valueEnd);
    valueStr.trim();

    if (valueStr.equals("true"))
    {
        value = true;
        return true;
    }
    else if (valueStr.equals("false"))
    {
        value = false;
        return true;
    }

    return false;
}

bool API::parseJsonULong(const String &json, const String &key, unsigned long &value)
{
    String keyStr = "\"" + key + "\":";
    int keyIndex = json.indexOf(keyStr);
    if (keyIndex == -1)
    {
        return false;
    }

    int valueStart = keyIndex + keyStr.length();
    int valueEnd = json.indexOf(",", valueStart);
    if (valueEnd == -1)
    {
        valueEnd = json.indexOf("}", valueStart);
    }

    if (valueEnd == -1)
    {
        return false;
    }

    String valueStr = json.substring(valueStart, valueEnd);
    valueStr.trim();
    value = valueStr.toInt(); // toInt() returns a long, which can be implicitly converted to unsigned long
    return true;
}