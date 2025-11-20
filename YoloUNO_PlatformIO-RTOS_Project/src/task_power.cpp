#include "task_power.h"

#define USE_LIGHT_SLEEP false // false -> Deep Sleep
#define SLEEP_DURATION_SECONDS  10
#define SLEEP_DURATION_US SLEEP_DURATION_SECONDS*1000000ULL
#define LED_GPIO 48
#define WAKEUP_GPIO 0

RTC_DATA_ATTR int bootCount = 1; // survives Deep Sleep

static void print_wakeup_reason() {
	esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
	switch (cause) {
		case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup: TIMER\n"); break;
		case ESP_SLEEP_WAKEUP_GPIO: Serial.println("Wakeup: GPIO\n"); break;
		case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup: EXT0\n"); break;
		default: Serial.printf("Wakeup: %d\n", cause); break;
	}
}

void power_demo_task(void *pvParameters) {
	(void)pvParameters;
	Serial.println("\n=== Power Demo Task Started ===");

	setCpuFrequencyMhz(20);

	pinMode(LED_GPIO, OUTPUT);

	while (1) {
		Serial.printf("Boot #%d\n", bootCount++);

		digitalWrite(LED_GPIO, HIGH);
		Serial.println("Turning on the LED. Staying awake for 5 seconds.\n");
		vTaskDelay(pdMS_TO_TICKS(5000));
		digitalWrite(LED_GPIO, LOW);

		if (USE_LIGHT_SLEEP) {
			Serial.printf("Done. Light Sleep for %lu ms.\n", SLEEP_DURATION_US);
			Serial.flush();

			WiFi.mode(WIFI_OFF);
			btStop();

			esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

			Serial.println("Going to light sleep soon...");
			vTaskDelay(pdMS_TO_TICKS(2000));
			esp_light_sleep_start();

			Serial.println("Woke up from light sleep!");
			print_wakeup_reason();
			vTaskDelay(pdMS_TO_TICKS(1000));
		}

		else {
			vTaskDelay(pdMS_TO_TICKS(100));
			Serial.printf("Done. Deep Sleep for %lu s.\n", SLEEP_DURATION_SECONDS);
			Serial.flush();

			WiFi.mode(WIFI_OFF);
			btStop();

			esp_sleep_enable_timer_wakeup(SLEEP_DURATION_US);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
			esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

			Serial.println("Going to deep sleep soon...");
			vTaskDelay(pdMS_TO_TICKS(2000));
			esp_deep_sleep_start();
		}
	}
}