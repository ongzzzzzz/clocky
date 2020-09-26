/*
    This sketch trys to Connect to the best AP based on a given list

*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("CEC", "CEC_2018");
  wifiMulti.addAP("ZONGZ", "zz12343705");
  

  Serial.println("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
}

void loop() {
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }
}
