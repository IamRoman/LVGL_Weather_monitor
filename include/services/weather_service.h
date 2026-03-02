#pragma once
#include <Arduino.h>

#define FORECAST_SIZE 8 // Number of hourly forecasts (e.g. 8 = 24 hours)
#define KP_FORECAST_DAYS 4

#define DAYS_COUNT 7
#define DESC_LEN 32

struct DailyForecast
{
	char date[11]; // YYYY-MM-DD
	float min_temp;
	float max_temp;
	int weather_id;
	char description[DESC_LEN];
	char icon[5];
};
struct WeatherData
{
	float temperature;
	int humidity;
	int pressure;
	int weather_id;
	char description[DESC_LEN];
	char icon[5];

	// --- WIND ---
	float wind_speed; // m/s
	int wind_deg;			// degrees
	char wind_dir[4]; // N, NE, E...

	// Hourly forecast
	float hourly_temp[FORECAST_SIZE]; // Temperature every 3 hours

	// KP 7 days
	float kp_forecast[KP_FORECAST_DAYS];
	int storm_forecast[KP_FORECAST_DAYS];

	// Weather 7 days
	DailyForecast daily[DAYS_COUNT];
	int daily_count;
};

void weather_time_init(); // NTP
bool weather_update(WeatherData &data, double lat, double lon);
