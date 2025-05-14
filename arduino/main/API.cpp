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
    // Check for commands on Serial (USB)
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n');
        input.trim();
        Serial.println("Received USB command: " + input);

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

    // Check for commands on Serial1 (ESP32)
    if (Serial1.available() > 0)
    {
        // Read command with a timeout
        String input = "";
        unsigned long startTime = millis();

        // Read until newline or timeout
        while ((millis() - startTime) < 5000)
        {
            if (Serial1.available())
            {
                char c = Serial1.read();
                if (c == '\n')
                {
                    break;
                }
                input += c;
            }
            delay(5);
        }

        input.trim();

        // Debug print what we received
        Serial.println("Received ESP32 command: " + input);

        // Parse command and payload
        int separatorIndex = input.indexOf(':');
        if (separatorIndex == -1)
        {
            String errorMsg = "{\"success\":false,\"reason\":\"Invalid command format\"}";
            Serial1.println(errorMsg);
            Serial.println("Sending error: " + errorMsg);
            return;
        }

        String command = input.substring(0, separatorIndex);
        String payload = input.substring(separatorIndex + 1);

        Serial.println("Command: " + command);
        Serial.println("Payload: " + payload);

        Result result = executeCommand(command, payload);

        // Send response back to ESP32
        String jsonResponse = result.toJSON();
        Serial.println("Sending response: " + jsonResponse);
        Serial1.println(jsonResponse);
        Serial1.flush(); // Ensure all data is sent
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

    // Print incoming payload for debugging
    Serial.println("Move command payload: " + payload);

    // Parse all potential movement parameters
    int speed = 0;
    float distance = 0;
    unsigned long timeMs = 0;
    bool checkUltrasonic = true;
    bool enableYawCorrection = true;

    // Parse all available parameters
    bool hasSpeed = parseJsonInt(payload, "speed", speed);
    bool hasDistance = parseJsonFloat(payload, "distance", distance);
    bool hasTimeMs = parseJsonULong(payload, "timeMs", timeMs);

    // Debug output
    Serial.println("Parsed values:");
    Serial.println("- hasSpeed: " + String(hasSpeed) + ", speed: " + String(speed));
    Serial.println("- hasDistance: " + String(hasDistance) + ", distance: " + String(distance));
    Serial.println("- hasTimeMs: " + String(hasTimeMs) + ", timeMs: " + String(timeMs));

    // Parse optional parameters
    parseJsonBool(payload, "checkUltrasonic", checkUltrasonic);
    parseJsonBool(payload, "enableYawCorrection", enableYawCorrection);

    // We need at least 2 of the 3 parameters (speed, distance, timeMs)
    // Handle all valid combinations

    if (hasSpeed && hasDistance)
    {
        // Case 1: Speed and Distance provided - use them to calculate time
        Serial.println("Using speed and distance to move");
        return motionController.translate(MovementParams::fromSpeedAndDistance(speed, distance), checkUltrasonic, enableYawCorrection);
    }
    else if (hasSpeed && hasTimeMs)
    {
        // Case 2: Speed and Time provided
        Serial.println("Using speed and time to move");
        return motionController.translate(MovementParams::fromSpeedAndTime(speed, timeMs), checkUltrasonic, enableYawCorrection);
    }
    else if (hasDistance && hasTimeMs)
    {
        // Case 3: Distance and Time provided - calculate required speed
        Serial.println("Using distance and time to move");
        return motionController.translate(MovementParams::fromDistanceAndTime(distance, timeMs), checkUltrasonic, enableYawCorrection);
    }
    else
    {
        // Invalid combination
        Serial.println("Invalid parameter combination");
        result.failure_reason = "Invalid parameter combination. Need at least 2 of: 'speed', 'distance', 'timeMs'";
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

    // Also try without quotes (more lenient parsing)
    if (keyIndex == -1)
    {
        keyStr = key + ":";
        keyIndex = json.indexOf(keyStr);
    }

    if (keyIndex == -1)
    {
        Serial.println("Key not found: " + key);
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
        Serial.println("Value end not found for key: " + key);
        return false;
    }

    String valueStr = json.substring(valueStart, valueEnd);
    valueStr.trim();

    // Remove quotes if present
    if (valueStr.startsWith("\"") && valueStr.endsWith("\""))
    {
        valueStr = valueStr.substring(1, valueStr.length() - 1);
    }

    // Check if it's actually a number
    for (unsigned int i = 0; i < valueStr.length(); i++)
    {
        if (i == 0 && valueStr.charAt(i) == '-')
            continue; // Allow negative numbers
        if (!isDigit(valueStr.charAt(i)))
        {
            Serial.println("Not a valid integer: " + valueStr);
            return false;
        }
    }

    value = valueStr.toInt();
    Serial.println("Parsed " + key + " = " + String(value));
    return true;
}

bool API::parseJsonFloat(const String &json, const String &key, float &value)
{
    String keyStr = "\"" + key + "\":";
    int keyIndex = json.indexOf(keyStr);

    // Also try without quotes (more lenient parsing)
    if (keyIndex == -1)
    {
        keyStr = key + ":";
        keyIndex = json.indexOf(keyStr);
    }

    if (keyIndex == -1)
    {
        Serial.println("Key not found: " + key);
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
        Serial.println("Value end not found for key: " + key);
        return false;
    }

    String valueStr = json.substring(valueStart, valueEnd);
    valueStr.trim();

    // Remove quotes if present
    if (valueStr.startsWith("\"") && valueStr.endsWith("\""))
    {
        valueStr = valueStr.substring(1, valueStr.length() - 1);
    }

    value = valueStr.toFloat();
    Serial.println("Parsed " + key + " = " + String(value));
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