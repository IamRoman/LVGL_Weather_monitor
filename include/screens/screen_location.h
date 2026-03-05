#pragma once
#include <lvgl.h>

struct City
{
  const char *name;
  float lat;
  float lon;
};

static const City cities[] = {
    {"Kyiv", 50.4501, 30.5234},
    {"Lviv", 49.8397, 24.0297},
    {"Odesa", 46.4825, 30.7233},
    {"Kharkiv", 49.9935, 36.2304},
    {"Dnipro", 48.4647, 35.0462},
    {"Verbka", 48.213889, 29.284722},
    {"Custom", 0.0, 0.0}};

#define CITY_COUNT (sizeof(cities) / sizeof(cities[0]))

void screen_location_init(void);
lv_obj_t * screen_location_get(void);
