#pragma once
#include <lvgl.h>
#include <weather_service.h>

void screen_dashboard_init(void);
lv_obj_t * screen_dashboard_get(void);
void dashboard_set_weather(const WeatherData &data);
