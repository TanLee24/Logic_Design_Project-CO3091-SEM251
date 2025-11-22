#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String ssid = "ACLAB";
String password = "ACLAB2023";
String wifi_ssid = "abcde";
String wifi_password = "123456789";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();
SemaphoreHandle_t xSemaphoreCreateMutex = xSemaphoreCreateMutex();

// RSSI (dBm)
QueueHandle_t xQueueWifiStrength = xQueueCreate(5, sizeof(int)); 
// Initialize queue with length 1 (Mailbox style) to store the latest soil moisture value
QueueHandle_t xQueueSoilMoisture = xQueueCreate(1, sizeof(int));