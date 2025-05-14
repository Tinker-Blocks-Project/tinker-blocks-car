#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char *ssid = "Mazen";
const char *password = "9001181261977";

// Use WebServer for easier API implementation
WebServer server(80);

void setup()
{
    Serial.begin(115200); // USB Serial Monitor
    Serial1.begin(9600);  // RX=TX pins for Arduino communication (using default pins)

    pinMode(2, OUTPUT); // set the LED pin mode (using pin 2)

    Serial.println("ESP32 ready");
    Serial1.println("ESP32 connected to Arduino"); // Send message to Arduino

    delay(10);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA); // Explicitly set station mode
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Also send WiFi status to Arduino
    Serial1.println("WiFi connected");
    Serial1.print("IP: ");
    Serial1.println(WiFi.localIP().toString());

    // Set up API endpoints
    setupAPIEndpoints();

    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();
}

// Handle API requests and bridge them to Arduino
String sendCommandToArduino(const String &command, const String &payload)
{
    // Clean up the payload - remove whitespace and format it more consistently
    String cleanPayload = payload;
    cleanPayload.trim();

    String fullCommand = command + ":" + cleanPayload;

    // Send command to Arduino and flush to ensure it's fully sent
    Serial.println("Sending to Arduino: " + fullCommand);
    Serial1.println(fullCommand);
    Serial1.flush(); // Make sure all data is sent before continuing

    delay(100); // Short delay to let Arduino process

    // Wait for response (timeout after 15 seconds)
    unsigned long startTime = millis();
    String response = "";

    // Clear any leftover data in the buffer
    while (Serial1.available())
    {
        Serial1.read();
    }

    // Wait for response
    while ((millis() - startTime) < 15000)
    {
        if (Serial1.available())
        {
            char c = Serial1.read();
            if (c == '\n')
            {
                // End of response
                break;
            }
            response += c;
        }
        delay(10);
    }

    response.trim();

    if (response.length() == 0)
    {
        Serial.println("No response from Arduino (timeout)");
        return "{\"success\":false,\"reason\":\"Timeout or no response from Arduino\"}";
    }

    Serial.println("Arduino response: " + response);
    return response;
}

// Generic handler for all API endpoints
void handleAPIRequest()
{
    if (server.method() != HTTP_POST)
    {
        server.send(405, "application/json", "{\"success\":false,\"reason\":\"Method not allowed\"}");
        return;
    }

    // Extract command from URI
    String uri = server.uri();
    String command = uri.substring(5); // Remove "/api/" prefix

    // Get payload
    String payload = server.arg("plain");

    // Send command to Arduino and get response
    String response = sendCommandToArduino(command, payload);

    // Send response back to client
    server.send(200, "application/json", response);
}

// Handle root request
void handleRoot()
{
    server.send(200, "text/plain", "Tinker Blocks ESP32 API Bridge");
}

// Setup API endpoints
void setupAPIEndpoints()
{
    server.on("/", HTTP_GET, handleRoot);

    // Use a single handler for all API endpoints
    server.on("/api/move", HTTP_POST, handleAPIRequest);
    server.on("/api/rotate", HTTP_POST, handleAPIRequest);
    server.on("/api/pen", HTTP_POST, handleAPIRequest);
    server.on("/api/gyro", HTTP_POST, handleAPIRequest);
    server.on("/api/sensor", HTTP_POST, handleAPIRequest);

    // Handle not found
    server.onNotFound([]()
                      { server.send(404, "application/json", "{\"success\":false,\"reason\":\"Endpoint not found\"}"); });
}