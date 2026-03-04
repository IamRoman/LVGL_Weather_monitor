#include <Arduino.h>
#include <lvgl.h>
#include "ui.h"
#include <wifi_service.h>
#include <wifi_widget.h>
#include <screen_home.h>
#include <WiFi.h>
#include <screen_dashboard.h>
#include <config_service.h>

extern void display_init(void);
extern void touch_init(void);

#define LATITUDE 48.213889
#define LONGITUDE 29.284722

WeatherData weather;

static void weather_timer_cb(lv_timer_t *timer)
{
	if (weather_update(weather, LATITUDE, LONGITUDE))
	{
		dashboard_set_weather(weather);
		Serial.println("Weather updated");
	}
	else
	{
		Serial.println("Weather update failed");
	}
}

void setup()
{
	Serial.begin(115200);

	config_load(); // Load App State (NVS 16 КБ)

	lv_init();

	display_init();
	touch_init();

	wifi_init("netis-6A6F79", "qwerty777");
	weather_time_init();
	ui_init();

	if (weather_update(weather, LATITUDE, LONGITUDE))
	{
		Serial.println("Weather updated");
		Serial.println(weather.temperature);
		dashboard_set_weather(weather);
		// WiFi.disconnect(true);
	}
	// Timer every 3 minutes
	lv_timer_create(weather_timer_cb, 180000, NULL);

	Serial.print("Free heap: ");
	Serial.println(ESP.getFreeHeap());

	Serial.print("Min free heap: ");
	Serial.println(ESP.getMinFreeHeap());
}

void loop()
{
	lv_task_handler();
	lv_tick_inc(5);
	delay(5);
}
