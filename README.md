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

#### Movement Parameters

Movement operations use a unified `MovementParams` structure that allows for flexible parameter combinations. You need to specify only two parameters, and the third will be automatically calculated:

```json
{
  "speed": int,         // -255 to 255, negative for backward movement
  "time_ms": int,       // Time in milliseconds
  "distance_cm": float  // Distance in centimeters
}
```

Valid parameter combinations:
- speed & distance
- speed & time
- distance & time

Rules for using movement parameters:
- For backward movement, use negative speed values
- Internal calculations use physics constants that can be calibrated for your specific hardware

#### Return Values

- Operations return a Result object:
```json
{
  "success": true,
  "failure_reason": "string",
  "success_result": {}
}
```

- `success` is true if the operation was successful
- `failure_reason` is a string that explains why the operation failed, if it failed. For example:
  - Ultrasonic sensor indicates an obstacle in front of the car
  - Pen is already down
  - Car is already moving
- `success_result` is the result of the operation if it was successful, defined by the operation. For example:
  - Distance traveled
  - Time taken
  - Pen state

#### Translation Movement

- **Move forward**
  - Parameters: MovementParams with any valid combination of speed, time, and distance
  - Failure:
    - Ultrasonic sensor indicates an obstacle in front of the car
  - Success:
    - Distance traveled
    - Time taken

- **Move backward**
  - Same as move forward, but with negative speed

#### Rotation Movement

- **Turn left or right**
  - Required parameters:
    - angle: turning angle in degrees (positive for left/counterclockwise, negative for right/clockwise)
    - speed: rotation speed (0-255)
    - absolute: optional boolean to indicate absolute vs. relative rotation
  - Failure:
    - Parameters are invalid
  - Success:
    - Total angle turned
    - Time taken

#### Pen Operations

- **Lift the pen up**
  - No parameters
  - Failure:
    - Pen is already up
  - Success:
    - Pen is up

- **Put the pen down**
  - No parameters
  - Failure:
    - Pen is already down
  - Success:
    - Pen is down

- **Toggle the pen**
  - No parameters

- **Get the pen state**
  - No parameters
  - Success: (up or down)

#### Ultrasonic Sensor

- **Get the distance to the nearest object in front of the car**
  - No parameters
  - Success: (distance in cm or 0 if no object is detected)

#### Gyro Operations

- **Initialize the gyro**
  - No parameters
  - Success: Gyro is initialized and ready to use

- **Calibrate the gyro**
  - No parameters
  - Note: Should be called when the car is stationary
  - Success: Calibration values are calculated and stored
  - **Calibration Process:**
    1. Warm-up period: Takes readings with short delays to stabilize the sensor
    2. Calibration phase: Takes multiple readings with delay between each reading
    3. For each axis (X, Y, Z):
      - Calculates the average of all readings
      - Stores these averages as calibration offsets
    4. These offsets are automatically subtracted from all future readings

- **Get gyro data**
  - No parameters
  - Success: Returns current gyro and accelerometer readings
  ```json
  {
    "accelX": float,
    "accelY": float,
    "accelZ": float,
    "gyroX": float,
    "gyroY": float,
    "gyroZ": float,
    "temperature": float
  }
  ```
