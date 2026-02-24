#pragma once
#include <Arduino.h>

#define FORECAST_SIZE 8 // Кількість погодинних прогнозів (наприклад 8 = 24 години)
#define KP_FORECAST_DAYS 4
struct WeatherData
{
	float temperature;
	int humidity;
	int pressure;
	int weather_id;

	// Погодинний прогноз
	float hourly_temp[FORECAST_SIZE]; // Температура кожні 3 години

	// поточний
	float kp_index;
	int storm_level;

	// 7 днів
	float kp_forecast[KP_FORECAST_DAYS];
	int storm_forecast[KP_FORECAST_DAYS];
};

void weather_time_init(); // NTP
bool weather_update(WeatherData &data);
