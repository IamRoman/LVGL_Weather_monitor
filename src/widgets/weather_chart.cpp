#include "weather_chart.h"
#include "lvgl.h"
#include <theme/colors.h>
#include <stdio.h>
#include <cmath>
#include <time.h>

int32_t y_vals_int[8];

const char *all_ticks[] = {"00", "03", "06", "09", "12", "15", "18", "21"};
const int TICKS_COUNT = 8;
const char *x_ticks[TICKS_COUNT];

static void chart_event_cb(lv_event_t *e)
{
  chart_event_data_t *data = (chart_event_data_t *)lv_event_get_user_data(e);
  if (!data)
    return;

  uint16_t num_points = data->num_points;
  int32_t *y_values = data->y_values;

  lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_DRAW_POST)
  {
    lv_layer_t *layer = lv_event_get_layer(e);

    // Text style settings
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = WHITE;
    label_dsc.font = &lv_font_montserrat_12;

    lv_area_t chart_area;
    lv_obj_get_content_coords(obj, &chart_area);
    int32_t chart_w = lv_area_get_width(&chart_area);
    int32_t chart_h = lv_area_get_height(&chart_area);

    // --- CALCULATION OF POSITION "0" (Range -40...50) ---
    // 0 is at 40 units from -40 (total scale 90)
    int32_t y_zero_pos = chart_area.y2 - (40 * chart_h / 90);

    // --- DRAWING THE ZERO LINE ---
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = MIDDLE_GRAY;
    line_dsc.width = 1;
    line_dsc.p1.x = (lv_value_precise_t)chart_area.x1;
    line_dsc.p1.y = (lv_value_precise_t)y_zero_pos;
    line_dsc.p2.x = (lv_value_precise_t)chart_area.x2;
    line_dsc.p2.y = (lv_value_precise_t)y_zero_pos;
    lv_draw_line(layer, &line_dsc);

    // --- "0" SUBSTITUTE ON Y AXIS ---
    lv_area_t area_y;
    area_y.x1 = chart_area.x1 - 45;
    area_y.x2 = chart_area.x1 - 5;
    area_y.y1 = y_zero_pos - 7;
    area_y.y2 = y_zero_pos + 7;
    label_dsc.text = "0";
    label_dsc.align = LV_TEXT_ALIGN_RIGHT;
    lv_draw_label(layer, &label_dsc, &area_y);

    // Get the current time
    time_t now = time(nullptr);
    struct tm *tm_info = localtime(&now);
    int hour_now = tm_info->tm_hour;

    // Index of the nearest 3-hour mark
    int start_index = hour_now / 3;

    // Forming x_ticks for the chart
    for (int i = 0; i < TICKS_COUNT; i++)
    {
      x_ticks[i] = all_ticks[(start_index + i) % TICKS_COUNT];
    }

    char buf[4];
    for (int i = 0; i < 8; i++)
    {
      int32_t x = chart_area.x1 + (i * chart_w / 7);
      lv_area_t area_x;
      area_x.x1 = x - 20;
      area_x.x2 = x + 20;
      area_x.y1 = chart_area.y2 + 5;
      area_x.y2 = chart_area.y2 + 25;

      sprintf(buf, "%s", x_ticks[i]);
      label_dsc.text = buf;
      label_dsc.align = LV_TEXT_ALIGN_CENTER;
      lv_draw_label(layer, &label_dsc, &area_x);
    }

    // --- TEMPERATURE VALUES ABOVE THE POINTS ---
    lv_chart_series_t *ser = lv_chart_get_series_next(obj, NULL);
    if (ser)
    {
      for (int i = 0; i < num_points; i++)
      {
        lv_point_t pt;
        lv_chart_get_point_pos_by_id(obj, ser, i, &pt);

        // --- Formatting float ---
        char buf[16];
        float val = y_values[i] / 10.0f;
        int w = (int)val;
        int f = (int)((fabs(val - w) * 10) + 0.5f); // we take fabs for the fractional part
        sprintf(buf, "%d.%d", w, f);

        lv_area_t area_v;
        // Position over point
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

weather_chart_t *create_weather_chart(lv_obj_t *parent)
{
  weather_chart_t *wc = (weather_chart_t *)lv_malloc(sizeof(weather_chart_t));

  wc->chart = lv_chart_create(parent);
  lv_obj_set_size(wc->chart, 220, 150);
  // lv_obj_center(wc->chart);

  lv_chart_set_type(wc->chart, LV_CHART_TYPE_SCATTER);
  lv_chart_set_range(wc->chart, LV_CHART_AXIS_PRIMARY_Y, -40, 50);
  lv_chart_set_range(wc->chart, LV_CHART_AXIS_PRIMARY_X, 0, 21);

  // lv_obj_set_style_bg_color(wc->chart, COLOR_BG_DARK, 0);
  lv_obj_set_style_bg_opa(wc->chart, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(wc->chart, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(wc->chart, 0, 0);

  lv_obj_set_style_pad_left(wc->chart, 15, 0);
  lv_obj_set_style_pad_right(wc->chart, 23, 0);
  lv_obj_set_style_pad_bottom(wc->chart, 35, 0);
  lv_obj_set_style_clip_corner(wc->chart, false, 0);

  lv_obj_set_style_size(wc->chart, 5, 5, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(wc->chart, 1, LV_PART_ITEMS);
  lv_chart_set_div_line_count(wc->chart, 0, 0);

  wc->series = lv_chart_add_series(
      wc->chart,
      CYAN,
      LV_CHART_AXIS_PRIMARY_Y);

  wc->event_data = (chart_event_data_t *)lv_malloc(sizeof(chart_event_data_t));
  wc->event_data->num_points = 0;
  wc->event_data->y_values = NULL;

  lv_obj_add_event_cb(wc->chart, chart_event_cb, LV_EVENT_ALL, wc->event_data);

  return wc;
}

void set_weather_chart_data(weather_chart_t *wc,
                            int32_t *x_vals,
                            const float *y_vals,
                            uint16_t count)
{
  if (!wc || !wc->chart || !wc->series)
    return;

  // We set the number of points
  lv_chart_set_point_count(wc->chart, count);

  for (int i = 0; i < 8; i++)
  {
    y_vals_int[i] = (int32_t)(y_vals[i] * 10 + 0.5f); // rounding for lv_chart_set_ext_y_array
  }

  // Binding data
  lv_chart_set_ext_x_array(wc->chart, wc->series, x_vals);
  lv_chart_set_ext_y_array(wc->chart, wc->series, y_vals_int);

  // --- Update user_data inside the wrapper ---
  if (wc->event_data)
  {
    wc->event_data->num_points = count;
    wc->event_data->y_values = y_vals_int;
  }

  // Updating the schedule
  lv_chart_refresh(wc->chart);
}