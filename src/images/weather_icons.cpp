#include "weather_icons.h"
#include <string.h>

struct IconMap {
	const char* code;
	const lv_img_dsc_t* icon;
};

static constexpr IconMap iconTable[] = {
    {"01d", &img_clear_day},
    {"01n", &img_clear_night},
    {"02d", &img_few_clouds_day},
    {"02n", &img_few_clouds_night},
    {"03d", &img_cloudy},
    {"03n", &img_cloudy},
    {"04d", &img_broken_clouds_day},
    {"04n", &img_broken_clouds_night},
    {"09d", &img_shower_rain_day},
    {"09n", &img_shower_rain_night},
    {"10d", &img_rain},
    {"10n", &img_rain},
    {"11d", &img_thunderstorm},
    {"11n", &img_thunderstorm},
    {"13d", &img_snow},
    {"13n", &img_snow},
    {"50d", &img_mist_day},
    {"50n", &img_mist_night},
};

const lv_img_dsc_t* getWeatherIcon(const char* code)
{
	for (const auto& item : iconTable)
	{
		if (strcmp(item.code, code) == 0)
				return item.icon;
	}

	return &img_clear_day; // fallback
}