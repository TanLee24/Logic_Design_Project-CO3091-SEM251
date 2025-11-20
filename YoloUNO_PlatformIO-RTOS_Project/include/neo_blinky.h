#ifndef __NEO_BLINKY__
#define __NEO_BLINKY__
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "global.h"

#define NEO_PIN 45
#define LED_COUNT 1 

// Task receives data from wifi_monitor task, the way the neo led blinks varies due to the wifi signal
void neo_blinky(void *pvParameters);

#endif