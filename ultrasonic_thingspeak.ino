#include <ESP8266WiFi.h>

const char* ssid = "Android1618";
const char* password = "12345678";
const char* server = "api.thingspeak.com";
const String apiKey = "I6A3FUVTDXMZR39P";

const int trigPin = D1;
const int echoPin = D2;

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  long duration, distance_cm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance_cm = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;

    if (client.connect(server, 80)) {
      String url = "/update?api_key=" + apiKey + "&field1=" + String(distance_cm);
      Serial.print("Sending data to ThingSpeak: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Connection: close\r\n\r\n");

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }

      String response = client.readStringUntil('\n');
      if (response.startsWith("HTTP/1.1 200 OK")) {
        Serial.println("Data sent successfully");
        Serial.println(response);
      } else {
        Serial.println("Error sending data");
      }
    }

    client.stop();
  }

  delay(500);
}
