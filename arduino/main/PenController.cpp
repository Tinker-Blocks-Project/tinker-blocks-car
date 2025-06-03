#include "include/PenController.h"

PenController::PenController(int pin, int upPosition, int downPosition)
    : servoPin(pin), penUpPos(upPosition), penDownPos(downPosition), isDown(false) {
}

void PenController::setup() {
    penServo.attach(servoPin);
}

void PenController::liftUp() {
    penServo.attach(servoPin);
    penServo.write(penUpPos);
    delay(200);
    penServo.detach();
    delay(1200);
    isDown = false;
}

void PenController::putDown() {
  penServo.attach(servoPin);
    penServo.write(penDownPos);
    delay(200);
    penServo.detach();
    delay(1200);
    isDown = true;
}

bool PenController::isLowered() const {
    return isDown;
}

void PenController::setPosition(int position) {
    penServo.write(position);
    isDown = (position == penDownPos);
}

int PenController::getPosition() const {
    return isDown ? penDownPos : penUpPos;
}
