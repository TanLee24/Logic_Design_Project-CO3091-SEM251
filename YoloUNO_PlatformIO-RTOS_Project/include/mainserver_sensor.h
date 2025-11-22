#ifndef MAINSERVER_SENSOR_H
#define MAINSERVER_SENSOR_H

#include "global.h"  
#include "Wire.h"
#include "DHT20.h"

void enable_I2C(void);

void DHT(void *pvParameter);

#endif