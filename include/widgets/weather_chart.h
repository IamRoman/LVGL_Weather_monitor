#pragma once
#include <lvgl.h>

typedef struct
{
  uint16_t num_points;
  int32_t *y_values;
} chart_event_data_t;
typedef struct
{
  lv_obj_t *chart;
  lv_chart_series_t *series;
  chart_event_data_t *event_data;
} weather_chart_t;

weather_chart_t *create_weather_chart(lv_obj_t *parent);
void set_weather_chart_data(weather_chart_t *wc,
                            int32_t *x_vals,
                            const float *y_vals,
                            uint16_t count);