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
HABinarySensor On("On");
HABinarySensor DayMode("DayMode");

HANumber Bright("Bright");
HANumber Color("Color");
HASensorNumber Battery("Battery");

ESPRotary r(D5,D6,2);
Button2 b;
void rotate(ESPRotary& r); 
void click(Button2& btn) ;
void dblclick(Button2& btn) ;
void onNumberCommand(HANumeric number, HANumber* sender);

int Col = 0 , Bri = 0;

unsigned int raw=0;
float volt=0.0;

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
  b.setClickHandler(click);
  b.setLongClickHandler(click);
  b.setDoubleClickHandler(dblclick);
  Serial.println("Encoder connected");

  EEPROM.begin(20);

  bool OnTmp = 0, ModeTmp = 0;
  EEPROM.get(0, Bri);
  delay(10);
  EEPROM.get(4, Col);
  delay(10);
  EEPROM.get(8, ModeTmp);
  delay(10);
  DayMode.setState(ModeTmp);
  EEPROM.get(12, OnTmp);
  delay(10);
  On.setState(OnTmp);

  Serial.println(Bri);
  Serial.println(Col);
  Serial.println(OnTmp);
  Serial.println(ModeTmp);

  Bright.setState(Bri);
  Bright.setIcon("mdi:brightness-6");
  Bright.setName("Bright");
  Bright.onCommand(onNumberCommand);

  Color.setState(Col);
  Color.setIcon("mdi:palette-outline");
  Color.setName("Color");
  Color.onCommand(onNumberCommand);

  Battery.setIcon("mdi:battery");
  Battery.setName("Battery");
  //Batery.onCommand(onNumberCommand);

  On.setCurrentState(OnTmp); 
  On.setName("On"); 
  On.setIcon("mdi:button-pointer");

  DayMode.setCurrentState(ModeTmp); 
  DayMode.setName("DayMode"); 
  DayMode.setIcon("mdi:toggle-switch-off-outline");

  pinMode(A0, INPUT);
  raw = analogRead(A0);
  volt=raw/1023.0;
  volt=volt*4.2;
  Battery.setValue(volt);
  mqtt.loop();
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

    Bright.setState(Bri);
    EEPROM.put(0, Bri);
  }
  else
  {
    if(r.getDirection() == rotary_direction::left)
      Col+=r.getIncrement()*2;
    else
      Col-=r.getIncrement()*2;

    if(Col > 360) Col = 0;
    if(Col < 0) Col = 0;

    Color.setState(Col);
    EEPROM.put(4, Col);
  }
  
  mqtt.loop();
  EEPROM.commit();
}

void click(Button2& btn) {
  On.setState(!On.getCurrentState());
  EEPROM.put(12, On.getCurrentState());
  raw = analogRead(A0);
  volt=raw/10.230;
  //volt=volt*4.2;
  Battery.setValue(volt);
  mqtt.loop();
  EEPROM.commit();
}

void dblclick(Button2& btn){
  DayMode.setState(!DayMode.getCurrentState());
  EEPROM.put(8, DayMode.getCurrentState());
  raw = analogRead(A0);
  volt=raw/10.230;
  //volt=volt*4.2;
  Battery.setValue(volt);
  mqtt.loop();
  EEPROM.commit();
}

void onNumberCommand(HANumeric number, HANumber* sender)
{
  if (sender == &Bright)
    Bri = number.toUInt32();

  if (sender == &Color)
    Col = number.toUInt32();

  sender->setState(number);
  Bright.setState(Bri);
  Color.setState(Col);

 mqtt.loop();
}

void loop() {
  ArduinoOTA.handle();

  r.loop();
  b.loop();

  static long long del= millis();
  if (millis() - del > 1e4)
  {
    raw = analogRead(A0);
    volt=raw/10.230;
    //volt=volt*4.2;
    Battery.setValue(volt);

    del = millis();
    mqtt.loop();
  }
}