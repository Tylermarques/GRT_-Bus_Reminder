
/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

void setup() {
  USE_SERIAL.begin(115200);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("BaseHub-Devices", "Lorenco1995");

}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin("http://nwoodthorpe.com/grt/V2/livetime.php?stop=2512"); //HTTP

    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
  
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
  
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
        DynamicJsonBuffer jsonBuffer(capacity);
        JsonObject& root = jsonBuffer.parseObject(payload);
        if (!root.success()) {
          Serial.println("parseObject() failed");
          return;
        };
        const char* route_id = root["data"][0]["routeId"];
        int coming_at = root["data"][0]["stopDetails"][0]["departure"];
        int cur_time = root["data"][0]["time"];
        displayRouteId(route_id.toInt(), 10); //dispTime of 10 ~= 6 secs real time
        displayTime((coming_at - cur_time));
        USE_SERIAL.printf("Route %s ", route_id);
        USE_SERIAL.printf("coming in %d seconds\n",  (coming_at - cur_time));
        USE_SERIAL.printf("Coming at t= %d \n", coming_at);
        USE_SERIAL.printf("Current time= %d \n", cur_time);
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}

