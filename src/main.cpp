#include "DHT.h"
#define DHTTYPE DHT22

uint8_t DHTPin = D2;

int timeSinceLastRead = 0;

DHT dht(DHTPin, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    dht.begin();

    Serial.println("Device Started");
    Serial.println("-------------------------------------");
    Serial.println("Running DHT!");
    Serial.println("-------------------------------------");
}

void loop()
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

        float hif = dht.computeHeatIndex(f, h);
        float hic = dht.computeHeatIndex(t, h, false);

        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" *C ");
        Serial.print(f);
        Serial.print(" *F\t");
        Serial.print("Heat index: ");
        Serial.print(hic);
        Serial.print(" *C ");
        Serial.print(hif);
        Serial.println(" *F");

        timeSinceLastRead = 0;
    }
    delay(100);
    timeSinceLastRead += 100;
}