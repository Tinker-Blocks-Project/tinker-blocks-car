# Tinker Blocks - ESP32 API Bridge

This repository contains the **ESP32 firmware** for the Tinker Blocks Car, responsible for exposing a WiFi API (HTTP/REST) and bridging commands to the Arduino over serial.

## ESP32 Role in the System

- Connects to WiFi and exposes a web API (HTTP endpoints)
- Receives commands from clients (e.g., Raspberry Pi, web apps)
- Translates HTTP requests into serial commands for the Arduino
- Forwards Arduino responses back to the client

## ESP32 Code Structure

Located in `esp32/`:

```
main.cpp, ...         - Main ESP32 application
include/              - Header files
lib/                  - (Optional) Libraries for HTTP, serial, etc.
README.md             - This documentation
```

## HTTP API Reference

The ESP32 exposes a RESTful API for controlling the car. All endpoints accept and return JSON.

### General Notes
- All endpoints: `POST` requests with JSON body
- All responses: JSON objects mirroring the Arduino's response
- The ESP32 handles communication errors and timeouts gracefully

### Endpoints

#### 1. Move the Car
- **POST** `/api/move`
- **Body:**
  - At least 2 of: `speed` (int), `distance` (float), `timeMs` (unsigned long)
  - Optional: `checkUltrasonic` (bool), `enableYawCorrection` (bool)
- **Example:**
```json
{
  "speed": 100,
  "distance": 20
}
```
- **Response:** (mirrors Arduino)
```json
{
  "success": true,
  "success_result": { "distance_traveled": 20.0, "time_taken": 784, "final_yaw": 0.32 }
}
```

#### 2. Rotate the Car
- **POST** `/api/rotate`
- **Body:**
  - `angle` (float, required)
  - Optional: `speed` (int), `absolute` (bool)
- **Example:**
```json
{
  "angle": 90,
  "speed": 100
}
```
- **Response:**
```json
{
  "success": true,
  "success_result": { "angle_turned": 90.0, "time_ms": 856, "direction_changes": 0 }
}
```

#### 3. Pen Control
- **POST** `/api/pen`
- **Body:**
  - `action` ("up", "down", "position")
  - For `position`: `position` (int)
- **Example:**
```json
{
  "action": "up"
}
```
- **Response:**
```json
{
  "success": true,
  "success_result": "Pen lifted up"
}
```

#### 4. Gyroscope
- **POST** `/api/gyro`
- **Body:**
  - `action` ("calibrate", "data", "yaw", "reference")
- **Example:**
```json
{
  "action": "data"
}
```
- **Response:**
```json
{
  "success": true,
  "success_result": { "accelX": 0.012345, "accelY": -0.987654, ... }
}
```

#### 5. Ultrasonic Sensor
- **POST** `/api/sensor`
- **Body:**
  - `action` ("distance", "obstacle")
  - For `obstacle`: `threshold` (float, optional)
- **Example:**
```json
{
  "action": "obstacle",
  "threshold": 15
}
```
- **Response:**
```json
{
  "success": true,
  "success_result": false
}
```

#### 6. IR Sensor
- **POST** `/api/ir`
- **Body:**
  - `action` ("black_obstacle")
- **Example:**
```json
{
  "action": "black_obstacle"
}
```
- **Response:**
```json
{
  "success": true,
  "success_result": false
}
```

### Communication with Arduino

- The ESP32 sends commands to the Arduino over serial using the protocol described in the Arduino README
- Each HTTP request is translated to a serial command, and the Arduino's response is returned to the client
- Serial baud rate: 115200
- Handles timeouts, retries, and error reporting

### Example Workflow
1. Client sends HTTP POST to ESP32 `/api/move` with `{ "speed": 100, "distance": 20 }`
2. ESP32 sends `move:{"speed":100,"distance":20}\n` to Arduino
3. Arduino processes and replies with a JSON response
4. ESP32 forwards the response to the client

---

**For details on the low-level serial protocol and supported commands, see the Arduino documentation.**
