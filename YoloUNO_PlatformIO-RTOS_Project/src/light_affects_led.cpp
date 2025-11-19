#include "light_affects_led.h"

Adafruit_NeoPixel pixels3(4, GPIO_NUM_8, NEO_GRB + NEO_KHZ800);

void lightAffectsLed(void *pvParameters)
{
    pixels3.begin();
    while(1) 
    {                          
        Serial.println("Light Affects LED");
        Serial.println(analogRead(GPIO_NUM_2));
        if(analogRead(GPIO_NUM_2) < 350)
        {
            pixels3.setPixelColor(0, pixels3.Color(255,0,0));
            pixels3.setPixelColor(1, pixels3.Color(255,0,0));
            pixels3.setPixelColor(2, pixels3.Color(255,0,0));
            pixels3.setPixelColor(3, pixels3.Color(255,0,0));
            pixels3.show();
        }
        if(analogRead(GPIO_NUM_2) > 550)
        {
            pixels3.setPixelColor(0, pixels3.Color(0,0,0));
            pixels3.setPixelColor(1, pixels3.Color(0,0,0));
            pixels3.setPixelColor(2, pixels3.Color(0,0,0));
            pixels3.setPixelColor(3, pixels3.Color(0,0,0));
            pixels3.show();
        }
        vTaskDelay(1000);
    }
}