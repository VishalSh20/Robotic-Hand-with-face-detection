import cv2
from cvzone.FaceDetectionModule import FaceDetector
import numpy as np
from flask import Flask, jsonify, request
import threading
import time

servoPos = [90, 90]  # Servo motor angles [X, Y]
detector = FaceDetector()  # FaceDetector instance
image_path = 'image.jpg'
processing_active = True

# Flask API Application
app = Flask(__name__)

def continuous_image_processing():
    global servoPos, processing_active
    
    while processing_active:
        img = cv2.imread(image_path)
        
        if img is None:
            time.sleep(0.1)  # Short delay if image not found
            continue
            
        ws, hs = img.shape[1], img.shape[0]
        
        # Process the image for face detection
        img, bboxs = detector.findFaces(img, draw=False)
        if bboxs:
            fx, fy = bboxs[0]["center"]
            servoX = np.interp(fx, [0, ws], [0, 180])
            servoY = np.interp(fy, [0, hs], [0, 180])
            
            servoX = max(0, min(servoX, 180))
            servoY = max(0, min(servoY, 180))
            
            servoPos[0] = int(servoX)
            servoPos[1] = int(servoY)
            
            cv2.circle(img, (fx, fy), 80, (200, 230, 0), 2)
            cv2.putText(img, f'({fx}, {fy})', (fx + 15, fy - 15), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 0), 2)
        
        cv2.putText(img, f'Servo X: {int(servoPos[0])} deg', (50, 50), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 0), 2)
        cv2.putText(img, f'Servo Y: {int(servoPos[1])} deg', (50, 100), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 0), 2)
        
        # Display the processed image
        cv2.imshow("Processed Image", img)
        if cv2.waitKey(1) & 0xFF == ord('q'):  # Press 'q' to quit
            break
            
        time.sleep(0.03)  # Add small delay to prevent high CPU usage

@app.route('/get_servo_angles', methods=['GET'])
def get_servo_angles():
    servo_y1 = min(servoPos[1],90) 
    servo_y2 = max(0,servoPos[1]-90)
    return jsonify({'servo_x': servoPos[0]-90, 'servo_y1':servo_y1,'servo_y2':servo_y2})

@app.route('/process_image', methods=['POST'])
def process_image():
    return jsonify({'status': 'Image processing is running', 'servo_x': servoPos[0], 'servo_y': servoPos[1]}), 200

if __name__ == '__main__':
    # Start image processing thread
    processing_thread = threading.Thread(target=continuous_image_processing, daemon=True)
    processing_thread.start()
    
    # Start Flask server
    app.run(host='0.0.0.0', port=5000, debug=False)
