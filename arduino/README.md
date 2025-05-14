# Tinker Blocks - Arduino Car Firmware

This repository contains the **Arduino firmware** for the Tinker Blocks Car, responsible for direct hardware control and exposing a serial API for higher-level controllers (such as the ESP32).

## The Car Hardware

- Custom wooden chassis (10cm x 20cm)
- 4 DC motors (differential/tank steering)
- 2 H-bridges (motor drivers)
- Ultrasonic sensor (front obstacle detection)
- Arduino Mega (main controller)
- 3x 3.7V Li-ion batteries (11.1V total)
- Voltage regulator (5V for Arduino)
- Servo for pen control (drawing)
- ESP32 (WiFi bridge, not covered here)
- MPU-6050 Gyroscope/Accelerometer (orientation)

## Arduino Code Structure

Located in `arduino/main/`:

```
main.ino                      - Main Arduino sketch
include/                      - Header files
  Motor.h, MotionController.h, ...
Motor.cpp                     - Motor class
MotionController.cpp          - Core motion controller
MotionController_translate.cpp- Translation logic
MotionController_rotate.cpp   - Rotation logic
GyroSensor.cpp                - Gyro/IMU logic
PenController.cpp             - Pen servo logic
UltrasonicSensor.cpp          - Ultrasonic logic
API.cpp                       - Serial API interface
Result.cpp, MovementParams.cpp- Data structures
```

## Serial API Reference

The Arduino exposes a serial API for movement, rotation, pen, gyro, and sensor commands. All commands are sent as a single line (ending with `\n`), and responses are JSON objects.

### Command Format

```
command:{"param1":"value1","param2":value2}
```
- `command`: One of `move`, `rotate`, `pen`, `gyro`, `sensor`
- JSON payload: Command-specific parameters

### Response Format

```json
{
  "success": true|false,
  "failure_reason": "string", // only if success is false
  "success_result": "string or json object" // only if success is true
}
```

### Supported Commands

#### 1. Movement (`move`)
- At least 2 of 3 required: `speed` (int, -255..255), `distance` (float, cm), `timeMs` (unsigned long, ms)
- Optional: `checkUltrasonic` (bool, default true), `enableYawCorrection` (bool, default true)
- Examples:
  - `move:{"speed":100,"distance":20}`
  - `move:{"speed":-150,"timeMs":1000}`
  - `move:{"distance":30,"timeMs":2000}`
- Success: `{ "distance_traveled":..., "time_taken":..., "final_yaw":... }`
- Failure: `{ "failure_reason": "Obstacle detected at ...cm" }`

#### 2. Rotation (`rotate`)
- Required: `angle` (float, deg)
- Optional: `speed` (int, 1..255, default 100), `absolute` (bool, default false)
- Examples:
  - `rotate:{"angle":90,"speed":100}`
  - `rotate:{"angle":-45,"speed":80}`
- Success: `{ "angle_turned":..., "time_ms":..., "direction_changes":... }`
- Failure: `{ "failure_reason": "Invalid speed..." }`

#### 3. Pen Control (`pen`)
- Required: `action` ("up", "down", "position")
- For `position`: `position` (int, 0..180)
- Examples:
  - `pen:{"action":"up"}`
  - `pen:{"action":"position","position":45}`

#### 4. Gyroscope (`gyro`)
- Required: `action` ("calibrate", "data", "yaw", "reference")
- Examples:
  - `gyro:{"action":"calibrate"}`
  - `gyro:{"action":"data"}`

#### 5. Ultrasonic Sensor (`sensor`)
- Required: `action` ("distance", "obstacle")
- For `obstacle`: `threshold` (float, cm, default 10.0)
- Examples:
  - `sensor:{"action":"distance"}`
  - `sensor:{"action":"obstacle","threshold":15}`

### Protocol Notes
- Baud rate: 115200
- Each command/response is a single line (ends with `\n`)
- All numeric values are returned with appropriate precision
- All errors are reported in `failure_reason`

---

**For details on the WiFi/HTTP API and how the ESP32 bridges to the Arduino, see the ESP32 documentation.**
