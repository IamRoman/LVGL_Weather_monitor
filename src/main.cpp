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
// UI callback (LVGL thread)
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

	delete res;

	loader_hide();

	weather_running = false;
}

// --------------------------------------------------
// FreeRTOS worker (HTTP thread)
// --------------------------------------------------
void weather_worker(void *pv)
{
	WeatherRequest req;

	while (true)
	{
		if (xQueueReceive(weather_queue, &req, portMAX_DELAY))
		{
			WeatherResult *res = new WeatherResult();

			if (WiFi.status() == WL_CONNECTED)
				res->ok = weather_update_all(weather, req.lat, req.lon);
			else
				res->ok = false;

			lv_async_call(weather_ui_update, res);
		}
	}
}

// --------------------
// Start update
// --------------------
void start_weather_update()
{
	if (weather_running)
	{
		Serial.println("Weather already running");
		return;
	}

	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("WiFi not connected");
		return;
	}

	float lat, lon;

	if (!config_get_location(&lat, &lon))
	{
		Serial.println("Location not found");
		return;
	}

	weather_running = true;

	WeatherRequest req;

	req.lat = lat;
	req.lon = lon;

	loader_show(lv_layer_top());

	xQueueSend(weather_queue, &req, portMAX_DELAY);
}

// --------------------
// Timer callback
// --------------------
static void weather_timer_cb(lv_timer_t *timer)
{
	start_weather_update();

	Serial.print("Free heap: ");
	Serial.println(ESP.getFreeHeap());

	Serial.print("Min free heap: ");
	Serial.println(ESP.getMinFreeHeap());
}

// --------------------
// SETUP
// --------------------
void setup()
{
	Serial.begin(115200);

	config_load(); // load NVS

	lv_init();

	display_init();
	touch_init();

	ui_init();

	// create queue
	weather_queue = xQueueCreate(2, sizeof(WeatherRequest));

	// create worker task
	xTaskCreatePinnedToCore(
			weather_worker,
			"weather_worker",
			12288,
			NULL,
			1,
			NULL,
			1);

	char ssid[33];
	char pass[65];

	if (config_get_wifi(ssid, pass))
	{
		Serial.println("Connecting saved WiFi...");
		wifi_init(ssid, pass);
	}
	else
	{
		Serial.println("No saved WiFi");
	}

	WiFi.onEvent([](arduino_event_id_t event, arduino_event_info_t info)
							 {
    if(event == ARDUINO_EVENT_WIFI_STA_GOT_IP)
    {
			Serial.println("WiFi connected, got IP");
			start_weather_update();
		} });

	weather_time_init();

	// update every 3 minutes
	lv_timer_create(weather_timer_cb, 180000, NULL);

	// first update
	start_weather_update();
}

// --------------------
// LOOP
// --------------------
void loop()
{
	lv_timer_handler();
	lv_tick_inc(5);
	delay(5);
}