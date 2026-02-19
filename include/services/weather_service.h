#pragma once
#include <Arduino.h>
struct WeatherData
{
    float temperature;
    int humidity;
    int pressure;
    int weather_id;
};

void weather_time_init(); // NTP
bool weather_update(WeatherData &data);

