#include "today_weather.h"

typedef struct {
  lv_obj_t * label_temp;
  lv_obj_t * label_hum;
  lv_obj_t * label_press;
} weather_widget_t;

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
  lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

  weather_widget_t * w =
      (weather_widget_t *)lv_malloc(sizeof(weather_widget_t));

  lv_obj_set_user_data(cont, w);

  /* Температура */
  w->label_temp = lv_label_create(cont);
  lv_obj_set_style_text_font(w->label_temp,
                              &lv_font_montserrat_22,
                              0);

  /* Вологість */
  w->label_hum = lv_label_create(cont);

  /* Тиск */
  w->label_press = lv_label_create(cont);

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
                        "Humidity: %d%%",
                        data.humidity);

  lv_label_set_text_fmt(w->label_press,
                        "Pressure: %d hPa",
                        data.pressure);

  lv_obj_set_style_text_color(w->label_temp,
                                  lv_color_hex(0xFF4D4D), 0);
}
