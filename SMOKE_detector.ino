/*
 
 MIT License

 Copyright (c) 2021 a3v3

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 By HYPERARX
 aravindvallab@protonmail.com
 CHEERS! (>‿◠)✌

*/


//LED INDICATORS
#define redLed     13          //  RED LED                                     GPIO13 (D7)
#define greenLed   15          //  GREEN LED                                   GPIO15 (D8)
#define blueLed    14          //  BLUE LED                                    GPIO14 (D5)

//BUZZER
#define buzzer     16          //  BUZZER                                      GPIO16 (D0)

//SENSOR
#define smoke      A0          // SENSOR                                       ADC0 (A0)

//LIQUID CRYSTAL LIBRARIES
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//WIFI LIBRARIES
#include <ESP8266WiFi.h>                          // Include the Wi-Fi library.
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>                     // Include the Wi-Fi-Multi library.
#include <ESP8266mDNS.h>                          // Include the mDNS library
#include <ArduinoOTA.h>                           // Include OTA library. 

ESP8266WiFiMulti wifiMulti;                       // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'.

int sensorThres = 100;                            // Your threshold value

void setup() {
  
  pinMode(buzzer, OUTPUT);
  pinMode(smoke,  INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  
  Serial.begin(115200);                             // Start the Serial communication to send messages to the computer. Note: upload speed for arduino is 9600 baud whereas in esp it is 115200 baud
  Serial.println('\n');

  lcd.init();                                       //starting lcd

  lcd.backlight();
  lcd.setCursor(0, 0);
  Serial.print("  SMOKE SENSOR ");
  lcd.print("  SMOKE SENSOR ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("  WARMING UP ! ");
  Serial.print("  WARMING UP ! ");

  wifiMulti.addAP("Saji", "SamsungM11");             // Wi-Fi networks you want to connect to.

  espCon;
}
//--------------------------------------------------------------

unsigned long previousTime = millis();
const unsigned long interval = 1000;

void loop() {

  ArduinoOTA.handle();

  unsigned long diff = millis() - previousTime;
  if (diff > interval) {
    digitalWrite(blueLed, !digitalRead(blueLed));                            // Change the state of the LED
    previousTime += diff;
  }

  int analogSensor = analogRead(smoke);
  lcd.print("Smoke Level:");
  lcd.print(analogSensor-50);
  Serial.print("Smoke Level:");
  Serial.println(analogSensor-50);                                             
  
  // Checks if it has reached the threshold value -------------------------
  
  if (analogSensor-50 > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    lcd.setCursor(0, 2);
    lcd.print("Alert....!!!");
    Serial.print("Alert....!!!");
    digitalWrite(greenLed, LOW);
    tone(buzzer, 1000, 200);
  }
  else
  {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    lcd.setCursor(0, 2);
    lcd.print(".....Normal.....");
    Serial.print(".....Normal.....");
    noTone(buzzer);
  }
  lcd.clear();
}

void espCon() {                                                      //ESP CONNECT

  digitalWrite(greenLed, HIGH);                                          //GREEN LED

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) {                          // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above.
    Serial.print(++i); Serial.print(' ');
    digitalWrite(greenLed, !digitalRead(greenLed));
    digitalWrite(redLed, !digitalRead(greenLed));
  }

  Serial.println('\n');
  Serial.print("(>‿◠)✌ You're Connected To Prototype V1 by ");
  Serial.println(WiFi.SSID());                                       // Tell us what network we're connected to.
  lcd.print(" CONNECTED To ");
  lcd.println(WiFi.SSID());                                       // Tell us what network we're connected to.
  Serial.print("(⌐▨_▨ ) This Is My Cool IP Address:\t");
  Serial.println(WiFi.localIP());                                    // Send the IP address of the ESP8266 to the computer.
  Serial.print("😶 This is my MAC Address:\t");
  Serial.println(WiFi.macAddress());                                 // Tell us what is the Mac address

 //----------------------------------- 
  
  //OTA -------

  ArduinoOTA.setHostname("SmokeDetector@2021");
  ArduinoOTA.setPassword("147258");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Authentication Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");
  digitalWrite(greenLed, 1);
  
  digitalWrite(greenLed, LOW);  
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(buzzer, LOW);   
  
  lcd.clear();                        
}
