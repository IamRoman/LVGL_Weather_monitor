#include "ui.h"
#include "screen_dashboard.h"

void ui_init(void)
{
  screen_dashboard_init();
  lv_scr_load(screen_dashboard_get());
}
