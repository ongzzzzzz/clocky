#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void setup () 
{
  Serial.begin(9600);
  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.begin(16,2);
  lcd.init();

  lcd.backlight();
}

void loop () 
{
    DateTime now = rtc.now();
    
    Serial.println("Current Date & Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print((now.minute()+5), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

     
    lcd.setCursor(0, 0);
    lcd.print("Date: ");
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    
    lcd.setCursor(6, 0);
    lcd.print(now.year(), DEC);
    lcd.print(":");
    lcd.print(now.month(), DEC);
    lcd.print(":");
    lcd.print(now.day(), DEC);
   
    lcd.setCursor(6, 1);
    lcd.print(now.hour(), DEC);
    lcd.print(":");
    lcd.print(now.minute()+5, DEC);
    lcd.print(":");
    lcd.print(now.second(), DEC); 
    
    Serial.println();
    delay(1000);
    lcd.clear();
}
