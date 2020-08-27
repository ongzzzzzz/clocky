//20Aug

//important
//scrolling text
//get event end time from firebase
//get cal data from firebase at midnight or event end  (use ||)

//optimize firebase / zapier to efficiency when get data
//refactor, dun use so much char day[] things 

//todo:
//interface for changing alarm time
//3d print + battery system
//check for events at specific time (see notebook)
// firebase system cloud function?
//control backlight
//get Time from internet
//make a JS webapp for it

//done:
//change alarm to LED see how, 
//buzzer sound at right time, use LEDSWITCH to stop alarm
//test SwitchLED, when alarm sound only light up
//get alarm time from firebase on device boot

// discontinued ideas
//use buttons to adjust alarm
//save alarm time in EEPROM

// RTC 3V, LCD 5V
// Buzzer 5V
// Both switches Input -> [Switch] -> Ground, NodeMCU internal Pullup
// ESP8266 Pinouts
// D0   = 16; SCL of RTC and LCD
// D1   = 5;  SDA of ETC and LCD
// D2   = 4;
// D3   = 0;
// D4   = 2;
// D5   = 14; Buzzer +ve Pin
// D6   = 12; Switch 1 
// D7   = 13; Switch 2
// D8   = 15; StopSwitch (LED sw)
// D9   = 3;
// D10  = 1;
//the switches are inverted (INPUT_PULLUP)
//the outputs / LEDs are not

#include <Wire.h>
#include "RTClib.h"
#include "FirebaseESP8266.h"
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define FIREBASE_HOST "esp8266-f2775.firebaseio.com"
#define FIREBASE_AUTH "EQ0xXkArCNnNnDlrd7kxaroYoTULU1lZgDPLtD9L"
#define WIFI_SSID "ZONGZ"
#define WIFI_PASSWORD "zz12343705"

#define alarm 14
#define sw1 12
#define sw2 13
#define StopSW 15 

FirebaseData firebaseData;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

String summary = "";
String begins = "";
String ends = "";
String alarmTime = "";

bool alarmState = 0;

byte Left[] = {
  B01010,
  B10101,
  B01010,
  B10101,
  B01010,
  B10101,
  B01010,
  B10101
};

byte Right[] = {
  B10101,
  B01010,
  B10101,
  B01010,
  B10101,
  B01010,
  B10101,
  B01010
};

//alarm time
DateTime actualTime;
int shiftedIndexes = 0;
String message = "";

void setup (){
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
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
  lcd.createChar(6, Left);
  lcd.createChar(9, Right);
  lcd.backlight();

  pinMode(alarm, OUTPUT);      //buzzer
  pinMode(sw1, INPUT_PULLUP);  //switch1
  pinMode(sw2, INPUT_PULLUP);  //switch2
  pinMode(StopSW, INPUT_PULLUP); //StopSW

  getAlarmTime();

  //get some message on startup
  if (Firebase.getString(firebaseData, "/calendar/event/summary")){
    message = firebaseData.stringData();
  } else {
    Serial.print("Error in getString: ");
    Serial.println(firebaseData.errorReason());
  }
}

void loop (){
  DateTime now = rtc.now();
  actualTime = now + TimeSpan(0,0,4,31);
  
  showDate(actualTime);
  
//  checkEvents(actualTime);

  char rightNow[] = "hh:mm:ss";
  String stringyTime = String(actualTime.toString(rightNow));
  Serial.print("the alarm i got: ");
  Serial.println(alarmTime);
  Serial.print("the time rn: ");
  Serial.println(stringyTime);
  
  if((stringyTime == alarmTime) && (alarmState == 0)){
    alarmState = 1;
    Serial.println("WAKEY WAKEY MDF WAKEY WAKEY MDF WAKEY WAKEY MDF WAKEY WAKEY MDF ");
  }
  
  if((alarmState == 1) && (digitalRead(StopSW) == HIGH)){
    alarmState = 0;
    Serial.println("!!!!!!!!!!!!!!!!!!!! Alarm Stopped !!!!!!!!!!!!!!!!!!!!");
  }
  
  digitalWrite(alarm, alarmState);

  Serial.print(rtc.getTemperature());
  Serial.println(" C");

  printEvents(message, shiftedIndexes);
  //scrolling mechanism
  if(shiftedIndexes > message.length()){
    shiftedIndexes = 0;
  } else{
    shiftedIndexes++;
  }  
  
  delay(1000);
  lcd.clear();
}

void printEvents(String message, int shiftedIndexes){
  String toPrint = "";
  
  toPrint += message.substring(shiftedIndexes, message.length());
  toPrint += message.substring(0, shiftedIndexes);

  lcd.setCursor(0, 1);
  lcd.print(toPrint);

  
}

void showDate(DateTime dt){
  char Date[] = "DDMMM";
  lcd.setCursor(0, 0);
  lcd.print(dt.toString(Date));
  
  lcd.write(6);
  lcd.write(9);
  
  char TimeRN[] = "hh:mm:ss";
  lcd.print(dt.toString(TimeRN));
}

void getAlarmTime(){
  if (Firebase.getString(firebaseData, "/calendar/alarmTime")){
    alarmTime = firebaseData.stringData();
  } else {
    Serial.print("Error in getString: ");
    Serial.println(firebaseData.errorReason());
  }
}



//  lcd.autoscroll();
//  // print from 0 to 9:
//  for (int thisChar = 0; thisChar < 10; thisChar++) {
//    lcd.print(thisChar);
//    delay(500);
//  }
//  // turn off automatic scrolling
//  lcd.noAutoscroll();

//void checkEvents(DateTime dt){
//  char currently[] = "hh:mm";
//  Serial.println(dt.toString(currently));
//
//  //if event ended OR is currently midnight, update summary and start and end time
//  
//  if(dt.toString(currently) == "00:00"){
//    if (Firebase.getString(firebaseData, "/calendar/event/summary")){
//      summary = firebaseData.stringData();
//    } else {
//      Serial.print("Error in getString: ");
//      Serial.println(firebaseData.errorReason());
//    }
//    if (Firebase.getString(firebaseData, "/calendar/event/begins")){
//      begins = firebaseData.stringData();
//    } else {
//      Serial.print("Error in getString: ");
//      Serial.println(firebaseData.errorReason());
//    }
//    if (Firebase.getString(firebaseData, "/calendar/event/ends")){
//      ends = firebaseData.stringData();
//    } else {
//      Serial.print("Error in getString: ");
//      Serial.println(firebaseData.errorReason());
//    }
//  }
//}

//void printEvents(DateTime dt){
//  char dayName[] = "DDD";
//  String today = dt.toString(dayName);
//  if(summary == ""){
//    Serial.println(today + ": Nothing Scheduled!");
//  }
//  else{
//    Serial.println(today + ": " + summary + " from " + begins + " to " + ends);
//  }
//}

/////////////////////////////////////////////////// references to the bottom ///////////////////////////////////////////////////

//The toString() buffer can be defined using following combinations:
  //hh - the hour with a leading zero (00 to 23)
  //mm - the minute with a leading zero (00 to 59)
  //ss - the whole second with a leading zero where applicable (00 to 59)
  //YYYY - the year as four digit number
  //YY - the year as two digit number (00-99)
  //MM - the month as number with a leading zero (01-12)
  //MMM - the abbreviated English month name ('Jan' to 'Dec')
  //DD - the day as number with a leading zero (01 to 31)
  //DDD - the abbreviated English day name ('Mon' to 'Sun')

  void types(String a) { Serial.println("it's a String"); }
  void types(int a) { Serial.println("it's an int"); }
  void types(char *a) { Serial.println("it's a char*"); }
  void types(float a) { Serial.println("it's a float"); }
  void types(bool a) { Serial.println("it's a bool"); }

//DateTime is an OBJECT
//dt.hour() and stuff are ints
//rtc.getTemperature() is a float
//the toString()s are char*s


//toggle input
//  if(digitalRead(sw1)== LOW){ alarmState = !alarmState; Serial.println("Switch is toggled")}

//push input
//  if(digitalRead(sw1) == LOW){ 
//    alarmState = 1; 
//    Serial.println("Switch is momentarily pressed"); 
//  } else {
//    alarmState = 0;
//  }
