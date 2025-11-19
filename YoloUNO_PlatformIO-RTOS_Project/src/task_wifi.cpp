#include "task_wifi.h"

void startAP()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(String(SSID_AP), String(PASS_AP));
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

void startSTA()
{
    if (WIFI_SSID.isEmpty())
    {
        vTaskDelete(NULL);
    }

    WiFi.mode(WIFI_STA);

    if (WIFI_PASS.isEmpty())
    {
        WiFi.begin(WIFI_SSID.c_str());
    }
    else
    {
        WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    //Give a semaphore here
    xSemaphoreGive(xBinarySemaphoreInternet);
}

bool Wifi_reconnect()
{
    const wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED)
    {
        return true;
    }
    startSTA();
    return false;
}

void Wifi_Task(void *pvParameters)
{
    Serial.println("Task Wi-Fi đang khởi động...");

    startSTA();

    Serial.println("Đã kết nối Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Check every 2s
    for(;;)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Wi-Fi mất kết nối, đang reconnect...");
            Wifi_reconnect();
        }
        vTaskDelay(200);
    }
}