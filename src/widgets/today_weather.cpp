#include "today_weather.h"
#include "weather_icons.h"
#include <theme/colors.h>

static void clean(lv_obj_t *obj)
{
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

void set_pressure_color(lv_obj_t *label, int pressure_hPa)
{
  lv_color_t color;

  if (pressure_hPa < 1050)
    color = GREEN;
  else if (pressure_hPa < 1090)
    color = YELLOW;
  else
    color = RED;

  lv_obj_set_style_text_color(label, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

lv_obj_t * today_weather_create(lv_obj_t * parent)
{
  lv_obj_t *main_cont = lv_obj_create(parent);
  lv_obj_set_size(main_cont, LV_PCT(100), LV_PCT(100));
  clean(main_cont);

  lv_obj_t *top_cont = lv_obj_create(main_cont);
  clean(top_cont);
  lv_obj_set_size(top_cont, LV_PCT(100), LV_PCT(10));
  lv_obj_align(top_cont, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *left_cont = lv_obj_create(main_cont);
  clean(left_cont);
  lv_obj_set_size(left_cont, LV_PCT(40), LV_PCT(90));
  lv_obj_align(left_cont, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_t *right_cont = lv_obj_create(main_cont);
  clean(right_cont);
  lv_obj_set_size(right_cont, LV_PCT(60), LV_PCT(90));
  lv_obj_align(right_cont, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

  // --- Віджет погоди ---
  weather_widget_t *w = (weather_widget_t *)lv_malloc(sizeof(weather_widget_t));
  lv_obj_set_user_data(main_cont, w);

  // --- Стилі ---
  static lv_style_t style_primary_label;
  lv_style_init(&style_primary_label);
  lv_style_set_text_color(&style_primary_label, LIGHT_BLUE);
  lv_style_set_text_font(&style_primary_label, &lv_font_montserrat_34);

  static lv_style_t style_secondary_label;
  lv_style_init(&style_secondary_label);
  lv_style_set_text_color(&style_secondary_label, WHITE);
  lv_style_set_text_font(&style_secondary_label, &lv_font_montserrat_14);

  // --- Icon ---
  const char *iconCode = "03d";
  w->icon = lv_image_create(left_cont);
  lv_img_set_src(w->icon, getWeatherIcon(iconCode));
  lv_obj_align(w->icon, LV_ALIGN_TOP_LEFT, 10, 0);

  // --- Description ---
  w->description = lv_label_create(top_cont);
  lv_obj_add_style(w->description, &style_secondary_label, 0);
  lv_obj_align(w->description, LV_ALIGN_CENTER, 0, 0);

  // --- Wind ---
  w->wind_data = lv_label_create(left_cont);
  lv_obj_add_style(w->wind_data, &style_secondary_label, 0);
  lv_obj_align(w->wind_data, LV_ALIGN_CENTER, 10, 20);

  // --- Temperature ---
  w->label_temp = lv_label_create(right_cont);
  lv_obj_add_style(w->label_temp, &style_primary_label, 0);
  lv_obj_align(w->label_temp, LV_ALIGN_TOP_MID, 0, 10);

  // --- Humidity ---
  w->label_hum = lv_label_create(right_cont);
  lv_obj_add_style(w->label_hum, &style_secondary_label, 0);
  lv_obj_align(w->label_hum, LV_ALIGN_CENTER, 0, 20);

  // --- Pressure ---
  w->label_press = lv_label_create(right_cont);
  lv_obj_add_style(w->label_press, &style_secondary_label, 0);
  lv_obj_align(w->label_press, LV_ALIGN_CENTER, 0, 40);

  return main_cont;
}

void today_weather_set_data(lv_obj_t * widget,
                             const WeatherData &data)
{
  weather_widget_t * w =
      (weather_widget_t *)lv_obj_get_user_data(widget);

  if(!w) return;

  lv_img_set_src(w->icon, getWeatherIcon(data.icon));

  char buf[16];
  sprintf(buf, "%.2f°C", data.temperature);
  lv_label_set_text(w->label_temp, buf);

  char description_buf[16];
  sprintf(description_buf, "%s", data.description);
  lv_label_set_text(w->description, description_buf);

  char wind_buf[20];
  snprintf(wind_buf, sizeof(wind_buf),
           "%.1f m/s %s",
           data.wind_speed,
           data.wind_dir);

  lv_label_set_text(w->wind_data, wind_buf);

  lv_label_set_text_fmt(w->label_hum,
                        "Hum: %d%%",
                        data.humidity);

  lv_label_set_text_fmt(w->label_press,
                        "P: %d hPa",
                        data.pressure);
  set_pressure_color(w->label_press, data.pressure);
}
