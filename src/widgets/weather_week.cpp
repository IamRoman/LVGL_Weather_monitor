#include "weather_week.h"
#include <stdlib.h> // for malloc
#include <weather_icons.h>
#include <weather_service.h>
#include <theme/colors.h>

static void clean(lv_obj_t *obj)
{
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
}

weather_week_widget * weather_week_create(lv_obj_t * parent, int days)
{
	if(days > DAYS_COUNT) days = DAYS_COUNT;

	weather_week_widget * w =
			(weather_week_widget *)malloc(sizeof(weather_week_widget));

	w->num_days = days;

	lv_obj_t * main_container = lv_obj_create(parent);
	lv_obj_set_size(main_container, LV_PCT(100), 100);
	lv_obj_set_layout(main_container, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(main_container, LV_FLEX_FLOW_ROW);
	clean(main_container);

	w->scroll = main_container;

	static lv_style_t style_secondary_label;
  lv_style_init(&style_secondary_label);
  lv_style_set_text_color(&style_secondary_label, WHITE);
  lv_style_set_text_font(&style_secondary_label, &lv_font_montserrat_14);

	for(int i = 0; i < days; i++)
	{
		lv_obj_t * item = lv_obj_create(main_container);
		lv_obj_set_size(item, 60, LV_PCT(100));
		clean(item);
		lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

		// Day
		lv_obj_t * label_day = lv_label_create(item);
		lv_label_set_text(label_day, "---");
		lv_obj_set_style_text_color(label_day, lv_color_white(), 0);
		lv_obj_add_style(label_day, &style_secondary_label, 0);
		lv_obj_align(label_day, LV_ALIGN_TOP_MID, 0, 0);

		// temperature
		lv_obj_t * label_temp = lv_label_create(item);
		lv_label_set_text(label_temp, "--°C");
		lv_obj_add_style(label_temp, &style_secondary_label, 0);
		lv_obj_align(label_temp, LV_ALIGN_CENTER, 0, -10);

		// Icon
		lv_obj_t * icon = lv_image_create(item);
		lv_img_set_src(icon, getWeatherIcon("01d"));
		lv_obj_align(icon, LV_ALIGN_BOTTOM_MID, 0, -5);


		w->items[i].cont = item;
		w->items[i].label_day = label_day;
		w->items[i].label_temp = label_temp;
		w->items[i].icon = icon;
	}

  return w;
}

void weather_week_update(weather_week_widget * w, const DailyForecast * daily)
{
	for(int i=0; i<w->num_days; i++)
	{
		// Виводимо день місяця або день тижня
		char day_str[16];
		snprintf(day_str, sizeof(day_str), "%.2s/%.2s", daily[i].date+8, daily[i].date+5); 
		// наприклад, "01-03" день-місяць
		lv_label_set_text(w->items[i].label_day, day_str);

		// Temperature min - max
		char buf[16];
		snprintf(buf, sizeof(buf), "%.0f  %.0f°", daily[i].min_temp, daily[i].max_temp);
		lv_label_set_text(w->items[i].label_temp, buf);

		// Icon
		lv_img_set_src(w->items[i].icon, getWeatherIcon(daily[i].icon));
	}
}