from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)

# Global list to store received data
received_data = []

@app.route('/')
def index():
    # Render a basic HTML page listing all received JSON data
    return render_template_string('''<!doctype html>
<html>
  <head>
    <title>ESP32 Logs</title>
    <style>
      body { font-family: sans-serif; margin: 2em; }
      ul { list-style-type: none; padding: 0; }
      li { background: #f4f4f4; margin: 0.5em 0; padding: 0.5em; border-radius: 4px; }
    </style>
  </head>
  <body>
    <h1>Received ESP32 JSON Data</h1>
    <ul>
      {% for entry in data %}
      <li>{{ entry }}</li>
      {% endfor %}
    </ul>
  </body>
</html>''', data=received_data)

@app.route('/log', methods=['POST'])
def log():
    data = request.get_json()
    if data:
        # Append received JSON to the list and print to console
        received_data.append(data)
        print("Received data:", data)
        return jsonify({"status": "success", "data": data}), 200
    else:
        return jsonify({"status": "fail", "message": "No JSON received"}), 400

if __name__ == '__main__':
    # Run the server so that it’s reachable on your local network.
    app.run(host='0.0.0.0', port=5000)
