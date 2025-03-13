#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Bingus Network";
const char* password = "xirl7972";

// Replace with your computer's IP address (not "localhost")
const char* serverName = "http://192.168.56.240:5000/log";

unsigned long previousMillis = 0;
const long interval = 1000;  // 1 second

void setup() {
  Serial.begin(9600);
    Serial.print("Wifi test");
    delay(10000);
      Serial.print("Wifi test");
    delay(10000);
      Serial.print("Wifi test");
    delay(10000);

  WiFi.begin(ssid, password);
  // delay 30 seconds
    delay(10000);

 
  // Wait for connection
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      // Create a JSON payload with a message and a timestamp
      String jsonPayload = "{\"message\": \"Hello from ESP32!\", \"timestamp\": " + String(currentMillis) + "}";

      int httpResponseCode = http.POST(jsonPayload);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
  }
}
