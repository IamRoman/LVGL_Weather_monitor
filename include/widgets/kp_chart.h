#pragma once
#include "lvgl.h"

#define KP_FORECAST_DAYS 4

void create_kp_chart(lv_obj_t * parent, float * kp_values);