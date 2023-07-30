#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

#define DHTPIN 4     // esp8266 D2핀 번호
#define DHTTYPE DHT11   // DHT 센서

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "jindodog";
const char* password = "a!1726354";

IPAddress staticIP(192, 168, 0, 39);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

ESP8266WebServer server(80);

WiFiClient client;

void setup() 
{
  Serial.begin(115200);
  Serial.setTimeout(2000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, subnet, gateway, dns);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.print(WiFi.localIP());
  server.begin();
  // Wait for serial to initialize.
  while(!Serial)
  {

  }
  
  dht.begin();

  Serial.println("START DHT!");

}

int timeSinceLastRead = 0;
void loop() 
{

  //2초마다 데이터 전송
  if(timeSinceLastRead > 2000) 
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
    Serial.println(" *C ");

    if (isnan(h) || isnan(t)) 
    {
      server.send(500, "text/plain", "Failed to read from DHT sensor!");
      return;
    }

    String data = "Humidity: " + String(h) + " %\t" + "Temperature: " + String(t) + " *C";
    server.send(200, "text/plain", data);

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;

  server.handleClient();
}