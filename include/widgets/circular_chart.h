#pragma once
#include <lvgl.h>

lv_obj_t * circular_chart_create(lv_obj_t * parent, const int value);
void circular_chart_set_data(lv_obj_t * widget, const int value);
