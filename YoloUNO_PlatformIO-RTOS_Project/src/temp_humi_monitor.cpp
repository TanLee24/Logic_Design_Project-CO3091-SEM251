#include "temp_humi_monitor.h"
DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);

void temp_humi_monitor(void *pvParameters)
{
    Wire.begin(11, 12);     // SDA, SCL
    dht20.begin();
    lcd.begin();
    lcd.backlight();

    // Local variable to store the latest soil moisture value received from Queue
    int currentSoilValue = 0; 
    int receivedValue = 0;

    while (1)
    {
        // --- CHECK QUEUE FOR NEW SOIL DATA ---
        // xQueueReceive with 0 tick wait time (non-blocking)
        // If new data is available, update currentSoilValue
        if(xQueueReceive(xQueueSoilMoisture, &receivedValue, 0) == pdTRUE) 
        {
            currentSoilValue = receivedValue;
        }

        // --- READ AIR HUMIDITY AND TEMPERATURE FROM DHT20
        dht20.read();
        // Reading temperature in Celsius
        float temperature = dht20.getTemperature();
        // Reading humidity
        float humidity = dht20.getHumidity();
        // Check if any reads failed and exit early
        if (isnan(temperature) || isnan(humidity))
        {
            Serial.println("Failed to read from DHT20!");
            vTaskDelay(1000);
            continue;
        }
        //Update global variables for temperature and humidity
        glob_temperature = temperature;
        glob_humidity = humidity;
        
        Serial.printf("Temp: %.2f°C | Humi: %.2f%%\n", temperature, humidity);

        // --- DISPLAY PAGE 1 (AIR INFO) ---
        // lcd.clear();
        // lcd.setCursor(0, 0);
        // lcd.printf("Temp: %.1f°C", temperature);
        // lcd.setCursor(0, 1);
        // lcd.printf("Humi: %.1f%%", humidity);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("AIR TEMP & HUMI"); // Title line
        lcd.setCursor(0, 1);
        lcd.printf("T:%.1f°C H:%.1f%%", temperature, humidity); // Data line
        
        vTaskDelay(3000); // Hold page 1 for 3s

        // --- DISPLAY PAGE 2 (SOIL INFO) ---
        // Check queue again to ensure we display the freshest data
        if(xQueueReceive(xQueueSoilMoisture, &receivedValue, 0) == pdTRUE) 
        {
            currentSoilValue = receivedValue;
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SOIL MOISTURE"); // Title line
        lcd.setCursor(0, 1);
        // Display value and status based on threshold
        if(currentSoilValue < 50) 
        {
            lcd.printf("Val:%d (DRY)", currentSoilValue);
        } 
        else if (currentSoilValue > 1500) 
        {
            lcd.printf("Val:%d (WET)", currentSoilValue);
        } 
        else 
        {
            lcd.printf("Val:%d (OK)", currentSoilValue);
        }
        // Hold Page 2 for 3 seconds
        vTaskDelay(3000);
    }
}