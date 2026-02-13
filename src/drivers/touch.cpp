#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <lvgl.h>

#define TOUCH_CS  21
#define TOUCH_IRQ 27

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

static XPT2046_Touchscreen touchscreen(TOUCH_CS, TOUCH_IRQ);
static lv_indev_t * indev;

void touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (touchscreen.touched())
    {
        TS_Point p = touchscreen.getPoint();

				// ROTATION_90: swap and invert Y
				int x = map(p.y, 3800, 200, 0, SCREEN_WIDTH);	 // TFT WIDTH
				int y = map(p.x, 200, 3800, 0, SCREEN_HEIGHT); // TFT HEIGHT

				// screenX = (RawX - RawX_min) * SCREEN_WIDTH / (RawX_max - RawX_min)
				// screenY = (RawY - RawY_min) * SCREEN_HEIGHT / (RawY_max - RawY_min)

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

void touch_init(void)
{
    touchscreen.begin();
    touchscreen.setRotation(1);

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read);
}
