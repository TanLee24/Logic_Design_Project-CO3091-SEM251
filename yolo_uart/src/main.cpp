#include <Arduino.h>

void setup()
{
    delay(1000);
    Serial.begin(115200);
    delay(500);
    Serial.println("Booting...");

    // Serial1: RX=44, TX=43
    Serial1.begin(115200, SERIAL_8N1, 43, 44);
}

void loop()
{
    Serial1.println("Hello from ESP32");
    Serial.println("Sent: Hello from ESP32");
    delay(1000);
}
