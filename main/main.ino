//TODO: Write err code for display when no busses to display
//TODO: Add code to sleep if it's in the middle of the night. 



int brightWait = 5; //Time to wait (in ms) before turning on and off LED
int refreshTime = 120; // Time in seconds to wait between fetching data
int powerPins[4] = {15, 13, 12, 14};
int powerPinCount = 4;
//2D array to convert numbers to segment bits
//I wired my display in order of a,f,b,g,c,dp,d,e
//int ALPHA_CODE[] = { , //00000110 A
//                     , //10010100 B
//                     , //00100011 C
//                     , //11000100 D
//                     , //00101100 E
//                   }
int ANIMATE[] = { // letter corresponds to section on 7 segment display
  254, // a 01111111
  251, // b 11011111
  239, // c 11110111
  191, // d 11111101
  127, // e 11111110
  253,   // f 10111111
};
int ANIMATE_COUNT = 6;

int ERR_CODE[] = {
  255, // Blank
  52, // E
  119, // r
  119, // r
};
int ERR_COUNT = 4;

int NUM_CODE[] = {40, //0
                  235, //1
                  50, //2
                  162, //3
                  225, //4
                  164, //5
                  36, //6
                  234, //7
                  32, //8
                  160, //9
                  255, //BLANK
                 };
int NUMDOT_CODE[] = {8, //0
                     203, //1
                     18, //2
                     130, //3
                     193, //4
                     132, //5
                     4, //6
                     202, //7
                     0, //8
                     128, //9
                    };


//Pin connected to ST_CP of 74HC595
int latchPin = 5;
//Pin connected to SH_CP of 74HC595
int clockPin = 16;
////Pin connected to DS of 74HC595
int dataPin = 4;
int infoArray[3] = {0, 0, 0};

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>



#define LED_PIN    2
#define NUM_LEDS   4

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;


CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  setLights(0,0,0,0);
  USE_SERIAL.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("BaseHub-Devices", "Lorenco1995");

  pinMode(latchPin, OUTPUT);

  for (int thisPin = 0; thisPin < powerPinCount; thisPin++) {
    Serial.printf("Setting power pin %d\n", powerPins[thisPin]);
    pinMode(powerPins[thisPin], OUTPUT);
    delay(100);
    digitalWrite(powerPins[thisPin], LOW);
    delay(100);
  }

  

}

void loop() {

  unsigned long startMillis = millis();
  getBusInfo(infoArray);
  if (infoArray[0] == 0) {
    delay(1000);
    return;
  }
  updateLights((infoArray[1] - infoArray[2]), 0.5); //1 is full brightness, 0.5 is half, etc. 
  while (true) {
    showRouteId(infoArray[0], 5);
    showTime((infoArray[1] - infoArray[2]), 10);
    if (((millis() - startMillis) / 1000) > refreshTime) {
      break;
    }
  }
  

}

void getBusInfo(int *info) {
  //
  while (true) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {

      HTTPClient http;
      // configure traged server and url
      http.begin("http://nwoodthorpe.com/grt/V2/livetime.php?stop=2512"); //HTTP
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
          //        if (root["ERROR_CODE"] == 23) {
          //          http.end();
          //          showError();
          //          return;
          //        }
          int routeId = root["data"][0]["routeId"];
          int arrivalTime = root["data"][0]["stopDetails"][0]["departure"];
          int curTime = root["data"][0]["time"];
          info[0] = routeId;
          info[1] = arrivalTime;
          info[2] = curTime;
          USE_SERIAL.printf("Route %d ", routeId);
          USE_SERIAL.printf("coming in %d seconds\n",  (arrivalTime - curTime));
          USE_SERIAL.printf("Coming at t= %d \n", arrivalTime);
          USE_SERIAL.printf("Current time= %d \n", arrivalTime);

        }
      }

      http.end();
      return;
    }
    delay(100);
  }
}

void setLights(int red, int green, int blue, float brightness) {
  Serial.printf("Light brightness set to %0.2f % \n", brightness);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(red * brightness, green * brightness , blue * brightness);
  }
  FastLED.show();
}

void updateLights(int timeInSecs, int brightness) {
  if (timeInSecs < 300) {
    if (timeInSecs < 180) {
      setLights(255, 0, 0, brightness);
    } else {
      setLights(247, 168, 71, brightness);
    }
  } else {
    setLights(0, 255, 0, brightness);
  }
}
void displayError(int i) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, ERR_CODE[i]);
  digitalWrite(latchPin, HIGH);
}

void displayDigit(int digit) {
  //  Set segment pins based on array
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, NUM_CODE[digit]);
  digitalWrite(latchPin, HIGH);
  //My common Anode display activates the segments on LOW
}

void displayDotDigit(int digit) {
  //  Set segment pins based on array
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, NUMDOT_CODE[digit]);
  digitalWrite(latchPin, HIGH);
  //My common Anode display activates the segments on LOW
}
void displayAnimation(int part) {
  //  Set segment pins based on array
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, ANIMATE[part]);
  digitalWrite(latchPin, HIGH);
  //My common Anode display activates the segments on LOW
}

void showError() {
  unsigned long startMillis = millis();
  while (true) {
    for (int i = 0; i < ERR_COUNT; i++) {
      displayError(i);
      digitalWrite(powerPins[i], HIGH);
      delay(brightWait);
      digitalWrite(powerPins[i], LOW);
    }
    if (((millis() - startMillis) / 1000) > 5) {
      break;
    }
  }
}
void showTime(int timeInSecs, int dispTime) {
  // Takes current time until arrival in seconds, converts to MM.SS, then invokes display function
  unsigned long startMillis = millis();
  int mins;
  int secs;
  int curSecs;
  while (true) {
    curSecs = timeInSecs - ((millis() - startMillis) / 1000);
    //    Serial.printf("curSecs = %d \t| curMillis = %d", curSecs, millis());
    mins = curSecs / 60;
    secs = curSecs % 60;
    // Run display for period of time before
    displayDigit((mins / 10) % 10);
    digitalWrite(powerPins[0], HIGH);
    delay(brightWait);
    digitalWrite(powerPins[0], LOW);
    displayDotDigit(mins % 10);
    digitalWrite(powerPins[1], HIGH);
    delay(brightWait);
    digitalWrite(powerPins[1], LOW);
    displayDigit((secs / 10) % 10);
    digitalWrite(powerPins[2], HIGH);
    delay(brightWait);
    digitalWrite(powerPins[2], LOW);
    displayDigit(secs % 10);
    digitalWrite(powerPins[3], HIGH);
    delay(brightWait);
    digitalWrite(powerPins[3], LOW);

    if (((millis() - startMillis) / 1000) > dispTime) {
      break;
    }
  }
}

void showRouteId(int routeId, int dispTime) {
  unsigned long startMillis = millis();
  if (routeId < 100) {
    if (routeId < 10) {
      while (true) {
        for (int i = 0; i < ANIMATE_COUNT; i++) {
          for (int j = 0; j < 5; j++) {

            displayAnimation(i);
            digitalWrite(powerPins[0], HIGH);
            delay(brightWait);
            digitalWrite(powerPins[0], LOW);

            displayDigit(10);
            digitalWrite(powerPins[1], HIGH);
            delay(brightWait);
            digitalWrite(powerPins[1], LOW);

            displayDigit(10);
            digitalWrite(powerPins[2], HIGH);
            delay(brightWait);
            digitalWrite(powerPins[2], LOW);

            displayDigit(routeId);
            digitalWrite(powerPins[3], HIGH);
            delay(brightWait);
            digitalWrite(powerPins[3], LOW);
          }
        }
        if (((millis() - startMillis) / 1000) > dispTime) {
          break;
        }
      }
      return;
    }
    // Route number >=10 && < 100
    while (true) {
      for (int i = 0; i < ANIMATE_COUNT; i++) {
        for (int j = 0; j < 5; j++) {

          displayAnimation(i);
          digitalWrite(powerPins[0], HIGH);
          delay(brightWait);
          digitalWrite(powerPins[0], LOW);

          displayDigit(10);
          digitalWrite(powerPins[1], HIGH);
          delay(brightWait);
          digitalWrite(powerPins[1], LOW);

          displayDigit((routeId / 10) % 10);
          digitalWrite(powerPins[2], HIGH);
          delay(brightWait);
          digitalWrite(powerPins[2], LOW);

          displayDigit(routeId % 10);
          digitalWrite(powerPins[3], HIGH);
          delay(brightWait);
          digitalWrite(powerPins[3], LOW);
        }
      }
      if (((millis() - startMillis) / 1000) > dispTime) {
        break;
      }
    }
    return;
  }
  // routeId >=100
  while (true) {
    for (int i = 0; i < ANIMATE_COUNT; i++) {
      for (int j = 0; j < 5; j++) {

        displayAnimation(i);
        digitalWrite(powerPins[0], HIGH);
        delay(brightWait);
        digitalWrite(powerPins[0], LOW);

        displayDigit((routeId / 100) % 10);
        digitalWrite(powerPins[1], HIGH);
        delay(brightWait);
        digitalWrite(powerPins[1], LOW);

        displayDigit((routeId / 10) % 10);
        digitalWrite(powerPins[2], HIGH);
        delay(brightWait);
        digitalWrite(powerPins[2], LOW);

        displayDigit(routeId % 10);
        digitalWrite(powerPins[3], HIGH);
        delay(brightWait);
        digitalWrite(powerPins[3], LOW);
      }
    }
    if (((millis() - startMillis) / 1000) > dispTime) {
      break;
    }
  }
  return;
}
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i = 0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
  for (i = 7; i >= 0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}

