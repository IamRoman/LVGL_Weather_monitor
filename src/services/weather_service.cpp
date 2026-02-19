#include "weather_service.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

#define OWM_HOST "api.openweathermap.org"
#define OWM_PORT 443

static const char *API_KEY = "d0e4132a4b555118a94675b394ef386f";
static const char* CITY = "Vinnytsia";
static const char* COUNTRY = "UA";

/* =========================
	 NTP INIT (один раз)
	 ========================= */
void weather_time_init()
{
	configTzTime("EET-2EEST,M3.5.0/3,M10.5.0/4",
							 "pool.ntp.org",
							 "time.nist.gov");

	struct tm timeinfo;
	int retry = 0;

	while (!getLocalTime(&timeinfo) && retry < 10)
	{
		delay(500);
		retry++;
	}
}

/* =========================
	 WEATHER UPDATE
	 ========================= */
bool weather_update(WeatherData &data)
{
	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("WiFi NOT connected");
		return false;
	}

	Serial.println("WiFi OK");

	WiFiClientSecure client;
	client.setInsecure();
	client.setTimeout(8000);

	Serial.println("Connecting to OWM...");

	if (!client.connect(OWM_HOST, OWM_PORT))
	{
		Serial.println("Connection FAILED");
		return false;
	}

	Serial.println("Connected!");

	String url = "/data/2.5/weather?q=";
	url += CITY;
	url += ",";
	url += COUNTRY;
	url += "&appid=";
	url += API_KEY;
	url += "&units=metric";

	Serial.println("Requesting:");
	Serial.println(url);

	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
							 "Host: " + OWM_HOST + "\r\n" +
							 "Connection: close\r\n\r\n");

	// HTTP статус
	String status = client.readStringUntil('\n');
	Serial.print("HTTP STATUS: ");
	Serial.println(status);

	if (!status.startsWith("HTTP/1.1 200"))
	{
		Serial.println("HTTP ERROR");
		client.stop();
		return false;
	}

	// Пропуск заголовків
	while (client.connected())
	{
		String line = client.readStringUntil('\n');
		if (line == "\r")
			break;
	}

	Serial.println("Reading JSON...");

	String payload = client.readString();
	Serial.println("RAW JSON:");
	Serial.println(payload);

	StaticJsonDocument<768> doc;

	DeserializationError error = deserializeJson(doc, payload);

	if (error)
	{
		Serial.print("JSON ERROR: ");
		Serial.println(error.c_str());
		client.stop();
		return false;
	}

	data.temperature = doc["main"]["temp"] | 0.0;
	data.humidity = doc["main"]["humidity"] | 0;
	data.pressure = doc["main"]["pressure"] | 0;
	data.weather_id = doc["weather"][0]["id"] | 0;

	Serial.println("Parsed OK!");
	Serial.println(data.temperature);
	Serial.println(data.humidity);
	Serial.println(data.pressure);

	client.stop();
	return true;
}

