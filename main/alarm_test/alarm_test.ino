//25July, add scrolling text next

#include <Wire.h>
#include "RTClib.h"
#include "FirebaseESP8266.h"
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "esp8266-f2775.firebaseio.com"
#define FIREBASE_AUTH "EQ0xXkArCNnNnDlrd7kxaroYoTULU1lZgDPLtD9L"
#define WIFI_SSID "ZONGZ"
#define WIFI_PASSWORD "zz12343705"

FirebaseData firebaseData;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

String summary = "";
String begins = "";
String ends = "";

void setup ()
{
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.begin(16, 2);
  lcd.init();

  lcd.backlight();
}

void loop ()
{
  DateTime now = rtc.now();

  if(now.hour() == 0 && now.minute() == 0){
    checkEvents();
  }

  showDate(now);

  String today = daysOfTheWeek[now.dayOfTheWeek()];

  if(summary == ""){
    Serial.println(today + ": Nothing Scheduled!");
  }
  else{
    Serial.println(today + ": " + summary + " from " + begins + " to " + ends);
  }

  Serial.println();
  delay(1000);
  lcd.clear();
}

void checkEvents(){

  
  if (Firebase.getString(firebaseData, "/calendar/event/summary")){
    summary = firebaseData.stringData();
  } else {
    Serial.print("Error in getString: ");
    Serial.println(firebaseData.errorReason());
  }


  if (Firebase.getString(firebaseData, "/calendar/event/begins")){
    begins = firebaseData.stringData();
  } else {
    Serial.print("Error in getString: ");
    Serial.println(firebaseData.errorReason());
  }


  if (Firebase.getString(firebaseData, "/calendar/event/ends")){
    ends = firebaseData.stringData();
  } else {
    Serial.print("Error in getString: ");
    Serial.println(firebaseData.errorReason());
  }
}

void showDate(DateTime dt){
  dt = dt + TimeSpan(0,0,4,37);
  
  lcd.setCursor(0, 0);
  lcd.print("Date: "); 
  lcd.setCursor(0, 1);
  lcd.print("Time: ");

  lcd.setCursor(6, 0);
  lcd.print(dt.year(), DEC);
  lcd.print("/");
  lcd.print(dt.month(), DEC);
  lcd.print("/");
  lcd.print(dt.day(), DEC);

//  String Minute = "0";
//  String Second = "0";
//  if(dt.minute() < 10){
//    Minute += String(dt.minute());
//  } else{
//    Minute = String(dt.minute());
//  }
//  
//  if(dt.second() < 10){
//    Second += String(dt.second());  
//  } else{
//    Second = String(dt.second());
//  }

  String Minute = "0";
  String Second = "0";
  
  if(dt.minute() < 10){
    Minute += String(dt.minute());  
  } else {
    Minute = String(dt.minute()); 
  }

  if(dt.second() < 10){
    Second += String(dt.second());
  } else{
    Second = String(dt.second());
  }

  lcd.setCursor(6, 1);
  lcd.print(dt.hour(), DEC);
  lcd.print(":");
  lcd.print(Minute);
  lcd.print(":");
  lcd.print(Second);
}

  void types(String a) { Serial.println("it's a String"); }
  void types(int a) { Serial.println("it's an int"); }
  void types(char *a) { Serial.println("it's a char*"); }
  void types(float a) { Serial.println("it's a float"); }
  void types(bool a) { Serial.println("it's a bool"); }

//  if (Firebase.getString(firebaseData, "/calendar/event/summary"))
//  {
//    Serial.print("Event summary: ");
//    Serial.println(firebaseData.stringData());
//  } else {
//    Serial.print("Error in getString: ");
//    Serial.println(firebaseData.errorReason());
//  }
//
//  if (Firebase.getString(firebaseData, "/calendar/event/begins"))
//  {
//    Serial.print("Event begin time: ");
//    Serial.println(firebaseData.stringData());
//  } else {
//    Serial.print("Error in getString: ");
//    Serial.println(firebaseData.errorReason());
//  }
//
//  if (Firebase.getString(firebaseData, "/calendar/event/ends"))
//  {
//    Serial.print("Event end time: ");
//    Serial.println(firebaseData.stringData());
//  } else {
//    Serial.print("Error in getString: ");
//    Serial.println(firebaseData.errorReason());
//  }
