#include "ui.h"
#include "screens/screen_home.h"

void ui_init(void)
{
  screen_home_init();
  lv_scr_load(screen_home_get());
}
