import cv2
from cvzone.FaceDetectionModule import FaceDetector
import numpy as np
from flask import Flask, jsonify, request
import time
import threading

servoPos = [90, 90]  # Servo motor angles [X, Y]
cap = None  # Global variable for VideoCapture
camera_source = 0  # Default camera source
capturing = False  # To track if capturing is ongoing

# Flask API Application
app = Flask(__name__)

@app.route('/get_servo_angles', methods=['GET'])
def get_servo_angles():
    return jsonify({'servo_x': servoPos[0], 'servo_y': servoPos[1]})

@app.route('/start_capturing', methods=['POST'])
def start_capturing():
    global cap, capturing, camera_source

    if capturing:
        return jsonify({'status': 'Camera is already on', 'source': camera_source}), 200

    # Get camera source from request or use default
    data = request.get_json()
    if data and 'source' in data:
        camera_source = data['source']

    cap = cv2.VideoCapture(camera_source)
    if not cap.isOpened():
        return jsonify({'status': 'Failed to open camera', 'source': camera_source}), 400

    capturing = True
    threading.Thread(target=process_camera_feed, daemon=True).start()
    return jsonify({'status': 'Camera started', 'source': camera_source}), 200

@app.route('/stop_capturing', methods=['POST'])
def stop_capturing():
    global capturing, cap

    if not capturing:
        return jsonify({'status': 'Camera is not running'}), 400

    capturing = False
    if cap:
        cap.release()
        cap = None
    return jsonify({'status': 'Camera stopped'}), 200

def process_camera_feed():
    global cap, capturing
    ws, hs = 1280, 720
    detector = FaceDetector()

    cap.set(3, ws)
    cap.set(4, hs)

    while capturing:
        success, img = cap.read()
        if not success:
            time.sleep(0.1)
            continue

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

        if(src == 0)
        cv2.imshow("Image", img)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    if cap:
        cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    # Start Flask server in a separate thread
    threading.Thread(target=lambda: app.run(host='0.0.0.0', port=5000, debug=True, use_reloader=False), daemon=True).start()
    while True:
        time.sleep(1)  # Keep the main thread alive
