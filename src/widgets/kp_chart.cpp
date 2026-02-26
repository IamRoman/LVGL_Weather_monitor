#include "kp_chart.h"
#include <stdio.h>
#include <time.h>
#include <theme/colors.h>

kp_chart_t *kp_chart_create(lv_obj_t *parent)
{
	static kp_chart_t chart;

	const int bar_width = 30;
	const int bar_spacing = 20;
	const int chart_height = 80;
	const int top_offset = 30;
	const int bottom_offset = 30;
	const int container_width = 200;
	const int container_height = chart_height + top_offset + bottom_offset;

	/* --- Container --- */
	chart.container = lv_obj_create(parent);
	lv_obj_set_size(chart.container, container_width, container_height);
	lv_obj_center(chart.container);
	lv_obj_clear_flag(chart.container, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_style_pad_all(chart.container, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(chart.container, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(chart.container, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_opa(chart.container, LV_OPA_TRANSP, LV_PART_MAIN);

	/* --- Title --- */
	chart.title_label = lv_label_create(chart.container);
	lv_label_set_text(chart.title_label, "");
	lv_obj_align(chart.title_label, LV_ALIGN_TOP_MID, 0, 10);

	/* --- Centering bars --- */
	int content_width = 4 * bar_width + 3 * bar_spacing;
	int start_x = (container_width - content_width) / 2;
	int base_y = top_offset + chart_height;

	/* --- Horizontal line --- */
	static lv_point_precise_t line_points[2];
	line_points[0].x = start_x;
	line_points[0].y = base_y;
	line_points[1].x = start_x + content_width;
	line_points[1].y = base_y;

	lv_obj_t *x_line = lv_line_create(chart.container);
	lv_line_set_points(x_line, line_points, 2);
	lv_obj_set_style_line_width(x_line, 2, LV_PART_MAIN);
	lv_obj_set_style_line_color(x_line, WHITE, LV_PART_MAIN);

	/* --- Creating bars and labels --- */
	for (int i = 0; i < KP_FORECAST_DAYS; i++)
	{
		chart.bars[i] = lv_obj_create(chart.container);
		lv_obj_set_size(chart.bars[i], bar_width, 10); // висота 10 по дефолту
		lv_obj_clear_flag(chart.bars[i], LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_pos(chart.bars[i],
									 start_x + i * (bar_width + bar_spacing),
									 base_y - 10);

		lv_obj_set_style_border_width(chart.bars[i], 0, LV_PART_MAIN);

		chart.value_labels[i] = lv_label_create(chart.container);
		lv_label_set_text(chart.value_labels[i], "");
		lv_obj_set_style_text_color(chart.value_labels[i], COLOR_TEXT_PRIMARY, LV_PART_MAIN);

		chart.x_labels[i] = lv_label_create(chart.container);
		lv_label_set_text(chart.x_labels[i], "");
		lv_obj_align_to(chart.x_labels[i], chart.bars[i], LV_ALIGN_OUT_BOTTOM_MID, -15, 5);
		lv_obj_set_style_text_color(chart.x_labels[i], COLOR_TEXT_PRIMARY, LV_PART_MAIN);
	}

	return &chart;
}

void kp_chart_set_data(kp_chart_t *chart, const WeatherData &data)
{
	const int chart_height = 80;
	const int max_value = 10;
	const int top_offset = 30;
	const int bar_width = 30;
	const int bar_spacing = 20;
	const int container_width = 200;

	int content_width = 4 * bar_width + 3 * bar_spacing;
	int start_x = (container_width - content_width) / 2;
	int base_y = top_offset + chart_height;

	float current_kp = data.kp_forecast[0];
	int current_storm_forecast = data.storm_forecast[0];

	/* --- Updating the title --- */
	char title_buf[32];
	int w = (int)current_kp;
	int f = (int)((current_kp - w) * 10 + 0.5);
	sprintf(title_buf, "Today Kp: %d.%d (G%d)", w, f, current_storm_forecast);
	lv_label_set_text(chart->title_label, title_buf);
	lv_obj_set_style_text_font(chart->title_label,
														 &lv_font_montserrat_18, 0);

	if (current_kp <= 3)
		lv_obj_set_style_text_color(chart->title_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
	else if (current_kp <= 6)
		lv_obj_set_style_text_color(chart->title_label, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
	else
		lv_obj_set_style_text_color(chart->title_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

	/* --- Generate DD/MM dates --- */
	char x_labels[KP_FORECAST_DAYS][6];
	time_t t = time(NULL);
	for (int i = 0; i < KP_FORECAST_DAYS; i++)
	{
		struct tm tm_info = *localtime(&t);
		tm_info.tm_mday += i;
		mktime(&tm_info);
		sprintf(x_labels[i], "%02d/%02d", tm_info.tm_mday, tm_info.tm_mon + 1);
	}

	/* --- Updating bars and captions --- */
	for (int i = 0; i < KP_FORECAST_DAYS; i++)
	{
		int bar_height = (data.kp_forecast[i] * chart_height) / max_value;

		lv_obj_set_size(chart->bars[i], bar_width, bar_height);
		lv_obj_set_pos(chart->bars[i],
									 start_x + i * (bar_width + bar_spacing),
									 base_y - bar_height);

		if (data.kp_forecast[i] <= 3)
			lv_obj_set_style_bg_color(chart->bars[i], lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
		else if (data.kp_forecast[i] <= 6)
			lv_obj_set_style_bg_color(chart->bars[i], lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
		else
			lv_obj_set_style_bg_color(chart->bars[i], lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

		/* --- Signature above the bar --- */
		char buf[16];
		int w2 = (int)data.kp_forecast[i];
		int f2 = (int)((data.kp_forecast[i] - w2) * 100 + 0.5);
		sprintf(buf, "%d.%02d", w2, f2);
		lv_label_set_text(chart->value_labels[i], buf);
		lv_obj_align_to(chart->value_labels[i], chart->bars[i], LV_ALIGN_OUT_TOP_MID, 0, -5);

		/* --- X-axis label --- */
		lv_label_set_text(chart->x_labels[i], x_labels[i]);
	}
}