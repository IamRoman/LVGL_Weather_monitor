#include <Preferences.h>
#include "app_state.h"

static Preferences prefs;

void config_load()
{
  prefs.begin("weather", true);

  app.lat = prefs.getDouble("lat", 50.45466); // Kyiv by default
  app.lon = prefs.getDouble("lon", 30.5238);

  prefs.end();
}

void config_save()
{
  prefs.begin("weather", false);

  prefs.putDouble("lat", app.lat);
  prefs.putDouble("lon", app.lon);

  prefs.end();
}