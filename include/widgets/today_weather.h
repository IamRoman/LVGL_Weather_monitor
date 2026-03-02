#pragma once
#include <lvgl.h>
#include "weather_service.h"

typedef struct
{
  lv_obj_t *label_temp;
  lv_obj_t *label_hum;
  lv_obj_t *label_press;
  lv_obj_t *icon;
  lv_obj_t *description;
  lv_obj_t *wind_data;
} weather_widget_t;

lv_obj_t * today_weather_create(lv_obj_t * parent);
void today_weather_set_data(lv_obj_t * widget, const WeatherData &data);
