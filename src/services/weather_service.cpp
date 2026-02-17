#include "weather_service.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

#define OWM_HOST "api.openweathermap.org"
#define OWM_PORT 443

// встав свій ключ
static const char* API_KEY = "d0e4132a4b555118a94675b394ef386f";
static const char* CITY = "Vinnytsia";
static const char* COUNTRY = "UA";

static void set_time_from_owm(time_t utc, int tz)
{
    time_t local = utc + tz;

    struct timeval tv;
    tv.tv_sec = local;
    tv.tv_usec = 0;

    settimeofday(&tv, NULL);
}

bool weather_update(WeatherData &data)
{
    WiFiClientSecure client;
    client.setInsecure(); // без сертифіката (економимо RAM)

    if (!client.connect(OWM_HOST, OWM_PORT))
        return false;

    String url = "/data/2.5/weather?q=";
    url += CITY;
    url += ",";
    url += COUNTRY;
    url += "&appid=";
    url += API_KEY;
    url += "&units=metric";

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + OWM_HOST + "\r\n" +
                 "Connection: close\r\n\r\n");

    // Пропускаємо HTTP заголовки
    while (client.connected())
    {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }

    // JSON filter — парсимо тільки потрібне
    StaticJsonDocument<256> filter;
    filter["main"]["temp"] = true;
    filter["main"]["humidity"] = true;
    filter["main"]["pressure"] = true;
    filter["weather"][0]["id"] = true;
    filter["dt"] = true;
    filter["timezone"] = true;

    StaticJsonDocument<512> doc;

    DeserializationError error =
        deserializeJson(doc, client, DeserializationOption::Filter(filter));

    if (error)
        return false;

    data.temperature = doc["main"]["temp"];
    data.humidity = doc["main"]["humidity"];
    data.pressure = doc["main"]["pressure"];
    data.weather_id = doc["weather"][0]["id"];

    time_t dt = doc["dt"];
    int timezone = doc["timezone"];

    set_time_from_owm(dt, timezone);

    client.stop();
    return true;
}
