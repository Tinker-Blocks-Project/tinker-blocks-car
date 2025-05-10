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
String handleRotate(String params);        // Primary rotation function (signed angles)
String handleRotateToAngle(String params); // Absolute angle rotation
String handleRotateLegacy(String params);  // Legacy handler (for compatibility)
String handleGetCurrentAngle(String params = "");
String handleGetAccumulatedAngle(String params = "");
String handleResetRotationTracking(String params = "");
String handlePenUp();
String handlePenDown();
String handleTogglePen();
String handleGetPenState();
String handleGetDistance();

#endif // API_H