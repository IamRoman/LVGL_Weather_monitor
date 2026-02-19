#pragma once
#include <lvgl.h>
#include "weather_service.h"

lv_obj_t * today_weather_create(lv_obj_t * parent);
void today_weather_set_data(lv_obj_t * widget, const WeatherData &data);
