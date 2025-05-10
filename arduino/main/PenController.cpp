#include "include/PenController.h"

PenController::PenController(int pin, int upPosition, int downPosition)
    : servoPin(pin), penUpPos(upPosition), penDownPos(downPosition), isDown(false)
{
}

void PenController::setup()
{
    penServo.attach(servoPin);
    liftUp(); // Default position on startup
}

void PenController::liftUp()
{
    penServo.write(penUpPos);
    isDown = false;
    delay(200); // Give the servo time to move
}

void PenController::putDown()
{
    penServo.write(penDownPos);
    isDown = true;
    delay(200); // Give the servo time to move
}

bool PenController::isLowered() const
{
    return isDown;
}

void PenController::setPosition(int position)
{
    penServo.write(position);
    isDown = (position == penDownPos);
    delay(200);
}

int PenController::getPosition() const
{
    return isDown ? penDownPos : penUpPos;
}