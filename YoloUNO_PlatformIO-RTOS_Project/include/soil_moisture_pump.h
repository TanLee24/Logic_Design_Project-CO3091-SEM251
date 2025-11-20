#ifndef __SOIL_MOISTURE_PUMP_H__
#define __SOIL_MOISTURE_PUMP_H__

#include <Arduino.h>
#include "global.h"

// Task to monitor soil moisture and control pump
void soilMoistureAffectsPump(void *pvParameters);

#endif