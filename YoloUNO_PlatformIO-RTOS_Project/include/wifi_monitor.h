#ifndef __WIFI_MONITOR_H__
#define __WIFI_MONITOR_H__

#include <WiFi.h>
#include "global.h"

// Task to check the wifi signal, and send the data to task neo_blinky
void wifi_monitor_task(void *pvParameters);

#endif