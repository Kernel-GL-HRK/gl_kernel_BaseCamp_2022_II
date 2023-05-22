#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "MY_TEST";
const char* password = "mypass";

const int serverPort = 8080;              

WiFiServer server(serverPort);
WiFiClient client;
bool clientConnected = false; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  server.begin();
  pinMode(14, OUTPUT);
  
}

void loop() {
  if (!clientConnected) {
    client = server.available();
    if (client) {
      clientConnected = true;
      Serial.println("Client connected");
    }
  }

  if (clientConnected) {
    if (client.connected()) {
      if (client.available()) {
        String command = client.readStringUntil('\r');
        Serial.println("Received command: " + command);

        if (command == "ON") {
          digitalWrite(14, HIGH);
          Serial.println("Pin 14 is turned ON");
        } else {
          digitalWrite(14, LOW);
          Serial.println("Pin 14 is turned OFF");
        }

        client.flush();
      }
    } else {
      client.stop();
      clientConnected = false;
      Serial.println("Client disconnected");
    }
  }
}
