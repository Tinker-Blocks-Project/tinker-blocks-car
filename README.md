# Tinker Blocks - Car

This repository contains the **Arduino Code** part of the project which controls the **Car** component.

## The Car

A small custom-built wooden car, with the following features:

- 12cm width x 24cm length
- Wheels are powered by DC-motors, 4 wheels and 4 motors
- 2 H-bridges, to control the motors (one per two motors)
- The front two wheels are steered with two servos, mounted vertically above each wheel
- An ultrasonic sensor mounted on the front, for obstacle detection
- Arduino Mega, to control the car
- 3 Lithium-batteries, each is 3.7V for a total of 11.1V to power the motors
- Voltage regulator to step down the voltage to 5V for the Arduino
- Small servo to control the rotation of a Pen, which is used to draw as the car moves
- ESP32, to allow wifi communication with the Arduino (e.g. via api endpoints that the Raspberry Pi will call)

## The Arduino Code

The Arduino Code is written in C++ and is located in the `src` folder.
The code should support the following features as api endpoints:

Any calculation which for example depends on the wheel size or other parameters should be done in the code internally, the caller should not have to worry about it.

### Code Structure

The code is organized into several modules to improve maintainability and separation of concerns:

```
src/
├── main.ino          - Main Arduino sketch file with setup() and loop()
├── include/          - Header files directory
│   ├── globals.h     - Global constants, variables, and structures
│   ├── movement.h    - Movement-related function declarations
│   ├── pen.h         - Pen control function declarations
│   ├── sensor.h      - Sensor function declarations
│   └── api.h         - API endpoint declarations
├── core/             - Core functionality
│   └── globals.cpp   - Implementation of global variables
├── movement/         - Movement control
│   └── movement.cpp  - Implementation of movement functions
├── pen/              - Pen control
│   └── pen.cpp       - Implementation of pen functions
├── sensor/           - Sensor functionality
│   └── sensor.cpp    - Implementation of sensor functions
└── api/              - API handling
    └── api.cpp       - Implementation of API endpoints
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

- Operations should return a Result object:
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
    - direction: "left" or "right"
    - radius: turning radius in centimeters
  - Movement parameters: Any valid MovementParams combination
  - Failure:
    - Ultrasonic sensor indicates an obstacle in front of the car
    - Parameters are invalid
  - Success:
    - Total angle turned
    - Distance traveled
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
