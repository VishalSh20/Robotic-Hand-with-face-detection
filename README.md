# Robotic Hand Control with Face Detection

This project implements a robotic hand controlled by servo motors. It uses face detection from an ESP32-CAM module and servo angle adjustments handled by an ESP8266. A Python backend server processes the video feed and provides servo angles for precise movement.

## Overview

The project consists of three major components:
1. **Python Backend**: 
   - Handles image recognition, face detection, and servo angle calculations.
   - Uses Flask to expose REST API endpoints for interaction with ESP devices.
   
2. **ESP8266 Code**: 
   - Fetches servo angles from the Python server and controls the robotic hand based on received data.
   - Communicates via HTTP requests to start the ESP32-CAM stream and retrieve servo angles.

3. **ESP32-CAM Code**:
   - Streams live video to the Python backend server for face detection.
   - Acts as the camera source for image processing.

---

## Project Files

1. **Python Backend**
   - The Python script processes video feed from the ESP32-CAM to detect faces and calculate servo angles for precise hand movements.
   - API Endpoints:
     - **`/start_capturing` (POST)**: Starts **reading** the ESP32-CAM stream.
     - **`/get_servo_angles` (GET)**: Returns the computed servo angles as JSON.
     - **`/stop_capturing` (POST)**: Stops **reading** the ESP32-CAM stream.
   - Dependencies:
     ```bash
     pip install flask opencv-python numpy cvzone threading
     ```

2. **ESP8266 Code**
   - This file handles:
     - Sending a POST request to `/start_capturing` to start the ESP32-CAM stream.
     - Continuously fetching servo angles from `/get_servo_angles` using HTTP GET requests.
     - Updating servo motor positions based on the fetched angles.

3. **ESP32-CAM Code**
   - Streams live video to the backend Python server.
   - No additional interaction with the ESP8266.

---

## Setup Instructions

### Python Server
1. Clone the repository and navigate to the project directory.
2. Install the required Python dependencies:
   ```bash
   pip install flask opencv-python numpy cvzone threading
