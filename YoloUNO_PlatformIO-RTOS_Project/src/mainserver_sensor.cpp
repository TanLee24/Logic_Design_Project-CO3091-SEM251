#include "mainserver_sensor.h"
#include "global.h"
DHT20_Data dht_data = {0.0, 0.0};
SemaphoreHandle_t dht_mutex = xSemaphoreCreateMutex();
static QueueHandle_t tQueue = NULL;

void enable_I2C(void){
    Wire.setPins(GPIO_NUM_11, GPIO_NUM_12);
    Wire.begin();
}

void DHT(void *pvParameter) {
    enable_I2C();
    DHT20 DHT;
    memset(&dht_data, 0, sizeof(dht_data));

    while(!DHT.begin()){
        Serial.println("[DHT] Trying to connect");
        vTaskDelay(1000);
    }

    while (true) {
        DHT.read();
        float temp = DHT.getTemperature();
        float humi = DHT.getHumidity();

        if(xSemaphoreTake(dht_mutex, 0)){
            dht_data.temp = temp;
            dht_data.humi = humi;
            xSemaphoreGive(dht_mutex);
        }
        vTaskDelay(1000);
    }
}