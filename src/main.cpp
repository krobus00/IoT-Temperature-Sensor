#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <config.h>

#define DHTTYPE DHT22

// PIN DEFINE
uint8_t DHTPin = D3;
DHT dht(DHTPin, DHTTYPE);

int timeSinceLastRead = 0;

String BASE = BASE_URL;
String ACCESS_TOKEN = NODE_ACCESS_TOKEN;

HTTPClient http;
WiFiClientSecure client;

void setupWIFI()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print("*");
    }
    Serial.println("");
    Serial.println("WiFi connection Successful");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    setupWIFI();
    dht.begin();

    Serial.println("Device Started");
    Serial.println("------------------");
    Serial.println("Running node!");
    Serial.println("------------------");
}

void postData(float &humidity, float &temperature, float &heatIndex)
{
    String endpoint = BASE + "/sensors";

    client.setFingerprint(FINGERPRINT);
    client.connect(HOST, PORT);
    http.begin(client, endpoint);

    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.addHeader("Content-Type", "application/json", false, true);
    http.addHeader("Authorization", "Bearer " + ACCESS_TOKEN, false, true);

    String jsonparams;
    StaticJsonDocument<200> buff;

    buff["humidity"] = humidity;
    buff["temperature"] = temperature;
    buff["heatIndex"] = heatIndex;

    serializeJson(buff, jsonparams);

    http.POST(jsonparams);

    buff.clear();
    client.stop();
    http.end();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }
    if (timeSinceLastRead < 2000)
    {
        delay(100);
        timeSinceLastRead += 100;
        return;
    }

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        timeSinceLastRead = 0;
        return;
    }

    float hic = dht.computeHeatIndex(t, h, false);

    postData(h, t, hic);
    timeSinceLastRead = 0;
}