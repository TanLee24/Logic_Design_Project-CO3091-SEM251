#ifndef ___MAIN_SERVER__
#define ___MAIN_SERVER__
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "Adafruit_NeoPixel.h"
#include "global.h"
#include "Wire.h"
#include "DHT20.h"

extern WebServer server;

String mainPage();
String settingsPage();

void startAP();
void setupServer();
void connectToWiFi();

void main_server_task(void *pvParameters);

extern int blinkInterval;
extern bool blinkMode;
void blink_mode(void *pvParameters);

void enable_I2C(void);

void DHT(void *pvParameters);

#endif