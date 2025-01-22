from flask import Flask, request, jsonify, render_template_string
from flask_cors import CORS

import threading
import random
import time
import sys

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

sensor_data = {
    'air_temp': None,
    'air_humid': None,
    'soil_temp': None,
    'soil_moist': None,
    'light': None
}


def generate_data():
    global sensor_data
    while True:
        sensor_data = {
            'air_temp': round(random.uniform(15.0, 30.0), 2),
            'air_humid': round(random.uniform(30.0, 70.0), 2),
            'soil_temp': round(random.uniform(10.0, 25.0), 2),
            'soil_moist': round(random.uniform(20.0, 60.0), 2),
            'light': round(random.uniform(200.0, 800.0), 2)
        }
        print('Generated data:', sensor_data)
        time.sleep(5)

@app.route('/log', methods=['POST'])
def log_data():
    global sensor_data
    sensor_data.update(request.get_json())
    print('Received data:', sensor_data)
    return jsonify({'message': 'Data logged'})

@app.route('/data', methods=['GET'])
def get_data():
    global sensor_data
    return jsonify(sensor_data)


def main():
    if len(sys.argv) != 2 or (sys.argv[1] != "release" and sys.argv[1] != "debug"):
        print("Usage: python server.py <release|debug>")
        return
        
    debug = sys.argv[1] == "debug"

    if debug:
       data_thread = threading.Thread(target=generate_data)
       data_thread.daemon = True
       data_thread.start()

    app.run(host='0.0.0.0', port=5000)

if __name__ == '__main__':
    main()
    