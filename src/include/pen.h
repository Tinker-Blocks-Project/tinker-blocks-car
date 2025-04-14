#ifndef PEN_H
#define PEN_H

#include "globals.h"
// Define servo and state
Servo penServo;
const int PEN_SERVO_PIN = 7;
bool isPenDown = false;

// Define angles for up and down (you may need to calibrate these)
const int PEN_UP_ANGLE = 90;
const int PEN_DOWN_ANGLE = 45;
// Pen operations
Result liftPenUp();
Result putPenDown();
Result togglePen();
String getPenState();

#endif // PEN_H