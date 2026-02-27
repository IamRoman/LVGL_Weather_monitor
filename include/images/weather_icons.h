#pragma once

#include <map>
#include <string>
#include "lvgl.h"

// Оголошення всіх іконок
LV_IMG_DECLARE(img_clear_day);
LV_IMG_DECLARE(img_clear_night);
LV_IMG_DECLARE(img_few_clouds_day);
LV_IMG_DECLARE(img_few_clouds_night);
LV_IMG_DECLARE(img_cloudy);
LV_IMG_DECLARE(img_broken_clouds_day);
LV_IMG_DECLARE(img_broken_clouds_night);
LV_IMG_DECLARE(img_shower_rain_day);
LV_IMG_DECLARE(img_shower_rain_night);
LV_IMG_DECLARE(img_rain);
LV_IMG_DECLARE(img_thunderstorm);
LV_IMG_DECLARE(img_snow);
LV_IMG_DECLARE(img_mist_day);
LV_IMG_DECLARE(img_mist_night);

const lv_img_dsc_t* getWeatherIcon(const char* code);