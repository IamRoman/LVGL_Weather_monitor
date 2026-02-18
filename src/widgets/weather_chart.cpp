#include "weather_chart.h"

#include "lvgl.h"

// Дані (static обов'язково для зовнішніх масивів)
static int32_t x_values[] = {0, 3, 6, 9, 12, 15, 18, 21};
static int32_t y_values[] = {-20, -10, 0, 10, 15, 22, 18, 34};
const uint16_t num_points = 8;

static void chart_event_cb(lv_event_t *e)
{
  lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_DRAW_POST)
  {
    lv_layer_t *layer = lv_event_get_layer(e);

    // Налаштування стилю тексту
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_hex(0x4DA6FF);
    label_dsc.font = &lv_font_montserrat_10;

    lv_area_t chart_area;
    lv_obj_get_content_coords(obj, &chart_area);
    int32_t chart_w = lv_area_get_width(&chart_area);
    int32_t chart_h = lv_area_get_height(&chart_area);

    // --- РОЗРАХУНОК ПОЗИЦІЇ "0" (Діапазон -40...50) ---
    // 0 знаходиться на позначці 40 одиниць від -40 (загальна шкала 90)
    int32_t y_zero_pos = chart_area.y2 - (40 * chart_h / 90);

    // --- 1. МАЛЮЄМО ЛІНІЮ НУЛЯ (LVGL 9.4 STYLE) ---
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0x33334d);
    line_dsc.width = 1;
    line_dsc.p1.x = (lv_value_precise_t)chart_area.x1;
    line_dsc.p1.y = (lv_value_precise_t)y_zero_pos;
    line_dsc.p2.x = (lv_value_precise_t)chart_area.x2;
    line_dsc.p2.y = (lv_value_precise_t)y_zero_pos;
    lv_draw_line(layer, &line_dsc);

    // --- 2. ПІДПИС "0" НА ОСІ Y ---
    lv_area_t area_y;
    area_y.x1 = chart_area.x1 - 45;
    area_y.x2 = chart_area.x1 - 5;
    area_y.y1 = y_zero_pos - 7;
    area_y.y2 = y_zero_pos + 7;
    label_dsc.text = "0";
    label_dsc.align = LV_TEXT_ALIGN_RIGHT;
    lv_draw_label(layer, &label_dsc, &area_y);

    // --- 3. ВІСЬ X (ГОДИНИ: 00, 03...21) ---
    const char *x_ticks[] = {"00", "03", "06", "09", "12", "15", "18", "21"};
    for (int i = 0; i < 8; i++)
    {
      int32_t x = chart_area.x1 + (i * chart_w / 7);
      lv_area_t area_x;
      area_x.x1 = x - 20;
      area_x.x2 = x + 20;
      area_x.y1 = chart_area.y2 + 5;
      area_x.y2 = chart_area.y2 + 25;
      label_dsc.text = x_ticks[i];
      label_dsc.align = LV_TEXT_ALIGN_CENTER;
      lv_draw_label(layer, &label_dsc, &area_x);
    }

    // --- 4. ЗНАЧЕННЯ ТЕМПЕРАТУРИ НАД ТОЧКАМИ ---
    lv_chart_series_t *ser = lv_chart_get_series_next(obj, NULL);
    if (ser)
    {
      for (int i = 0; i < num_points; i++)
      {
        lv_point_t pt;
        lv_chart_get_point_pos_by_id(obj, ser, i, &pt);

        char buf[16];
        lv_snprintf(buf, sizeof(buf), "%d", (int)y_values[i]);

        lv_area_t area_v;
        // Позиція над точкою
        area_v.x1 = chart_area.x1 + pt.x - 15;
        area_v.x2 = chart_area.x1 + pt.x + 15;
        area_v.y1 = chart_area.y1 + pt.y - 18;
        area_v.y2 = chart_area.y1 + pt.y - 3;

        label_dsc.text = buf;
        label_dsc.align = LV_TEXT_ALIGN_CENTER;
        lv_draw_label(layer, &label_dsc, &area_v);
      }
    }
  }
}

void create_weather_chart(lv_obj_t *parent)
{
  lv_obj_t *chart = lv_chart_create(parent);
  lv_obj_set_size(chart, 240, 150);
  lv_obj_center(chart);

  // Основні налаштування графіка
  lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -40, 50);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 21);
  lv_chart_set_point_count(chart, num_points);

  // Сітка: 0 горизонтальних ліній, 8 вертикальних (для кожної мітки часу)
  lv_chart_set_div_line_count(chart, 0, 8);

  // Стилізація
  lv_obj_set_style_bg_color(chart, lv_color_hex(0x1e1e2f), 0);
  lv_obj_set_style_border_width(chart, 0, 0);

  // Відступи (Padding), щоб влізли підписи зовні графіка
  lv_obj_set_style_pad_left(chart, 50, 0);
  lv_obj_set_style_pad_bottom(chart, 35, 0);
  lv_obj_set_style_clip_corner(chart, false, 0); // Дозволяє малювати підписи в зоні Padding

  // Стиль маркерів та ліній
  lv_obj_set_style_size(chart, 5, 5, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);
  lv_chart_set_div_line_count(chart, 0, 0);

  // Створення серії та прив'язка даних
  lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0x00E0FF), LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_ext_x_array(chart, ser, (int32_t *)x_values);
  lv_chart_set_ext_y_array(chart, ser, (int32_t *)y_values);

  // Додавання колбеку для малювання підписів та лінії нуля
  lv_obj_add_event_cb(chart, chart_event_cb, LV_EVENT_ALL, NULL);

  lv_chart_refresh(chart);
}