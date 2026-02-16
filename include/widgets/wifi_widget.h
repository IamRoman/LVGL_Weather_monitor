#pragma once
#include <lvgl.h>

lv_obj_t* wifi_icon_create(lv_obj_t* parent);
void wifi_icon_set_level(lv_obj_t* icon, uint8_t level); // 0-4