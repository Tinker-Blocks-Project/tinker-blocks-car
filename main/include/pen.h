#ifndef PEN_H
#define PEN_H

#include "globals.h"

void setupPen();
Result liftPenUp();
Result putPenDown();
Result togglePen();
String getPenState();

#endif // PEN_H