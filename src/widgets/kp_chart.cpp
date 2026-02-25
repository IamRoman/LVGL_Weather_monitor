#include "kp_chart.h"
#include <stdio.h>
#include <time.h>
#include <theme/colors.h>

void create_kp_chart(lv_obj_t *parent, float *kp_values)
{
	/* --- Дані --- */
	float data[4] = {
			kp_values[0],
			kp_values[1],
			kp_values[2],
			kp_values[3]};

	float current_kp = data[0];

	/* --- Генеруємо дати DD/MM --- */
	char x_labels[4][6];
	time_t t = time(NULL);

	for (int i = 0; i < 4; i++)
	{
		struct tm tm_info = *localtime(&t);
		tm_info.tm_mday += i;
		mktime(&tm_info);
		sprintf(x_labels[i], "%02d/%02d",
						tm_info.tm_mday,
						tm_info.tm_mon + 1);
	}

	/* --- Параметри --- */
	const int bar_width = 30;
	const int bar_spacing = 20;
	const int chart_height = 80;
	const int max_value = 10;

	const int top_offset = 30;
	const int bottom_offset = 30;

	const int container_width = 200;
	const int container_height = chart_height + top_offset + bottom_offset;

	/* --- Контейнер --- */
	lv_obj_t *chart_cont = lv_obj_create(parent);
	lv_obj_set_size(chart_cont, container_width, container_height);
	lv_obj_center(chart_cont);
	lv_obj_clear_flag(chart_cont, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_style_pad_all(chart_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(chart_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(chart_cont, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_opa(chart_cont, LV_OPA_TRANSP, LV_PART_MAIN);

	/* --- Тайтл --- */
	char title_buf[32];
	int whole = (int)current_kp;
	int frac = (int)((current_kp - whole) * 10 + 0.5);
	sprintf(title_buf, "Current Kp: %d.%d", whole, frac);

	lv_obj_t *title_label = lv_label_create(chart_cont);
	lv_label_set_text(title_label, title_buf);
	lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 5);

	if (current_kp <= 3)
		lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
	else if (current_kp <= 6)
		lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
	else
		lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

	/* --- Центрування барів --- */
	int content_width = 4 * bar_width + 3 * bar_spacing;
	int start_x = (container_width - content_width) / 2;
	int base_y = top_offset + chart_height;

	/* --- Горизонтальна лінія --- */
	static lv_point_precise_t line_points[2];

	line_points[0].x = start_x;
	line_points[0].y = base_y;

	line_points[1].x = start_x + content_width;
	line_points[1].y = base_y;

	lv_obj_t *x_line = lv_line_create(chart_cont);
	lv_line_set_points(x_line, line_points, 2);
	lv_obj_set_style_line_width(x_line, 2, LV_PART_MAIN);
	lv_obj_set_style_line_color(x_line, WHITE, LV_PART_MAIN);

	/* --- Бари --- */
	for (int i = 0; i < 4; i++)
	{
		int bar_height = (data[i] * chart_height) / max_value;

		lv_obj_t *bar = lv_obj_create(chart_cont);
		lv_obj_set_size(bar, bar_width, bar_height);
		lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_pos(bar,
									 start_x + i * (bar_width + bar_spacing),
									 base_y - bar_height);

		lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN);

		if (data[i] <= 3)
			lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
		else if (data[i] <= 6)
			lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
		else
			lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

		/* --- Підпис над баром (2 знаки) --- */
		char buf[16];
		int w = (int)data[i];
		int f = (int)((data[i] - w) * 100 + 0.5);
		sprintf(buf, "%d.%02d", w, f);

		lv_obj_t *label = lv_label_create(chart_cont);
		lv_label_set_text(label, buf);
		lv_obj_align_to(label, bar, LV_ALIGN_OUT_TOP_MID, 0, -5);
		lv_obj_set_style_text_color(label, COLOR_TEXT_PRIMARY, LV_PART_MAIN);

		/* --- Підпис осі X --- */
		lv_obj_t *x_label = lv_label_create(chart_cont);
		lv_label_set_text(x_label, x_labels[i]);
		lv_obj_align_to(x_label, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
		lv_obj_set_style_text_color(x_label, COLOR_TEXT_PRIMARY, LV_PART_MAIN);
	}
}