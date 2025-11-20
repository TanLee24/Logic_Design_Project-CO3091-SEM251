#ifndef __LIGHT_AFFECTS_LED_H__
#define __LIGHT_AFFECTS_LED_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
//#include <WiFi.h>

extern Adafruit_NeoPixel pixels3;

// Task to monitor light and affect LED: dark -> turn on LED, bright -> turn off LED
void lightAffectsLed(void *pvParameters);

#endif