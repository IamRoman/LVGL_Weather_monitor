#include <Arduino.h>
#include <lvgl.h>

#include "ui.h"

extern void display_init(void);
extern void touch_init(void);

void setup()
{
    Serial.begin(115200);

		lv_init();

		display_init();
		touch_init();

		ui_init();
}

void loop()
{
	lv_task_handler();
	lv_tick_inc(5);
	delay(5);
}
