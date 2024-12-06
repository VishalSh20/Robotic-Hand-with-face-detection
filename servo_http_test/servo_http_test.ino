#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>

const char* ssid = "Rich Boys";
const char* password = "1235467890";
const char* serverURL = "http://192.168.29.70:5000/get_servo_angles";

Servo servoX,servoY1,servoY2;
const int servoXPin = D5;
const int servoY1Pin = D6;
const int servoY2Pin = D7;

// Global variables for servo positions
int servo_x = 0; // Default position for X servo
int servo_y1 = 0; // Default position for Y1 servo
int servo_y2 = 0; // Default position for Y2 servo

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");

  // Attach servos to pins
  servoX.attach(servoXPin);
  servoY1.attach(servoY1Pin);
  servoY2.attach(servoY2Pin);
  // Initialize servos to default positions
  servoX.write(servo_x);
  servoY1.write(servo_y1);
  servoY2.write(servo_y2);
}

void loop() {
  updateServoAngles(); // Call the function to update servo angles
  delay(500);         // Wait .5 seconds before the next request
}

// Function to make HTTP request and update servo angles

  void updateServoAngles() {
  if (WiFi.status() == WL_CONNECTED) { // Check Wi-Fi connection
    WiFiClient client; // Create a WiFiClient object
    HTTPClient http;

    // Use the new API with WiFiClient
    if (http.begin(client, serverURL)) {
      int httpResponseCode = http.GET();

      if (httpResponseCode == 200) { // If the response is OK
        String payload = http.getString();
        Serial.println("Response received: " + payload);

        // Parse JSON response
        int new_servo_x = parseServoValue(payload, "servo_x");
        int new_servo_y1 = parseServoValue(payload, "servo_y1");
        int new_servo_y2 = parseServoValue(payload, "servo_y2");
        

        if (new_servo_x != -1 && new_servo_y1 != -1 && new_servo_y2 != -1) { // Update global variables if parsing was successful
          servo_x = new_servo_x;
          servo_y1 = new_servo_y1;
          servo_y2 = new_servo_y2;

          // Move servos to new positions
          servoX.write(servo_x);
          servoY1.write(servo_y1);
          servoY2.write(servo_y2);

          Serial.printf("Servo positions updated: X=%d, Y1=%d\n, Y2=%d\n", servo_x, servo_y1,servo_y2);
        }
      } else {
        Serial.printf("HTTP request failed, error code: %d\n", httpResponseCode);
      }

      http.end(); // Close the HTTP connection
    } else {
      Serial.println("Failed to connect to server.");
    }
  } else {
    Serial.println("Wi-Fi not connected!");
  }
}

// Function to parse servo values from JSON response
int parseServoValue(const String& json, const String& key) {
  int keyIndex = json.indexOf(key);
  if (keyIndex == -1) return -1; // Key not found

  int colonIndex = json.indexOf(":", keyIndex);
  int commaIndex = json.indexOf(",", colonIndex);
  if (commaIndex == -1) commaIndex = json.indexOf("}", colonIndex); // Handle last key in JSON

  if (colonIndex != -1 && commaIndex != -1) {
    String value = json.substring(colonIndex + 1, commaIndex);
    return value.toInt();
  }

  return -1; // Parsing failed
}
