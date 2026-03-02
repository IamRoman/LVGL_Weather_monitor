#pragma once
#include "lvgl.h"
#include <weather_service.h>

#define DAYS_COUNT 7

// Structure for one day
typedef struct {
  lv_obj_t * cont;
  lv_obj_t * label_day;
  lv_obj_t * label_temp;
  lv_obj_t * icon;
} weather_day_item_t;

// Main widget
typedef struct {
  lv_obj_t * scroll;
  weather_day_item_t items[DAYS_COUNT];
  int num_days;
} weather_week_widget;

weather_week_widget * weather_week_create(lv_obj_t * parent, int days);
void weather_week_update(weather_week_widget * w, const DailyForecast * daily);