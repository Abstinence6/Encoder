#include <Arduino.h>
#include <ArduinoHA.h>

#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>

#include "ArduinoOTA.h"

#include "ESPRotary.h"
#include "Button2.h"

const char* ssid = "Mi Wi-Fi";
const char* password = "12345678";
byte mac[] = {0x40, 0xf5, 0x20, 0x33, 0x82, 0xef};

WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

#define BROKER_ADDR     IPAddress(192,168,1,159)
HABinarySensor Button("Button");
HABinarySensor Mode("Mode");
HASensorNumber Bright("Bright");
HASensorNumber Color("Color");

ESPRotary r(D5,D6,2);
Button2 b;
void rotate(ESPRotary& r); 
void click(Button2& btn) ;
void dblclick(Button2& btn) ;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.print("\nConnecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  device.setName("Dimmer");
  device.setSoftwareVersion("1.0.0");
  device.enableSharedAvailability();
  device.enableLastWill();

  Serial.println("\nWiFi connected");
  Serial.println("\nIP address: ");
  Serial.println(WiFi.localIP());

  mqtt.begin(BROKER_ADDR,1883,"HAMQTT","gisterezis");
  Serial.println("\nMQTT connected");

  ArduinoOTA.onStart([]() {
    Serial.println("\nOTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("\nOTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\nOTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("\nOTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("\nOTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("\nOTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("\nOTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("\nOTA End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("\nOTA Ready");

  r.begin(D5,D6,2);
  r.setChangedHandler(rotate);

  b.begin(D7);
  b.setChangedHandler(click);
  b.setDoubleClickHandler(dblclick);
  Serial.println("Encoder connected");

  Bright.setValue(0);
  Bright.setIcon("mdi:brightness-6");
  Bright.setName("Bright");

  Color.setValue(0);
  Color.setIcon("mdi:palette-outline");
  Color.setName("Color");

  Button.setCurrentState(0); 
  Button.setName("Button"); 
  Button.setIcon("mdi:button-pointer");

  Mode.setCurrentState(0); 
  Mode.setName("Mode"); 
  Mode.setIcon("mdi:toggle-switch-off-outline");
}

void rotate(ESPRotary& r) {
  if(!b.isPressed())
  {
    if(r.getDirection() == rotary_direction::left)
      Bright.setValue(r.getIncrement());
    else
      Bright.setValue(-r.getIncrement()); 
  }
  else
  {
    if(r.getDirection() == rotary_direction::left)
      Color.setValue(r.getIncrement());
    else
      Color.setValue(-r.getIncrement());
  }
  
  mqtt.loop();
  Bright.setValue(0);
  Color.setValue(0);
}

void click(Button2& btn) {
  Button.setState(b.isPressed());
  mqtt.loop();
}

void dblclick(Button2& btn){
  Mode.setState(!Mode.getCurrentState());
  mqtt.loop();
}

void loop() {
  ArduinoOTA.handle();

  r.loop();
  b.loop();

  static long long del= millis();
  if (millis() - del > 1e4)
  {
    del = millis();
    mqtt.loop();
  }
}

/*
#include <Arduino.h>
#include <ArduinoHA.h>

#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>

#include "ArduinoOTA.h"

#include "ESPRotary.h"
#include "Button2.h"

#include "EEPROM.h"

const char* ssid = "Mi Wi-Fi";
const char* password = "12345678";
byte mac[] = {0x40, 0xf5, 0x20, 0x33, 0x82, 0xef};

WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

#define BROKER_ADDR     IPAddress(192,168,1,159)
HABinarySensor Button("Button");
HABinarySensor Mode("Mode");
HASensorNumber Bright("Bright");
HASensorNumber Color("Color");

ESPRotary r(D5,D6,2);
Button2 b;
void rotate(ESPRotary& r); 
void click(Button2& btn) ;
void dblclick(Button2& btn) ;

int Col = 0 , Bri = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.print("\nConnecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  device.setName("Dimmer");
  device.setSoftwareVersion("1.0.0");
  device.enableSharedAvailability();
  device.enableLastWill();

  Serial.println("\nWiFi connected");
  Serial.println("\nIP address: ");
  Serial.println(WiFi.localIP());

  mqtt.begin(BROKER_ADDR,1883,"HAMQTT","gisterezis");
  Serial.println("\nMQTT connected");

  ArduinoOTA.onStart([]() {
    Serial.println("\nOTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("\nOTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\nOTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("\nOTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("\nOTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("\nOTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("\nOTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("\nOTA End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("\nOTA Ready");

  r.begin(D5,D6,2);
  r.setChangedHandler(rotate);

  b.begin(D7);
  b.setChangedHandler(click);
  b.setDoubleClickHandler(dblclick);
  Serial.println("Encoder connected");

  EEPROM.begin(16);

  EEPROM.get(0, Bri);
  delay(10);
  EEPROM.get(4, Col);
  delay(10);
  int tmp = 0;
  EEPROM.get(8, tmp);
  delay(10);
  Mode.setState(tmp);

  Serial.println(Bri);
  Serial.println(Col);
  Serial.println(tmp);

  Bright.setValue(Bri);
  Bright.setIcon("mdi:brightness-6");
  Bright.setName("Bright");

  Color.setValue(Col);
  Color.setIcon("mdi:palette-outline");
  Color.setName("Color");

  Button.setCurrentState(0); 
  Button.setName("Button"); 
  Button.setIcon("mdi:button-pointer");

  Mode.setCurrentState(tmp); 
  Mode.setName("Mode"); 
  Mode.setIcon("mdi:toggle-switch-off-outline");
}

void rotate(ESPRotary& r) {
  if(!b.isPressed())
  {
    if(r.getDirection() == rotary_direction::left)
      Bri+=r.getIncrement();
    else
      Bri-=r.getIncrement();

    if(Bri > 100) Bri = 100;
    if(Bri < 0) Bri = 0;

    Bright.setValue(Bri);
    EEPROM.put(0, Bri);
  }
  else
  {
    if(r.getDirection() == rotary_direction::left)
      Col+=r.getIncrement();
    else
      Col-=r.getIncrement();

    if(Col > 360) Col = 0;
    if(Col < 0) Col = 0;

    Color.setValue(Col);
    EEPROM.put(4, Col);
  }
  
  mqtt.loop();
  EEPROM.commit();
}

void click(Button2& btn) {
  Button.setState(b.isPressed());
  mqtt.loop();
}

void dblclick(Button2& btn){
  Mode.setState(!Mode.getCurrentState());
  EEPROM.put(8, Mode.getCurrentState());
  mqtt.loop();
  EEPROM.commit();
}

void loop() {
  ArduinoOTA.handle();

  r.loop();
  b.loop();

  static long long del= millis();
  if (millis() - del > 1e4)
  {
    del = millis();
    mqtt.loop();
  }
}
*/