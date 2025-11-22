#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "mainserver.h"
#include "tinyml.h"
#include "coreiot.h"
#include "light_affects_led.h"
#include "soil_moisture_pump.h"
#include "temp_humi_monitor.h"
#include "wifi_monitor.h"

// include task
#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_wifi.h"
#include "task_webserver.h"
#include "task_core_iot.h"

#include "task_power.h"

void setup()
{
  Serial.begin(115200);
  delay(3000);
  // check_info_File(0);
  
  /*************************************** Tan Le's tasks *************************************/
  xTaskCreate(Wifi_Task, "Task WIFI", 8192, NULL, 3, NULL);
  xTaskCreate(wifi_monitor_task, "WiFiMonitor", 4096, NULL, 2, NULL);
  xTaskCreate(neo_blinky, "Task NEO Blink", 4096, NULL, 2, NULL);
  xTaskCreate(lightAffectsLed, "Light Affects LED", 4096, NULL, 2, NULL);
  xTaskCreate(soilMoistureAffectsPump, "Soil Moisture Affects Pump", 4096, NULL, 2, NULL);
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor", 4096, NULL, 2, NULL);
  /********************************************************************************************/
  
  /*************************************** Tuan Anh's tasks *************************************/
  xTaskCreate(main_server_task, "Main Server", 8192, NULL, 2, NULL);
	xTaskCreate(blink_mode, "Blink Mode", 4096, NULL, 1, NULL);
  // xTaskCreate(power_demo_task, "Power Demo", 4096, NULL, 1, NULL);
  /********************************************************************************************/

  xTaskCreate(led_blinky, "Task LED Blinky", 4096, NULL, 2, NULL);
  // xTaskCreate(tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  // xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
  // xTaskCreate(Task_Toogle_BOOT, "Task_Toogle_BOOT", 4096, NULL, 2, NULL);
}

void loop()
{
  // if (check_info_File(1))
  // {
  //   if (!Wifi_reconnect())
  //   {
  //     Webserver_stop();
  //   }
  //   else
  //   {
  //     //CORE_IOT_reconnect();
  //   }
  // }
  // Webserver_reconnect();
}