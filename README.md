# Tinker Blocks - Car

This repository contains the **Arduino Code** part of the project which controls the **Car** component.

## The Car

A small custom-built wooden car, with the following features:

- 10cm width x 20cm length
- Wheels are powered by DC-motors, 4 wheels and 4 motors
- 2 H-bridges, to control the motors (one per two motors)
- Uses differential steering (tank-style turning) where left and right wheels rotate in opposite directions to turn
- An ultrasonic sensor mounted on the front, for obstacle detection
- Arduino Mega, to control the car
- 3 Lithium-batteries, each is 3.7V for a total of 11.1V to power the motors
- Voltage regulator to step down the voltage to 5V for the Arduino
- Small servo to control the rotation of a Pen, which is used to draw as the car moves
- ESP32, to allow wifi communication with the Arduino (e.g. via api endpoints that the Raspberry Pi will call)
- MPU-6050 Gyroscope and Accelerometer, for precise movement tracking and orientation sensing, especially for accurate turning angles

## The Arduino Code

The Arduino Code is written in C++ and is located in the `main` folder.
The code supports various features exposed via API endpoints.

Any calculation which for example depends on the wheel size or other parameters is done in the code internally, the caller should not have to worry about it.

### Code Structure

The code is organized into a class-based architecture to improve maintainability and separation of concerns:

```
arduino/main/
├── main.ino                - Main Arduino sketch file with setup() and loop()
├── include/                - Header files directory
│   ├── Motor.h             - Motor class declaration
│   ├── MotionController.h  - Motion controller class declaration
│   ├── GyroSensor.h        - Gyroscope sensor class declaration
│   ├── PenController.h     - Pen controller class declaration
│   ├── UltrasonicSensor.h  - Ultrasonic sensor class declaration
│   ├── API.h               - API interface class declaration
│   ├── Result.h            - Result structure declaration
│   └── MovementParams.h    - Movement parameters structure
├── Motor.cpp               - Motor class implementation
├── MotionController.cpp    - Base motion controller implementation (constructor, setup, basic motor control)
├── MotionController_translate.cpp - Translation movement implementation
├── MotionController_rotate.cpp - Rotation movement implementation  
├── GyroSensor.cpp          - Gyroscope sensor implementation
├── PenController.cpp       - Pen controller implementation
├── UltrasonicSensor.cpp    - Ultrasonic sensor implementation
├── API.cpp                 - API interface implementation
├── Result.cpp              - Result structure implementation
└── MovementParams.cpp      - Movement parameters implementation
```

### Class Architecture Overview

- **Motor**: Handles individual motor control with direct hardware interaction
- **MotionController**: Manages all movement operations using the motors 
  - Split into three files:
    - **MotionController.cpp**: Core functionality and motor control
    - **MotionController_translate.cpp**: Linear motion with sophisticated yaw correction
    - **MotionController_rotate.cpp**: PID-based rotation control with direction change tracking
- **GyroSensor**: Handles gyroscope readings and orientation calculations
- **PenController**: Controls the pen servo operations
- **UltrasonicSensor**: Manages distance sensing
- **API**: Processes commands and interfaces with all other components

## Interacting with the API

The car's API accepts commands via the serial port in a specific format: `command:json_payload`. After processing the command, it returns a JSON response.

### Command Format

All commands follow this format:
```
command:{"param1":"value1","param2":value2}
```

Where:
- `command` is the API endpoint (move, rotate, pen, gyro, sensor)
- The JSON payload contains the parameters specific to that command

### Example Commands and Responses

#### Movement Examples

1. Moving forward 20cm at speed 100:
```
move:{"speed":100,"distance":20}
```
Response:
```json
{"success":true,"result":"Moved at speed 100 for 784ms"}
```

2. Moving backward for 1 second at speed 150:
```
move:{"speed":-150,"timeMs":1000}
```
Response:
```json
{"success":true,"result":"Moved at speed -150 for 1000ms"}
```

3. Failed move due to obstacle detection:
```
move:{"speed":100,"distance":30}
```
Response:
```json
{"success":false,"reason":"Obstacle detected at 12.45cm"}
```

#### Rotation Examples

1. Turning left (counterclockwise) 90 degrees:
```
rotate:{"angle":90,"speed":100}
```
Response:
```json
{"success":true,"result":"Rotated 90.00 degrees"}
```

2. Turning right (clockwise) 45 degrees:
```
rotate:{"angle":-45,"speed":80}
```
Response:
```json
{"success":true,"result":"Rotated -45.00 degrees"}
```

3. Rotating to absolute heading (North = 0 degrees):
```
rotate:{"angle":0,"speed":100,"absolute":true}
```
Response:
```json
{"success":true,"result":"Rotated -37.50 degrees"}
```

#### Pen Control Examples

1. Lifting the pen up:
```
pen:{"action":"up"}
```
Response:
```json
{"success":true,"result":"Pen lifted up"}
```

2. Putting the pen down:
```
pen:{"action":"down"}
```
Response:
```json
{"success":true,"result":"Pen put down"}
```

3. Setting a custom pen position:
```
pen:{"action":"position","position":45}
```
Response:
```json
{"success":true,"result":"Pen position set to 45"}
```

#### Sensor Examples

1. Reading the distance to the nearest object:
```
sensor:{"action":"distance"}
```
Response:
```json
{"success":true,"result":"24.37"}
```

2. Checking for obstacles within a threshold:
```
sensor:{"action":"obstacle","threshold":15}
```
Response:
```json
{"success":true,"result":"false"}
```

#### Gyro Examples

1. Calibrating the gyroscope:
```
gyro:{"action":"calibrate"}
```
Response:
```json
{"success":true,"result":"Gyro calibrated"}
```

2. Getting current gyro data:
```
gyro:{"action":"data"}
```
Response:
```json
{"success":true,"result":{"accelX":0.012345,"accelY":-0.987654,"accelZ":9.812345,"gyroX":0.000123,"gyroY":0.000456,"gyroZ":0.000789,"temperature":23.45}}
```

3. Getting the current yaw angle:
```
gyro:{"action":"yaw"}
```
Response:
```json
{"success":true,"result":"127.84"}
```

4. Setting the reference orientation:
```
gyro:{"action":"reference"}
```
Response:
```json
{"success":true,"result":"Reference yaw set"}
```

### API Reference

All commands to the Arduino follow this format: `command:{"param1":"value1","param2":value2}`
Responses are always JSON objects with the following structure:
```json
{
  "success": true|false,
  "failure_reason": "string", // only if success is false
  "success_result": "string or json object" // only if success is true
}
```

#### 1. Movement Commands

Command: `move`

**Parameters:**
- You must provide at least 2 of these 3 parameters:
  - `speed` (int): -255 to 255, negative values for backward movement
  - `distance` (float): Distance in centimeters
  - `timeMs` (unsigned long): Time in milliseconds 
- Optional parameters:
  - `checkUltrasonic` (boolean): Whether to check for obstacles (default: true)
  - `enableYawCorrection` (boolean): Whether to use gyro for straight movement (default: true)

**Parameter Combinations:**
1. `speed` + `distance`: Car will calculate the required time
2. `speed` + `timeMs`: Car will calculate the distance covered
3. `distance` + `timeMs`: Car will calculate the required speed

**Examples:**
```
move:{"speed":100,"distance":20}
move:{"speed":-150,"timeMs":1000}
move:{"distance":30,"timeMs":2000}
move:{"speed":200,"distance":50,"checkUltrasonic":false}
```

**Success Response:**
```json
{
  "success": true,
  "success_result": "{\"distance_traveled\":20.00,\"time_taken\":784,\"final_yaw\":0.32}"
}
```

**Failure Response:**
```json
{
  "success": false,
  "failure_reason": "Obstacle detected at 12.45cm"
}
```

#### 2. Rotation Commands

Command: `rotate`

**Parameters:**
- Required:
  - `angle` (float): Angle in degrees. Positive for left/CCW, negative for right/CW
- Optional:
  - `speed` (int): Rotation speed between 1-255 (default: 100)
  - `absolute` (boolean): If true, rotate to absolute heading; if false, relative rotation (default: false)

**Examples:**
```
rotate:{"angle":90,"speed":100}
rotate:{"angle":-45,"speed":80}
rotate:{"angle":0,"speed":100,"absolute":true}
```

**Success Response:**
```json
{
  "success": true,
  "success_result": "{\"angle_turned\":90.00,\"time_ms\":856,\"direction_changes\":0}"
}
```

**Failure Response:**
```json
{
  "success": false,
  "failure_reason": "Invalid speed. Must be between 1 and 255."
}
```

#### 3. Pen Control Commands

Command: `pen`

**Parameters:**
- Required:
  - `action` (string): One of "up", "down", or "position"
- Required for "position" action:
  - `position` (int): Position value for the servo (typically 0-180)

**Examples:**
```
pen:{"action":"up"}
pen:{"action":"down"}
pen:{"action":"position","position":45}
```

**Success Response:**
```json
{
  "success": true,
  "success_result": "Pen lifted up"
}
```

**Failure Response:**
```json
{
  "success": false,
  "failure_reason": "Missing or invalid 'position' parameter for pen position"
}
```

#### 4. Gyroscope Commands

Command: `gyro`

**Parameters:**
- Required:
  - `action` (string): One of "calibrate", "data", "yaw", or "reference"

**Examples:**
```
gyro:{"action":"calibrate"}
gyro:{"action":"data"}
gyro:{"action":"yaw"}
gyro:{"action":"reference"}
```

**Success Response for "calibrate":**
```json
{
  "success": true,
  "success_result": "Gyro calibrated"
}
```

**Success Response for "data":**
```json
{
  "success": true,
  "success_result": "{\"accelX\":0.012345,\"accelY\":-0.987654,\"accelZ\":9.812345,\"gyroX\":0.000123,\"gyroY\":0.000456,\"gyroZ\":0.000789,\"temperature\":23.45}"
}
```

**Success Response for "yaw":**
```json
{
  "success": true,
  "success_result": "127.84"
}
```

**Success Response for "reference":**
```json
{
  "success": true,
  "success_result": "Reference yaw set"
}
```

**Failure Response:**
```json
{
  "success": false,
  "failure_reason": "Unknown gyro action: invalid_action"
}
```

#### 5. Ultrasonic Sensor Commands

Command: `sensor`

**Parameters:**
- Required:
  - `action` (string): One of "distance" or "obstacle"
- Optional for "obstacle" action:
  - `threshold` (float): Distance threshold in centimeters (default: 10.0)

**Examples:**
```
sensor:{"action":"distance"}
sensor:{"action":"obstacle","threshold":15}
```

**Success Response for "distance":**
```json
{
  "success": true,
  "success_result": "24.37"
}
```

**Success Response for "obstacle":**
```json
{
  "success": true,
  "success_result": "false"
}
```

**Failure Response:**
```json
{
  "success": false,
  "failure_reason": "Unknown sensor action: invalid_action"
}
```

#### Command Validation & Error Handling

- All commands are validated for proper format and parameter values
- If a required parameter is missing or has an invalid value, the command will fail
- In case of JSON parsing errors, you'll receive an error response
- For movement commands, calculations ensure reasonable values
- Obstacle detection during movement will abort the command and return a failure with the obstacle distance

#### Communication Protocol Notes

- Commands must end with a newline character ('\n')
- Serial communication uses 115200 baud rate
- Responses will be sent as a complete JSON string followed by a newline
- All JSON values are properly escaped to ensure valid parsing
- Numeric values in responses have appropriate precision
  - Float values typically have 2 decimal places
  - Integer values are represented without decimal places
