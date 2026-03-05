#include <Arduino.h>
#include <lvgl.h>
#include "ui.h"
#include <wifi_service.h>
#include <wifi_widget.h>
#include <screen_home.h>
#include <WiFi.h>
#include <screen_dashboard.h>
#include <config_service.h>
#include "loader.h"

extern void display_init(void);
extern void touch_init(void);

// #define LATITUDE 48.213889 // Verbka
// #define LONGITUDE 29.284722

WeatherData weather;

struct WeatherResult
{
	bool ok;
};

// --------------------
// UI callback
// --------------------
void weather_ui_update(void *data)
{
	WeatherResult *res = (WeatherResult *)data;

	if (res->ok)
	{
		dashboard_set_weather(weather);
		Serial.println("Weather updated");
	}
	else
	{
		Serial.println("Weather update failed");
	}

	loader_hide();

	delete res; // freeing memory
}

// --------------------------------------------------
// FreeRTOS task (HTTP)
// --------------------------------------------------
void weather_task(void *pv)
{
	float lat, lon;

	if (!config_get_location(&lat, &lon))
	{
		vTaskDelete(NULL);
		return;
	}

	WeatherResult *res = new WeatherResult;
	res->ok = weather_update(weather, lat, lon);

	lv_async_call(weather_ui_update, res);

	vTaskDelete(NULL);
}

void update_weather(void)
{
	float lat, lon;

	if (!config_get_location(&lat, &lon))
	{
		Serial.println("Failed to get location");
		return;
	}

	if (weather_update(weather, lat, lon))
	{
		dashboard_set_weather(weather);
		Serial.println("Weather updated");
	}
	else
	{
		Serial.println("Weather update failed");
	}
}

void start_weather_update()
{
	loader_show(lv_layer_top());

	xTaskCreate(
			weather_task,
			"weather_task",
			8192, // stack
			NULL,
			1,
			NULL);
}

static void weather_timer_cb(lv_timer_t *timer)
{
	start_weather_update();
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

	float lat, lon;

	if (!config_get_location(&lat, &lon))
	{
		Serial.println("Failed to get location");
		return;
	}

	if (weather_update(weather, lat, lon))
	{
		Serial.println("Weather updated");
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
	lv_timer_handler();
	lv_tick_inc(5);
	delay(5);
}
