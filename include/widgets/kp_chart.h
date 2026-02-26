#pragma once
#include "lvgl.h"
#include "weather_service.h"

#define KP_FORECAST_DAYS 4

typedef struct
{
  lv_obj_t *container;
  lv_obj_t *bars[KP_FORECAST_DAYS];
  lv_obj_t *value_labels[KP_FORECAST_DAYS];
  lv_obj_t *x_labels[KP_FORECAST_DAYS];
  lv_obj_t *title_label;
} kp_chart_t;

// Creating a chart container without initializing data
kp_chart_t *kp_chart_create(lv_obj_t *parent);

// Updating chart data
void kp_chart_set_data(kp_chart_t *chart, const WeatherData &data);