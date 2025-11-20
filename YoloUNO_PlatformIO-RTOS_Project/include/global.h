#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define LED_PIN 48
#define NEO_PIN 45
#define BOOT_PIN 0

extern float glob_temperature;
extern float glob_humidity;

extern String wifi_ssid;
extern String wifi_password;

extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;

extern QueueHandle_t xQueueWifiStrength; // Queue to get the signal strength of wifi
extern QueueHandle_t xQueueSoilMoisture; // Queue to get the value of soil moisture

#endif