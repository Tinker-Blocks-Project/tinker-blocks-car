#include "../include/pen.h"

void setupPen()
{
    penServo.attach(PEN_SERVO_PIN);
    penServo.write(PEN_UP_ANGLE); // Start lifted
    isPenDown = false;
}

Result liftPenUp()
{
    Result result;

    if (!isPenDown)
    {
        result.success = false;
        result.failure_reason = "Pen is already up.";
        result.success_result = "";
        return result;
    }

    penServo.write(PEN_UP_ANGLE);
    delay(300);
    isPenDown = false;

    result.success = true;
    result.failure_reason = "";
    result.success_result = "Pen lifted up.";
    return result;
}

Result putPenDown()
{
    Result result;

    if (isPenDown)
    {
        result.success = false;
        result.failure_reason = "Pen is already down.";
        result.success_result = "";
        return result;
    }

    penServo.write(PEN_DOWN_ANGLE);
    delay(300);
    isPenDown = true;

    result.success = true;
    result.failure_reason = "";
    result.success_result = "Pen put down.";
    return result;
}

Result togglePen()
{
    return isPenDown ? liftPenUp() : putPenDown();
}

String getPenState()
{
    return isPenDown ? "down" : "up";
}