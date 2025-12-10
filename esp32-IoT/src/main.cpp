
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>

// ==== WIFI ====
const char* ssid = "Galaxy A32C7A5";
const char* password = "mhpn1603";

// ==== COREIOT ====
// 👉 Token của bạn
const char* mqttServer = "app.coreiot.io";
const int mqttPort = 1883;
const char* accessToken = "aot730wjyq53d121y85t";

WiFiClient espClient;
PubSubClient client(espClient);

// ==== DHT20 (AHT20) ====
Adafruit_AHTX0 aht;

// Hàm reconnect MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Reconnecting to CoreIoT MQTT...");

    // Tạo Client ID duy nhất
    String clientId = "ESP32S3-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientId.c_str(), accessToken, accessToken)) {
      Serial.println("Connected to CoreIoT!");
    } else {
      Serial.print("Failed, state = ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(11, 12);

  // ===== WiFi =====
  Serial.print("Connecting WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // ===== MQTT =====
  client.setServer(mqttServer, mqttPort);
  reconnectMQTT();

  // ===== DHT20 =====
  if (!aht.begin()) {
    Serial.println("DHT20 init FAILED! Check sensor or wiring.");
    while (1) delay(1000);
  }
  Serial.println("DHT20 initialized successfully!");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Đọc cảm biến
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  float t = temp.temperature;
  float h = humidity.relative_humidity;

  Serial.printf("Temperature: %.2f°C | Humidity: %.2f%%\n", t, h);

  // Tạo JSON gửi lên server
  char payload[100];
  snprintf(payload, sizeof(payload),
           "{\"temperature\":%.2f, \"humidity\":%.2f}",
           t, h);

  // Gửi MQTT lên CoreIoT
  if (client.publish("v1/devices/me/telemetry", payload)) {
    Serial.print("Published: ");
    Serial.println(payload);
  } else {
    Serial.println("Publish failed!");
  }

  delay(5000);  // mỗi 5s gửi một lần
}

