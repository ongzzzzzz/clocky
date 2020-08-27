/* 
   https://simple-circuit.com/arduino-ds3231-real-time-clock-alarm-temperature/Arduino 
   real time clock and calendar with 2 alarm functions and temperature monitor using DS3231
   Read DS3231 RTC datasheet to understand the code
   Time & date parameters can be set using two push buttons connected to pins 9 (B1) & 10 (B2).
   Alarm1 and alarm2 can be set using two push buttons connected to 11 (B3) & 10 (B2).
   Pin 12 becomes high when alarm occurred and button B2 returns it to low and
   turns the occurred alarm OFF.
   DS3231 interrupt pin is connected to Arduino external interrupt pin 2.
*/
 
// include LCD library code
#include <LiquidCrystal.h>
// include Wire library code (needed for I2C protocol devices)
#include <Wire.h>
 
// LCD module connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);
 
const int button1   =  9;                   // button1 pin number
const int button2   = 10;                   // button1 pin number
const int button3   = 11;                   // button1 pin number
const int alarm_pin = 12;                   // Alarms pin number
 
void setup() {
  pinMode(9,  INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  digitalWrite(alarm_pin, LOW);
  // set up the LCD's number of columns and rows
  lcd.begin(20, 4);
  Wire.begin();                                 // Join i2c bus
  attachInterrupt(digitalPinToInterrupt(2), Alarm, FALLING);
}
 
// Variables declaration
bool alarm1_status, alarm2_status;
char Time[]     = "  :  :  ",
     calendar[] = "      /  /20  ",
     alarm1[]   = "A1:   :  :00", alarm2[]   = "A2:   :  :00",
     temperature[] = "T:   .   C";
byte  i, second, minute, hour, day, date, month, year,
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour,
      status_reg;
 
void Alarm(){
  digitalWrite(alarm_pin, HIGH);
}
void DS3231_read(){                             // Function to read time & calendar data
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
  Wire.write(0);                                // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS3231 and release I2C bus at end of reading
  second = Wire.read();                         // Read seconds from register 0
  minute = Wire.read();                         // Read minuts from register 1
  hour   = Wire.read();                         // Read hour from register 2
  day    = Wire.read();                         // Read day from register 3
  date   = Wire.read();                         // Read date from register 4
  month  = Wire.read();                         // Read month from register 5
  year   = Wire.read();                         // Read year from register 6
}
void alarms_read_display(){                     // Function to read and display alarm1, alarm2 and temperature data
  byte control_reg, temperature_lsb;
  char temperature_msb;
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
  Wire.write(0x08);                             // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 11);                   // Request 11 bytes from DS3231 and release I2C bus at end of reading
  alarm1_minute = Wire.read();                  // Read alarm1 minutes
  alarm1_hour   = Wire.read();                  // Read alarm1 hours
  Wire.read();                                  // Skip alarm1 day/date register
  alarm2_minute = Wire.read();                  // Read alarm2 minutes
  alarm2_hour   = Wire.read();                  // Read alarm2 hours
  Wire.read();                                  // Skip alarm2 day/date register
  control_reg = Wire.read();                    // Read the DS3231 control register
  status_reg  = Wire.read();                    // Read the DS3231 status register
  Wire.read();                                  // Skip aging offset register
  temperature_msb = Wire.read();                // Read temperature MSB
  temperature_lsb = Wire.read();                // Read temperature LSB
    // Convert BCD to decimal
  alarm1_minute = (alarm1_minute >> 4) * 10 + (alarm1_minute & 0x0F);
  alarm1_hour   = (alarm1_hour   >> 4) * 10 + (alarm1_hour & 0x0F);
  alarm2_minute = (alarm2_minute >> 4) * 10 + (alarm2_minute & 0x0F);
  alarm2_hour   = (alarm2_hour   >> 4) * 10 + (alarm2_hour & 0x0F);
    // End conversion
  alarm1[8]     = alarm1_minute % 10  + 48;
  alarm1[7]     = alarm1_minute / 10  + 48;
  alarm1[5]     = alarm1_hour   % 10  + 48;
  alarm1[4]     = alarm1_hour   / 10  + 48;
  alarm2[8]     = alarm2_minute % 10  + 48;
  alarm2[7]     = alarm2_minute / 10  + 48;
  alarm2[5]     = alarm2_hour   % 10  + 48;
  alarm2[4]     = alarm2_hour   / 10  + 48;
  alarm1_status = bitRead(control_reg, 0);      // Read alarm1 interrupt enable bit (A1IE) from DS3231 control register
  alarm2_status = bitRead(control_reg, 1);      // Read alarm2 interrupt enable bit (A2IE) from DS3231 control register
  if(temperature_msb < 0){
    temperature_msb = abs(temperature_msb);
    temperature[2] = '-';
  }
  else
    temperature[2] = ' ';
  temperature_lsb >>= 6;
  temperature[4] = temperature_msb % 10  + 48;
  temperature[3] = temperature_msb / 10  + 48;
  if(temperature_lsb == 0 || temperature_lsb == 2){
    temperature[7] = '0';
    if(temperature_lsb == 0) temperature[6] = '0';
    else                     temperature[6] = '5';
  }
  if(temperature_lsb == 1 || temperature_lsb == 3){
    temperature[7] = '5';
    if(temperature_lsb == 1) temperature[6] = '2';
    else                     temperature[6] = '7';
  }
  temperature[8]  = 223;                        // Put the degree symbol
  lcd.setCursor(10, 0);
  lcd.print(temperature);                       // Display temperature
  lcd.setCursor(0, 2);
  lcd.print(alarm1);                            // Display alarm1
  lcd.setCursor(17, 2);
  if(alarm1_status)  lcd.print("ON ");          // If A1IE = 1 print 'ON'
  else               lcd.print("OFF");          // If A1IE = 0 print 'OFF'
  lcd.setCursor(0, 3);
  lcd.print(alarm2);                            // Display alarm2
  lcd.setCursor(17, 3);
  if(alarm2_status)  lcd.print("ON ");          // If A2IE = 1 print 'ON'
  else               lcd.print("OFF");          // If A2IE = 0 print 'OFF'
}
void calendar_display(){                        // Function to display calendar
  switch(day){
    case 1:  strcpy(calendar, "Sun   /  /20  "); break;
    case 2:  strcpy(calendar, "Mon   /  /20  "); break;
    case 3:  strcpy(calendar, "Tue   /  /20  "); break;
    case 4:  strcpy(calendar, "Wed   /  /20  "); break;
    case 5:  strcpy(calendar, "Thu   /  /20  "); break;
    case 6:  strcpy(calendar, "Fri   /  /20  "); break;
    case 7:  strcpy(calendar, "Sat   /  /20  "); break;
    default: strcpy(calendar, "Sat   /  /20  ");
  }
  calendar[13] = year  % 10 + 48;
  calendar[12] = year  / 10 + 48;
  calendar[8]  = month % 10 + 48;
  calendar[7]  = month / 10 + 48;
  calendar[5]  = date  % 10 + 48;
  calendar[4]  = date  / 10 + 48;
  lcd.setCursor(0, 1);
  lcd.print(calendar);                          // Display calendar
}
void DS3231_display(){
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);
  // End conversion
  Time[7]     = second % 10  + 48;
  Time[6]     = second / 10  + 48;
  Time[4]     = minute % 10  + 48;
  Time[3]     = minute / 10  + 48;
  Time[1]     = hour   % 10  + 48;
  Time[0]     = hour   / 10  + 48;
  calendar_display();                           // Call calendar display function
  lcd.setCursor(0, 0);
  lcd.print(Time);                              // Display time
}
void Blink(){
  byte j = 0;
  while(j < 10 && (digitalRead(button1) || i >= 5) && digitalRead(button2) && (digitalRead(button3) || i < 5)){
    j++;
    delay(25);
  }
}
byte edit(byte x, byte y, byte parameter){
  char text[3];
  while(!digitalRead(button1) || !digitalRead(button3));    // Wait until button B1 is released
  while(true){
    while(!digitalRead(button2)){                           // If button B2 is pressed
      parameter++;
      if(((i == 0) || (i == 5)) && parameter > 23)          // If hours > 23 ==> hours = 0
        parameter = 0;
      if(((i == 1) || (i == 6)) && parameter > 59)          // If minutes > 59 ==> minutes = 0
        parameter = 0;
      if(i == 2 && parameter > 31)                          // If date > 31 ==> date = 1
        parameter = 1;
      if(i == 3 && parameter > 12)                          // If month > 12 ==> month = 1
        parameter = 1;
      if(i == 4 && parameter > 99)                          // If year > 99 ==> year = 0
        parameter = 0;
      if(i == 7 && parameter > 1)                           // For alarms ON or OFF (1: alarm ON, 0: alarm OFF)
        parameter = 0;
      lcd.setCursor(x, y);
      if(i == 7){                                           // For alarms ON & OFF
        if(parameter == 1)  lcd.print("ON ");
        else                lcd.print("OFF");
      }
      else{
        sprintf(text,"%02u", parameter);
        lcd.print(text);
      }
      if(i >= 5){
        DS3231_read();                          // Read data from DS3231
        DS3231_display();                       // Display DS3231 time and calendar
      }
      delay(200);                               // Wait 200ms
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                            // Print two spaces
    if(i == 7) lcd.print(" ");                  // Print space (for alarms ON & OFF)
    Blink();                                    // Call Blink function
    lcd.setCursor(x, y);
    if(i == 7){                                 // For alarms ON & OFF
      if(parameter == 1)  lcd.print("ON ");
      else                lcd.print("OFF");
    }
    else{
      sprintf(text,"%02u", parameter);
      lcd.print(text);
    }
    Blink();
    if(i >= 5){
      DS3231_read();
      DS3231_display();}
    if((!digitalRead(button1) && i < 5) || (!digitalRead(button3) && i >= 5)){
      i++;                                      // Increment 'i' for the next parameter
      return parameter;                         // Return parameter value and exit
    }
  }
}
 
void loop() {
  if(!digitalRead(button1)){                    // If B1 button is pressed
      i = 0;
      hour   = edit(0, 0, hour);
      minute = edit(3, 0, minute);
      while(!digitalRead(button1));             // Wait until button B1 released
      while(true){
        while(!digitalRead(button2)){           // If button B2 button is pressed
          day++;                                // Increment day
          if(day > 7) day = 1;
          calendar_display();                   // Call display_calendar function
          lcd.setCursor(0, 1);
          lcd.print(calendar);                  // Display calendar
          delay(200);
        }
        lcd.setCursor(0, 1);
        lcd.print("   ");                       // Print 3 spaces
        Blink();
        lcd.setCursor(0, 1);
        lcd.print(calendar);                    // Print calendar
        Blink();                                // Call Blink function
        if(!digitalRead(button1))               // If button B1 is pressed
          break;
      }
      date = edit(4, 1, date);                  // Edit date
      month = edit(7, 1, month);                // Edit month
      year = edit(12, 1, year);                 // Edit year
      // Convert decimal to BCD
      minute = ((minute / 10) << 4) + (minute % 10);
      hour = ((hour / 10) << 4) + (hour % 10);
      date = ((date / 10) << 4) + (date % 10);
      month = ((month / 10) << 4) + (month % 10);
      year = ((year / 10) << 4) + (year % 10);
      // End conversion
      // Write time & calendar data to DS3231 RTC
      Wire.beginTransmission(0x68);             // Start I2C protocol with DS3231 address
      Wire.write(0);                            // Send register address
      Wire.write(0);                            // Reset sesonds and start oscillator
      Wire.write(minute);                       // Write minute
      Wire.write(hour);                         // Write hour
      Wire.write(day);                          // Write day
      Wire.write(date);                         // Write date
      Wire.write(month);                        // Write month
      Wire.write(year);                         // Write year
      Wire.endTransmission();                   // Stop transmission and release the I2C bus
      delay(200);
    }
    if(!digitalRead(button3)){                  // If B3 button is pressed
      while(!digitalRead(button3));             // Wait until button B3 released
      i = 5;
      alarm1_hour   = edit(4,  2, alarm1_hour);
      alarm1_minute = edit(7,  2, alarm1_minute);
      alarm1_status = edit(17, 2, alarm1_status);
      i = 5;
      alarm2_hour   = edit(4,  3, alarm2_hour);
      alarm2_minute = edit(7,  3, alarm2_minute);
      alarm2_status = edit(17, 3, alarm2_status);
      alarm1_minute = ((alarm1_minute / 10) << 4) + (alarm1_minute % 10);
      alarm1_hour   = ((alarm1_hour   / 10) << 4) + (alarm1_hour % 10);
      alarm2_minute = ((alarm2_minute / 10) << 4) + (alarm2_minute % 10);
      alarm2_hour   = ((alarm2_hour   / 10) << 4) + (alarm2_hour % 10);
      // Write alarms data to DS3231
      Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
      Wire.write(7);                              // Send register address (alarm1 seconds)
      Wire.write(0);                              // Write 0 to alarm1 seconds
      Wire.write(alarm1_minute);                  // Write alarm1 minutes value to DS3231
      Wire.write(alarm1_hour);                    // Write alarm1 hours value to DS3231
      Wire.write(0x80);                           // Alarm1 when hours, minutes, and seconds match
      Wire.write(alarm2_minute);                  // Write alarm2 minutes value to DS3231
      Wire.write(alarm2_hour);                    // Write alarm2 hours value to DS3231
      Wire.write(0x80);                           // Alarm2 when hours and minutes match
      Wire.write(4 | alarm1_status | (alarm2_status << 1));      // Write data to DS3231 control register (enable interrupt when alarm)
      Wire.write(0);                              // Clear alarm flag bits
      Wire.endTransmission();                     // Stop transmission and release the I2C bus
      delay(200);                                 // Wait 200ms
    }
    if(!digitalRead(button2) && digitalRead(alarm_pin)){         // When button B2 pressed with alarm (Reset and turn OFF the alarm)
      digitalWrite(alarm_pin, LOW);               // Turn OFF the alarm indicator
      Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
      Wire.write(0x0E);                           // Send register address (control register)
      // Write data to control register (Turn OFF the occurred alarm and keep the other as it is)
      Wire.write(4 | (!bitRead(status_reg, 0) & alarm1_status) | ((!bitRead(status_reg, 1) & alarm2_status) << 1));
      Wire.write(0);                              // Clear alarm flag bits
      Wire.endTransmission();                     // Stop transmission and release the I2C bus
    }
    DS3231_read();                                // Read time and calendar parameters from DS3231 RTC
    alarms_read_display();                        // Read and display alarms parameters
    DS3231_display();                             // Display time & calendar
    delay(50);                                    // Wait 50ms
}
// End of code
