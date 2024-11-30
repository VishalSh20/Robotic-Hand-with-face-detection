#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Server details
const String serverHost = "http://<your-server-ip>:5000"; // Replace <your-server-ip> with the Flask server IP
const String startCameraEndpoint = "/start_capturing";
const String getServoAnglesEndpoint = "/get_servo_angles";

// Variables
unsigned long previousMillis = 0; // Timer for requests
const long interval = 2000;       // Interval to fetch servo angles (2 seconds)

// Function to start the camera
bool startCamera() {
  HTTPClient http;
  String url = serverHost + startCameraEndpoint;

  http.begin(url);
  int httpCode = http.POST(""); // Empty POST request

  bool success = (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED);
  http.end();
  return success;
}

// Function to get servo angles
bool getServoAngles(int& servoX, int& servoY, String& errorMessage) {
  HTTPClient http;
  String url = serverHost + getServoAnglesEndpoint;

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Parse the response (expected format: {"servo_x":90,"servo_y":45})
    int xIndex = payload.indexOf("\"servo_x\":");
    int yIndex = payload.indexOf("\"servo_y\":");

    if (xIndex >= 0 && yIndex >= 0) {
      servoX = payload.substring(xIndex + 10, payload.indexOf(",", xIndex)).toInt();
      servoY = payload.substring(yIndex + 10, payload.indexOf("}", yIndex)).toInt();
      http.end();
      return true;
    } else {
      errorMessage = "Invalid response format";
    }
  } else {
    errorMessage = "HTTP Error: " + String(httpCode);
  }

  http.end();
  return false;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Start the camera
  if (startCamera()) {
    Serial.println("Camera started successfully");
  } else {
    Serial.println("Failed to start camera");
    while (true); // Halt if camera doesn't start
  }

  // Wait for a few seconds before starting main tasks
  delay(5000);
}

void loop() {
  static int servoX = 0, servoY = 0;
  static String errorMessage;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (getServoAngles(servoX, servoY, errorMessage)) {
      Serial.println("Servo Angles: X = " + String(servoX) + ", Y = " + String(servoY));
    } else {
      Serial.println("Error: " + errorMessage);
    }
  }
}
