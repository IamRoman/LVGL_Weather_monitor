#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <XPT2046_Touchscreen.h>

#define TOUCH_CS 21
#define TOUCH_IRQ 27

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  480
#define TFT_ROTATION  LV_DISPLAY_ROTATION_90

TFT_eSPI tft = TFT_eSPI();

int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];


XPT2046_Touchscreen touchscreen(TOUCH_CS, TOUCH_IRQ);


// ===== Touch callback =====
void touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (touchscreen.touched())
    {
        TS_Point p = touchscreen.getPoint();

				// ROTATION_90: swap and invert Y
				int x = map(p.y, 3800, 200, 0, SCREEN_WIDTH);	 // TFT WIDTH
				int y = map(p.x, 200, 3800, 0, SCREEN_HEIGHT); // TFT HEIGHT

				// limitations within the screen
				if (x < 0) x = 0; if (x > SCREEN_WIDTH) x = SCREEN_WIDTH;
        if (y < 0) y = 0; if (y > SCREEN_HEIGHT) y = SCREEN_HEIGHT;

        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;

				// Serial.print("Touch X="); Serial.print(x);
				// Serial.print(" Y="); Serial.println(y);
		}
		else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// ===== Create UI =====
void create_ui()
{
	lv_obj_t *screen = lv_disp_get_scr_act(NULL);

	lv_obj_t *label = lv_label_create(screen);
	lv_label_set_text(label, "Hello world!!!");
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t *btn = lv_btn_create(screen);
	lv_obj_set_size(btn, 200, 60);
	lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);

	lv_obj_add_event_cb(btn, [](lv_event_t *e) {
		lv_event_code_t code = lv_event_get_code(e);
		if (code == LV_EVENT_CLICKED) {
			Serial.println("Button pressed!");
		}
	}, LV_EVENT_ALL, NULL);
}

void setup()
{
    Serial.begin(115200);

    touchscreen.begin();
    touchscreen.setRotation(1);

    lv_init();

    // ==== Create display with TFT_eSPI helper ====
		lv_display_t * disp;
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
		lv_display_set_rotation(disp, TFT_ROTATION);

		// ==== Add touch input ====
		lv_indev_t *indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
		lv_indev_set_read_cb(indev, touch_read);

		create_ui();
}

void loop()
{
		lv_task_handler();
    lv_tick_inc(5);
    delay(5);
}
