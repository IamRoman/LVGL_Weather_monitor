#include "weather_chart.h"

void create_weather_chart_V1(lv_obj_t *parent)
{
	// Масиви даних: X (години), Y (температура)
	static lv_coord_t x_values[] = {0, 3, 6, 9, 12, 15, 18, 21};
	static lv_coord_t y_values[] = {-20, -10, 0, 10, 15, 22, -18, -34};
	const uint16_t num_points = 8;

	// ===== CHART =====
	lv_obj_t *chart = lv_chart_create(parent);
	lv_obj_set_size(chart, 220, 150);
	lv_obj_center(chart);

	// Встановлюємо тип SCATTER
	lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
	lv_chart_set_point_count(chart, num_points);

	// Діапазони для обох осей
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -40, 50);
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 21);

	lv_obj_set_style_border_width(chart, 0, 0);
	lv_obj_set_style_bg_color(chart, lv_color_hex(0x1e1e2f), 0);
	lv_chart_set_div_line_count(chart, 5, 7);

	// Додаємо серію
	lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0x00E0FF), LV_CHART_AXIS_PRIMARY_Y);

	// Прив'язуємо масиви до серії (важливо: масиви мають бути static)
	lv_chart_set_ext_x_array(chart, ser, x_values);
	lv_chart_set_ext_y_array(chart, ser, y_values);

	// ===== X ПІДПИСИ =====
	lv_obj_t *x_labels = lv_label_create(parent); // Використовуємо parent замість screen
	lv_label_set_text(x_labels, "00   03   06   09   12   15   18   21");
	lv_obj_set_style_text_color(x_labels, lv_color_hex(0x4DA6FF), 0);
	lv_obj_align_to(x_labels, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

	lv_chart_refresh(chart);

	// ===== Додавання значень над точками =====
	// Оновлюємо координати об'єктів після рефрешу
	for (int i = 0; i < num_points; i++)
	{
		lv_point_t p;
		lv_chart_get_point_pos_by_id(chart, ser, i, &p);

		lv_obj_t *label = lv_label_create(chart);
		lv_label_set_text_fmt(label, "%d", y_values[i]);
		lv_obj_set_style_text_color(label, lv_color_hex(0x4DA6FF), 0);
		lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);

		// Позиціонування (враховуйте, що в LVGL 8+ координати точок відносні всередині чарта)
		lv_obj_align(label, LV_ALIGN_TOP_LEFT, p.x - 10, p.y - 20);
	}
}

static void chart_event_cb(lv_event_t *e)
{
	lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
	lv_event_code_t code = lv_event_get_code(e);

	// Малюємо підписи ПІСЛЯ того, як намальований основний графік
	if (code == LV_EVENT_DRAW_POST)
	{
		lv_layer_t *layer = lv_event_get_layer(e);

		lv_draw_label_dsc_t label_dsc;
		lv_draw_label_dsc_init(&label_dsc);
		label_dsc.color = lv_color_hex(0x4DA6FF);
		label_dsc.font = &lv_font_montserrat_14; // Переконайтеся, що цей шрифт включений

		lv_area_t chart_area;
		lv_obj_get_content_coords(obj, &chart_area);

		// --- Ось Y ---
		for (int i = 0; i < 5; i++)
		{
			int32_t val = -40 + (i * 90 / 4);
			char buf[16];
			lv_snprintf(buf, sizeof(buf), "%d", (int)val);

			int32_t y = chart_area.y2 - (i * lv_area_get_height(&chart_area) / 4);

			lv_area_t a;
			a.x1 = chart_area.x1 - 45;
			a.x2 = chart_area.x1 - 5;
			a.y1 = y - 7;
			a.y2 = y + 7;
			label_dsc.text = buf;
			label_dsc.align = LV_TEXT_ALIGN_RIGHT;
			lv_draw_label(layer, &label_dsc, &a);
		}

		// --- Ось X ---
		const char *x_ticks[] = {"0", "3", "6", "9", "12", "15", "18", "21", "24"};
		for (int i = 0; i < 9; i++)
		{
			int32_t x = chart_area.x1 + (i * lv_area_get_width(&chart_area) / 7);

			lv_area_t a;
			a.x1 = x - 20;
			a.x2 = x + 20;
			a.y1 = chart_area.y2 + 5;
			a.y2 = chart_area.y2 + 25;
			label_dsc.text = x_ticks[i];
			label_dsc.align = LV_TEXT_ALIGN_CENTER;
			lv_draw_label(layer, &label_dsc, &a);
		}
	}
}

void create_weather_chart(lv_obj_t *parent)
{
	// Дані мають бути static
	static int32_t x_values[] = {0, 3, 6, 9, 12, 15, 18, 21, 24};
	static int32_t y_values[] = {-20, -10, 0, 10, 15, 22, -18, -14, 0};

	lv_obj_t *chart = lv_chart_create(parent);
	lv_obj_set_size(chart, 240, 150);
	lv_obj_center(chart);
	lv_obj_set_style_border_width(chart, 0, 0);

	// Налаштування Scatter
	lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -40, 50);
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 24);
	lv_chart_set_div_line_count(chart, 0, 0);
	lv_chart_set_point_count(chart, 9); // Чітко 9 точок

	// Стиль фону та відступи (ВАЖЛИВО: великі падінги, щоб текст не обрізався)
	lv_obj_set_style_bg_color(chart, lv_color_hex(0x1e1e2f), 0);
	lv_obj_set_style_pad_left(chart, 20, 0);
	lv_obj_set_style_pad_bottom(chart, 20, 0);
	lv_obj_set_style_pad_right(chart, 20, 0);			 // Трохи справа для останньої мітки X
	lv_obj_set_style_clip_corner(chart, false, 0); // Дозволяємо малювати за межами

	// Додаємо серію
	lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0x00E0FF), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_ext_x_array(chart, ser, x_values);
	lv_chart_set_ext_y_array(chart, ser, y_values);

	// Додаємо колбек
	lv_obj_add_event_cb(chart, chart_event_cb, LV_EVENT_ALL, NULL);

	for (int i = 0; i < 9; i++)
	{
		lv_point_t p;
		lv_chart_get_point_pos_by_id(chart, ser, i, &p);

		lv_obj_t *label = lv_label_create(chart);
		lv_label_set_text_fmt(label, "%d", y_values[i]);
		lv_obj_set_style_text_color(label, lv_color_hex(0x4DA6FF), 0);
		lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);

		// Позиціонування (враховуйте, що в LVGL 8+ координати точок відносні всередині чарта)
		lv_obj_align(label, LV_ALIGN_TOP_LEFT, p.x - 10, p.y - 20);
	}

	lv_chart_refresh(chart);
}