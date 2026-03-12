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

QueueHandle_t weather_queue;

static bool weather_running = false;

typedef struct
{
	float lat;
	float lon;
} WeatherRequest;

typedef struct
{
	bool ok;
} WeatherResult;

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

	weather_running = false;
}

// --------------------------------------------------
// FreeRTOS worker (HTTP)
// --------------------------------------------------
void weather_worker(void *pv)
{
	WeatherRequest req;

	while (true)
	{
		if (xQueueReceive(weather_queue, &req, portMAX_DELAY))
		{
			WeatherResult res;

			res.ok = weather_update(weather, req.lat, req.lon);

			lv_async_call(weather_ui_update, &res);
		}
	}
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
	if (weather_running)
	{
		Serial.println("Weather already running");
		return;
	}

	weather_running = true;

	float lat, lon;

	if (!config_get_location(&lat, &lon))
		return;

	WeatherRequest req;

	req.lat = lat;
	req.lon = lon;

	loader_show(lv_layer_top());

	xQueueSend(weather_queue, &req, 0);
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

	weather_queue = xQueueCreate(2, sizeof(WeatherRequest));

	xTaskCreate(
			weather_worker,
			"weather_worker",
			8192,
			NULL,
			1,
			NULL);

	start_weather_update();

	lv_timer_create([](lv_timer_t *t)
									{ start_weather_update(); },
									180000,
									NULL);

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
