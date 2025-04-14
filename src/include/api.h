#ifndef API_H
#define API_H

#include "globals.h"

// API initialization
void setupAPI();
void handleAPIRequests();

// Helper function to parse movement parameters
MovementParams parseMovementParams(String params);

// API endpoint handlers
String handleMoveForward(String params);
String handleMoveBackward(String params);
String handleTurnLeftRight(String params);
String handlePenUp();
String handlePenDown();
String handleTogglePen();
String handleGetPenState();
String handleGetDistance();

#endif // API_H