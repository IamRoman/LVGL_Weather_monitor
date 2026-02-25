#include "kp_chart.h"
#include <stdio.h>
#include <time.h>

#define KP_FORECAST_DAYS 4

typedef struct {
    float *kp_values;
    uint32_t count;
} chart_user_data_t;

static lv_color_t kp_to_color(float kp)
{
	if (kp < 4.0f)
		return lv_palette_main(LV_PALETTE_GREEN); // слабка буря
	else if (kp < 6.0f)
		return lv_palette_main(LV_PALETTE_YELLOW); // середня буря
	else
		return lv_palette_main(LV_PALETTE_RED); // сильна буря
}

static void chart_event_cb(lv_event_t *e)
{
	lv_obj_t *chart = (lv_obj_t *)lv_event_get_target(e);
	if (lv_event_get_code(e) != LV_EVENT_DRAW_POST)
		return;

	lv_layer_t *layer = lv_event_get_layer(e);

	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	label_dsc.color = lv_color_hex(0x4DA6FF);
	label_dsc.font = &lv_font_montserrat_10;
	label_dsc.align = LV_TEXT_ALIGN_CENTER;

	lv_area_t chart_area;
	lv_obj_get_content_coords(chart, &chart_area);
	int32_t chart_w = lv_area_get_width(&chart_area);

	chart_user_data_t *ud = (chart_user_data_t *)lv_obj_get_user_data(chart);

	/* Генеруємо підписи осі X — сьогодні + 3 дні */
	char x_ticks[4][6]; // 4 дні, формат dd/mm
	time_t t = time(NULL);
	struct tm tm_info = *localtime(&t);
	for (int i = 0; i < 4; i++)
	{
		struct tm tmp = tm_info;
		tmp.tm_mday += i; // додаємо дні
		mktime(&tmp);			// коригує перехід місяця/року
		strftime(x_ticks[i], sizeof(x_ticks[i]), "%d/%m", &tmp);
	}

	/* Малюємо підписи осі X */
	for (uint32_t i = 0; i < ud->count; i++)
	{
		int32_t x = chart_area.x1 + (i * chart_w) / (ud->count - 1);

		lv_area_t area;
		area.x1 = x - 20; // трохи ширше для відступу
		area.x2 = x + 20;
		area.y1 = chart_area.y2 + 8;	// верт. відступ від низу графіка
		area.y2 = chart_area.y2 + 12; // підпис нижче бару

		label_dsc.text = x_ticks[i];
		lv_draw_label(layer, &label_dsc, &area);
	}

	/* Значення над кожним баром */
	lv_chart_series_t *ser = NULL;
	uint32_t bar_idx = 0; // серія = індекс бару
	while ((ser = lv_chart_get_series_next(chart, ser)) != NULL)
	{
		lv_point_t pt;
		lv_chart_get_point_pos_by_id(chart, ser, bar_idx, &pt);

		// char buf[16];
		// lv_snprintf(buf, sizeof(buf), "%d", (int)ud->kp_values[bar_idx]);
		// lv_snprintf(buf, sizeof(buf), "%.1f", ud->kp_values[bar_idx]);
		float val = ud->kp_values[bar_idx];
		int whole = (int)val;
		int frac = (int)((val - whole) * 10 + 0.5); // округлення до 1 знаку
		char buf[16];
		lv_snprintf(buf, sizeof(buf), "%d.%d", whole, frac);

		lv_area_t area_v;
		area_v.x1 = chart_area.x1 + pt.x - 15;
		area_v.x2 = chart_area.x1 + pt.x + 15;
		area_v.y1 = chart_area.y1 + pt.y - 18;
		area_v.y2 = chart_area.y1 + pt.y - 3;

		label_dsc.text = buf;
		lv_draw_label(layer, &label_dsc, &area_v);

		bar_idx++; // наступний бар
	}
}

void create_kp_chart(lv_obj_t *parent, float *kp_values)
{
	// lv_obj_t *main_cont = lv_obj_create(parent);
	// lv_obj_set_size(main_cont, LV_PCT(100), LV_PCT(100));
	// lv_obj_center(main_cont);

	lv_obj_t *chart = lv_chart_create(parent);
	lv_obj_set_size(chart, LV_PCT(80), LV_PCT(80));
	lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
	lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 10);
	lv_chart_set_point_count(chart, 4);
	lv_obj_center(chart);

	// Стилізація
	lv_obj_set_style_bg_color(chart, lv_color_hex(0x1e1e2f), 0);
	lv_obj_set_style_border_width(chart, 0, 0);

	// Сітка: 0 горизонтальних ліній, 8 вертикальних (для кожної мітки часу)
	lv_chart_set_div_line_count(chart, 0, 0);

	// створюємо окрему серію для кожного бару
	for (int i = 0; i < 4; i++)
	{
		lv_color_t bar_color = kp_to_color(kp_values[i]);
		lv_chart_series_t *ser = lv_chart_add_series(chart, bar_color, LV_CHART_AXIS_PRIMARY_Y);
		lv_chart_set_value_by_id(chart, ser, i, kp_values[i]);
	}

	/* Передаємо дані через user_data */
	chart_user_data_t *ud = new chart_user_data_t;
	ud->kp_values = kp_values;
	ud->count = 4;
	lv_obj_set_user_data(chart, ud);

	lv_obj_add_event_cb(chart, chart_event_cb, LV_EVENT_ALL, NULL);
	lv_chart_refresh(chart);
}

// void create_kp_chart(lv_obj_t * parent, float * kp_values)
// {
//   /* Create main container */
//     lv_obj_t * main_cont = lv_obj_create(parent);
//     lv_obj_set_size(main_cont, 200, 150);
//     lv_obj_center(main_cont);

//     /* Create wrapper */
//     lv_obj_t * wrapper = lv_obj_create(main_cont);
//     lv_obj_remove_style_all(wrapper);
//     lv_obj_set_size(wrapper, lv_pct(300), lv_pct(100));
//     lv_obj_set_flex_flow(wrapper, LV_FLEX_FLOW_COLUMN);

//     /* Create chart */
//     lv_obj_t * chart = lv_chart_create(wrapper);
//     lv_obj_set_width(chart, lv_pct(100));
//     lv_obj_set_flex_grow(chart, 1);
//     lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
//     lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 10); // Kp індекс від 0 до 9
//     lv_chart_set_point_count(chart, 4);
//     lv_obj_set_style_radius(chart, 0, 0);

//     /* Create X-axis scale */
//     lv_obj_t * scale_bottom = lv_scale_create(wrapper);
//     lv_scale_set_mode(scale_bottom, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
//     lv_obj_set_size(scale_bottom, lv_pct(100), 25);
//     lv_scale_set_total_tick_count(scale_bottom, 4);
//     lv_scale_set_major_tick_every(scale_bottom, 1);
//     lv_obj_set_style_pad_hor(scale_bottom, lv_chart_get_first_point_center_offset(chart), 0);

//     static const char * days[] = {"Day 1", "Day 2", "Day 3", "Day 4", NULL};
//     lv_scale_set_text_src(scale_bottom, days);

//     /* Add data series */
//     lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

//     /* Set values for 4 days (Kp index example) */
//     // int kp_values[4] = {3, 5, 7, 4}; // приклад магнітних бурь
//     for(int i = 0; i < 4; i++) {
//         lv_chart_set_next_value(chart, ser, kp_values[i]);
//     }

//     lv_chart_refresh(chart); /* Refresh chart */
// }