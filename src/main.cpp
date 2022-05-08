#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <config.h>

#define DHTTYPE DHT22

// PIN DEFINE
uint8_t DHTPin = D3;
uint8_t LED = D0;
DHT dht(DHTPin, DHTTYPE);

int timeSinceLastRead = 0;

String BASE = BASE_URL;
String ACCESS_TOKEN = NODE_ACCESS_TOKEN;

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
    pinMode(LED, OUTPUT);
    setupWIFI();
    dht.begin();

    Serial.println("Device Started");
    Serial.println("------------------");
    Serial.println("Running node!");
    Serial.println("------------------");
}

void postData(float humidity, float temperature, float heatIndex)
{
    String endpoint = BASE + "/sensors";
    String res;
    HTTPClient http;
    WiFiClientSecure client;

    client.setFingerprint(FINGERPRINT);
    client.connect(HOST, PORT);

    http.begin(client, endpoint);

    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + ACCESS_TOKEN);

    String jsonparams;
    DynamicJsonDocument buff(1024);

    buff["humidity"] = humidity;
    buff["temperature"] = temperature;
    buff["heatIndex"] = heatIndex;

    serializeJson(buff, jsonparams);

    int httpCode = http.POST(jsonparams);
    if (httpCode == 200)
    {
        digitalWrite(LED, LOW);
        delay(100);
        digitalWrite(LED, HIGH);
        delay(100);
        timeSinceLastRead += 200;
    }
    http.end();
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {

        if (timeSinceLastRead > 2000)
        {
            float h = dht.readHumidity();
            float t = dht.readTemperature();
            float f = dht.readTemperature(true);

            if (isnan(h) || isnan(t) || isnan(f))
            {
                Serial.println("Failed to read from DHT sensor!");
                timeSinceLastRead = 0;
                return;
            }

            float hic = dht.computeHeatIndex(t, h, false);

            timeSinceLastRead = 0;
            postData(h, t, hic);
        }
        delay(100);
        timeSinceLastRead += 100;
    }
}