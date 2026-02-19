#include "widgets/digital_clock.h"
#include <time.h>
#include <theme/colors.h>

static lv_obj_t * label_time;
static lv_obj_t * label_date;
static bool is_24h = true;

static const char *days[] = {
		"Sunday", "Monday", "Tuesday",
		"Wednesday", "Thursday", "Friday", "Saturday"};

static void update_clock(lv_timer_t * timer)
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	if (!t)
		return;

	int hour = t->tm_hour;

	if (!is_24h)
	{
		hour = hour % 12;
		if (hour == 0)
			hour = 12;
	}

	lv_label_set_text_fmt(label_time,
												is_24h ? "%02d:%02d:%02d"
															 : "%02d:%02d:%02d %s",
												hour,
												t->tm_min,
												t->tm_sec,
												t->tm_hour >= 12 ? "PM" : "AM");

	lv_label_set_text_fmt(label_date,
												"%s, %02d.%02d.%04d",
												days[t->tm_wday],
												t->tm_mday,
												t->tm_mon + 1,
												t->tm_year + 1900);
}

void clock_toggle_format(void)
{
	is_24h = !is_24h;
}

void clock_create(lv_obj_t * parent)
{
	static lv_style_t style_label;
	lv_style_init(&style_label);
	lv_style_set_text_color(&style_label, WHITE);

	label_date = lv_label_create(parent);
	lv_obj_set_style_text_font(label_date,
														 &lv_font_montserrat_14, 0);
	lv_obj_align(label_date, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_add_style(label_date, &style_label, 0);

	label_time = lv_label_create(parent);
	lv_obj_set_style_text_font(label_time,
														 &lv_font_montserrat_40, 0);
	lv_obj_align(label_time, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_add_style(label_time, &style_label, 0);

	lv_timer_create(update_clock, 1000, NULL);
}
