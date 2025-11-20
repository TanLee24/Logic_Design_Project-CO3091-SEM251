#include "wifi_monitor.h"

void wifi_monitor_task(void *pvParameters)
{
    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            int rssi = WiFi.RSSI(); // Signal strength in dBm, -30 strong, -80 weak
            xQueueSend(xQueueWifiStrength, &rssi, 0);
            Serial.printf("WiFi RSSI: %d dBm\n", rssi);
        }
        else
        {
            int disconnected = -100; // Value for "disconnected"
            xQueueSend(xQueueWifiStrength, &disconnected, 0);
            Serial.println("WiFi disconnected!");
        }
        vTaskDelay(2000);
    }
}