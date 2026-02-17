#pragma once
#include <Arduino.h>

struct WeatherData
{
    float temperature;
    int humidity;
    int pressure;
    int weather_id;
};

bool weather_update(WeatherData &data);
