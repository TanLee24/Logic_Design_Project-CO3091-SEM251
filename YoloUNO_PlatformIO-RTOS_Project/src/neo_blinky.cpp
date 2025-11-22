#include "neo_blinky.h"

void neo_blinky(void *pvParameters)
{
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();

    int rssi = -100;                      // last known RSSI
    uint32_t currentColor = strip.Color(0, 0, 255);  // default: disconnected (blue)
    bool ledOn = false;                   // blink state: ON/OFF
    const TickType_t timeoutNoData = pdMS_TO_TICKS(5000); // After 5s does not receive RSSI -> gray
    
    TickType_t lastUpdateTick = xTaskGetTickCount();

    while (1)
    {
        // Receive latest RSSI (non-blocking to blink frequently)
        int newRssi;
        if (xQueueReceive(xQueueWifiStrength, &newRssi, 0) == pdTRUE)
        {
            rssi = newRssi;
            lastUpdateTick = xTaskGetTickCount();

            // Display color due to RSSI
            if (rssi == -100)
            {
                // Disconnected
                currentColor = strip.Color(0, 0, 255); // blue
            }
            else if (rssi > -60)
            {
                // Strong signal
                currentColor = strip.Color(0, 255, 0); // green
            }
            else if (rssi > -75)
            {
                // Average signal
                currentColor = strip.Color(255, 255, 0); // yellow
            }
            else
            {
                // Weak signal
                currentColor = strip.Color(255, 0, 0); // red
            }
        }

        // Time out -> gray
        if (xTaskGetTickCount() - lastUpdateTick > timeoutNoData)
        {
            currentColor = strip.Color(128, 128, 128); // gray
        }

        // Blink: toggle ON/OFF per 1s
        if (ledOn)
        {
            strip.setPixelColor(0, currentColor);  // show current color
        }
        else
        {
            strip.setPixelColor(0, 0);             // OFF
        }
        strip.show();

        ledOn = !ledOn;                            // Toggle ON/OFF

        vTaskDelay(1000);
    }
}
