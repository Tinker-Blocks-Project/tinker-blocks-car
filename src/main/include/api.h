#ifndef API_H
#define API_H

#include "globals.h"
#include "movement.h"

// API initialization
void setupAPI();
void handleAPIRequests();

// Helper function to parse movement parameters
MovementParams parseMovementParams(String params);

// API endpoint handlers
String handleMoveForward(String params);
String handleMoveBackward(String params);
String handleTurnLeftRight(String params);
String handleRotateToAngle(String params);
String handleGetCurrentAngle(String params = "");
String handleGetAccumulatedAngle(String params = "");
String handleResetRotationTracking(String params = "");
String handlePenUp();
String handlePenDown();
String handleTogglePen();
String handleGetPenState();
String handleGetDistance();

#endif // API_H