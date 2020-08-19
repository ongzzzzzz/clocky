//3Aug

//change alarm to LED see how, 
//buzzer sound at right time, 
//test SwitchLED, when alarm sound only light up
//scrolling text, 


//interface for changing alarm time
//3d print + battery system
//check for events at specific time (see notebook)
// firebase system cloud function?

#include <Wire.h>
#include "RTClib.h"
#include "FirebaseESP8266.h"
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "esp8266-f2775.firebaseio.com"
#define FIREBASE_AUTH "EQ0xXkArCNnNnDlrd7kxaroYoTULU1lZgDPLtD9L"
#define WIFI_SSID "ZONGZ"
#define WIFI_PASSWORD "zz12343705"

#define alarm 14
#define sw1 12
#define sw2 13

// RTC 3V, LCD 5V
// Buzzer 5V
// Both switches Input ->  Switch -> Ground, NodeMCU internal Pullup

// ESP8266 Pinouts
// D0   = 16; SCL of RTC and LCD
// D1   = 5;  SDA of ETC and LCD
// D2   = 4;
// D3   = 0;
// D4   = 2;
// D5   = 14; Buzzer +ve Pin
// D6   = 12; Switch 1 
// D7   = 13; Switch 2
// D8   = 15;
// D9   = 3;
// D10  = 1;


FirebaseData firebaseData;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
String summary = "";
String begins = "";
String ends = "";

bool state = 1;

//Set alarm time? 
//DateTime alarm (0,0,0);

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

  pinMode(alarm, OUTPUT);        //buzzer
  pinMode(sw1, INPUT_PULLUP);  //switch1
  pinMode(sw2, INPUT_PULLUP);  //switch2

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

//////////////////////////////////////////////////////////////////////// 
//cacat place

//  if((digitalRead(sw1) == LOW) && (digitalRead(sw2) == LOW)){
//    digitalWrite(alarm, LOW);
//  }
//  if(digitalRead(sw1) == LOW){Serial.println("SW1 IS ON");}
//  if(digitalRead(sw2) == LOW){Serial.println("SW2 IS ON");}

//  state = !state;
//  if(state){  Serial.println("State is true ");  }
//  else if(!state){  Serial.println("State is false");}

  digitalWrite(alarm, LOW);

////////////////////////////////////////////////////////////////////////
  
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

  String Hour = "0";
  String Minute = "0";
  String Second = "0";

  if(dt.hour() < 10){
    Hour += String(dt.hour());
  } else{
    Hour = String(dt.hour());
  }
  
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
  lcd.print(Hour);
  lcd.print(":");
  lcd.print(Minute);
  lcd.print(":");
  lcd.print(Second);
}

//  void types(String a) { Serial.println("it's a String"); }
//  void types(int a) { Serial.println("it's an int"); }
//  void types(char *a) { Serial.println("it's a char*"); }
//  void types(float a) { Serial.println("it's a float"); }
//  void types(bool a) { Serial.println("it's a bool"); }
