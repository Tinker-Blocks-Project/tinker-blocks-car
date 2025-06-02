cmake_minimum_required(VERSION 3.29)
project(tinker_blocks_car)

set(CMAKE_CXX_STANDARD 14)

include_directories(arduino/main)
include_directories(arduino/main/include)
include_directories(esp32)
include_directories(esp32/main)

add_executable(tinker_blocks_car
    arduino/main/include/API.h
    arduino/main/include/GyroSensor.h
    arduino/main/include/MotionController.h
    arduino/main/include/Motor.h
    arduino/main/include/MovementParams.h
    arduino/main/include/PenController.h
    arduino/main/include/Result.h
    arduino/main/include/UltrasonicSensor.h
    arduino/main/API.cpp
    arduino/main/GyroSensor.cpp
    arduino/main/main.ino
    arduino/main/MotionController.cpp
    arduino/main/MotionController_rotate.cpp
    arduino/main/MotionController_translate.cpp
    arduino/main/Motor.cpp
    arduino/main/MovementParams.cpp
    arduino/main/PenController.cpp
    arduino/main/Result.cpp
    arduino/main/UltrasonicSensor.cpp
    esp32/main/main.ino
        arduino/main/include/IRSensor.h
        arduino/main/IRSensor.cpp
)
