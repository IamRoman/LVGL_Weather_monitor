#include <Arduino.h>
#include <lvgl.h>
#include "ui.h"
#include <wifi_service.h>
#include <wifi_widget.h>
#include <screen_home.h>
#include <WiFi.h>
#include <weather_service.h>

extern void display_init(void);
extern void touch_init(void);

WeatherData weather;

void setup()
{
    Serial.begin(115200);

		lv_init();

		display_init();
		touch_init();

		wifi_init("netis-6A6F79", "qwerty777");
		while (WiFi.status() != WL_CONNECTED)
			delay(100);

		if (weather_update(weather))
		{
			Serial.println("Weather updated");
			Serial.println(weather.temperature);
			// WiFi.disconnect(true);
		}

		ui_init();
}

void loop()
{
	lv_task_handler();
	lv_tick_inc(5);
	delay(5);
}
