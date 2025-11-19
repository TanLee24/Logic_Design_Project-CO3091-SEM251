#include "neo_blinky.h"

void neo_blinky(void *pvParameters)
{
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();

    int rssi;

    while (1)
    {
        // Receive RSSI from queue, wait 5s for max
        if (xQueueReceive(xQueueWifiStrength, &rssi, 5000) == pdTRUE)
        {
            // Process LED colors due to wifi signal strength
            if (rssi == -100)
            {
                // Disconnect
                strip.setPixelColor(0, strip.Color(0, 0, 255)); // blue
            }
            else if (rssi > -60)
            {
                // Strong signal
                strip.setPixelColor(0, strip.Color(0, 255, 0)); // green
            }
            else if (rssi > -75)
            {
                // Average signal
                strip.setPixelColor(0, strip.Color(255, 255, 0)); // yellow
            }
            else
            {
                // Weak signal
                strip.setPixelColor(0, strip.Color(255, 0, 0)); // red
            }

            strip.show();
        }
        else
        {
            // No more new data
            strip.setPixelColor(0, strip.Color(128, 128, 128)); // gray = timeout
            strip.show();
        }
        
        vTaskDelay(1000);
    }
}