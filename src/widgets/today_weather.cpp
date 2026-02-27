#include "today_weather.h"
#include "weather_icons.h"
#include <theme/colors.h>

lv_obj_t * today_weather_create(lv_obj_t * parent)
{
  lv_obj_t * cont = lv_obj_create(parent);

  lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(cont,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_set_style_pad_all(cont, 0, 0);
  lv_obj_set_style_border_width(cont, 0, 0);
  lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

  weather_widget_t * w =
      (weather_widget_t *)lv_malloc(sizeof(weather_widget_t));

  lv_obj_set_user_data(cont, w);

  const char *iconCode = "03d"; // weatherData.icon;
  w->icon = lv_image_create(parent);
  lv_img_set_src(w->icon, getWeatherIcon(iconCode));
  lv_obj_set_style_transform_scale(w->icon, 256, LV_PART_MAIN);
  lv_obj_align(w->icon, LV_ALIGN_TOP_LEFT, 10, 0);

  static lv_style_t style_primary_label;
  lv_style_init(&style_primary_label);
  lv_style_set_text_color(&style_primary_label, LIGHT_BLUE);
  lv_style_set_text_font(&style_primary_label, &lv_font_montserrat_34);

  static lv_style_t style_secondary_label;
  lv_style_init(&style_secondary_label);
  lv_style_set_text_color(&style_secondary_label, WHITE);
  lv_style_set_text_font(&style_secondary_label, &lv_font_montserrat_14);

  /* Температура */
  w->label_temp = lv_label_create(cont);
  lv_obj_add_style(w->label_temp, &style_primary_label, 0);

  /* Вологість */
  w->label_hum = lv_label_create(cont);
  lv_obj_add_style(w->label_hum, &style_secondary_label, 0);

  /* Тиск */
  w->label_press = lv_label_create(cont);
  lv_obj_add_style(w->label_press, &style_secondary_label, 0);

  return cont;
}

void today_weather_set_data(lv_obj_t * widget,
                             const WeatherData &data)
{
  weather_widget_t * w =
      (weather_widget_t *)lv_obj_get_user_data(widget);

  Serial.println("Dashboard");
  Serial.println(data.temperature);

  if(!w) return;

  char buf[16];
  sprintf(buf, "%.2f°C", data.temperature);
  lv_label_set_text(w->label_temp, buf);

  lv_label_set_text_fmt(w->label_hum,
                        "Hum: %d%%",
                        data.humidity);

  lv_label_set_text_fmt(w->label_press,
                        "P: %d hPa",
                        data.pressure);
}
