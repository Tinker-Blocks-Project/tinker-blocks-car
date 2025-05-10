#ifndef API_H
#define API_H

#include <Arduino.h>
#include "Result.h"
#include "MotionController.h"
#include "GyroSensor.h"
#include "PenController.h"
#include "UltrasonicSensor.h"

class API
{
private:
    MotionController &motionController;
    GyroSensor &gyroSensor;
    PenController &penController;
    UltrasonicSensor &ultrasonicSensor;

    // JSON parsing helpers
    bool parseJsonInt(const String &json, const String &key, int &value);
    bool parseJsonULong(const String &json, const String &key, unsigned long &value);
    bool parseJsonFloat(const String &json, const String &key, float &value);
    bool parseJsonString(const String &json, const String &key, String &value);
    bool parseJsonBool(const String &json, const String &key, bool &value);

public:
    API(
        MotionController &motion,
        GyroSensor &gyro,
        PenController &pen,
        UltrasonicSensor &ultrasonic);

    void setup();

    // Process serial commands
    void processCommands();

    // Command handlers
    Result executeCommand(const String &command, const String &payload);
    Result moveCommand(const String &payload);
    Result rotateCommand(const String &payload);
    Result penCommand(const String &payload);
    Result gyroCommand(const String &payload);
    Result sensorCommand(const String &payload);
};

#endif // API_H