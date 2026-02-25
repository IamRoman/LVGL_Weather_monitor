#include "kp_chart.h"
#include <stdio.h>
#include <time.h>

void create_kp_chart(lv_obj_t *parent, float *kp_values)
{
	// Дані магнітної бурі для 4 днів
	float data[4] = {3.0f, 5.5f, 2.25f, 7.75f};
	char x_labels[4][6]; // 4 дні, формат "DD/MM"
	float current_kp = kp_values[0];

	// Отримуємо сьогоднішню дату та наступні 3 дні
	time_t t = time(NULL);
	struct tm tm_info;
	for (int i = 0; i < 4; i++)
	{
		tm_info = *localtime(&t);
		tm_info.tm_mday += i;
		mktime(&tm_info);
		sprintf(x_labels[i], "%02d/%02d", tm_info.tm_mday, tm_info.tm_mon + 1);
	}

	const int bar_width = 30;
	const int bar_spacing = 20;
	const int chart_height = 80;
	const int max_value = 10;

	// Контейнер чарта
	lv_obj_t *chart_cont = lv_obj_create(parent);
	lv_obj_set_size(chart_cont, 4 * (bar_width + bar_spacing) + 40, chart_height + 90);
	lv_obj_center(chart_cont);
	lv_obj_clear_flag(chart_cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(chart_cont, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_border_width(chart_cont, 1, LV_PART_MAIN);

	// Тайтл: поточне значення Kp
	char title_buf[32];
	int whole = (int)current_kp;
	int frac = (int)((current_kp - whole) * 10 + 0.5);
	lv_snprintf(title_buf, sizeof(title_buf), "Current Kp: %d.%d", whole, frac);

	lv_obj_t *title_label = lv_label_create(chart_cont);
	lv_label_set_text(title_label, title_buf);
	lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 5);

	if (current_kp <= 3)
		lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
	else if (current_kp <= 6)
		lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
	else
		lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

	// Горизонтальна вісь X
	static lv_point_precise_t line_points[2];
	line_points[0].x = 10;
	line_points[0].y = chart_height;
	line_points[1].x = 4 * (bar_width + bar_spacing) + 30;
	line_points[1].y = chart_height;

	lv_obj_t *x_line = lv_line_create(chart_cont);
	lv_line_set_points(x_line, line_points, 2);
	lv_obj_align(x_line, LV_ALIGN_TOP_LEFT, 0, 30);
	lv_obj_set_style_line_width(x_line, 2, LV_PART_MAIN);
	lv_obj_set_style_line_color(x_line, lv_color_black(), LV_PART_MAIN);

	// Малюємо бари
	for (int i = 0; i < 4; i++)
	{
		int bar_height = (data[i] * chart_height) / max_value;

		lv_obj_t *bar = lv_obj_create(chart_cont);
		lv_obj_set_size(bar, bar_width, bar_height);
		lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_align(bar, LV_ALIGN_TOP_LEFT, 20 + i * (bar_width + bar_spacing), 30 + chart_height - bar_height);

		if (data[i] <= 3)
			lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
		else if (data[i] <= 6)
			lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
		else
			lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

		lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN);

		// Підпис над баром з float 2 знаки
		char buf[16];
		int w = (int)data[i];
		int f = (int)((data[i] - w) * 100 + 0.5); // 2 знаки після коми
		sprintf(buf, "%d.%02d", w, f);

		lv_obj_t *label = lv_label_create(chart_cont); // створюємо label
		lv_label_set_text(label, buf);
		lv_obj_align_to(label, bar, LV_ALIGN_OUT_TOP_MID, 0, -5);

		// Підпис осі X
		lv_obj_t *x_label = lv_label_create(chart_cont);
		lv_label_set_text(x_label, x_labels[i]);
		lv_obj_align_to(x_label, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	}
}