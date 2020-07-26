//********************************
//Read Adafruit feed from Arduino
//********************************

#include "config.h"

// the Adafruit IO username of whomever owns the feed
#define FEED_OWNER "ongzz"

// set up the `sharedFeed`
AdafruitIO_Feed *sharedFeed = io.feed("training", FEED_OWNER);

void setup() {
  
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.println("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'sharedFeed' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  sharedFeed->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.println(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  sharedFeed->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

}

  // this function is called whenever an 'sharedFeed' feed message
  // is received from Adafruit IO. it was attached to
  // the 'digital' feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {
  
  //Turn the feed message into a string
 String answer = data->toString();
 //Serial.println("Raw asnwer " + answer);
 
 //Turn the string into usable data
   //date
   String date = answer.substring(0,10);
   //starting time
   String startHourString = answer.substring(11,13);
   int startHour = startHourString.toInt();
 
   String startMinuteString = answer.substring(14,16);
   int startMinute = startMinuteString.toInt();

   //ending time
   String endHourString = answer.substring(36,38);
   int endHour = endHourString.toInt();
  
   String endMinuteString = answer.substring(39,41);
   int endMinute = endMinuteString.toInt();
  
  
 //Print results in serial monitor
 //Serial.println("Raw asnwer " + answer);
 Serial.println("");
 Serial.println("***********************");
 Serial.println("Training on " + date );
 Serial.println("***********************");
 Serial.println("Training starts at " + startHourString + ":" + startMinuteString);
 Serial.println("Training ends at " + endHourString + ":" + endMinuteString);
  


 //You can use the data to calculate all sorts of things
  
 //Calculate duration (doesn't work at midnight)
 int startTime = startHour*60 + startMinute; //starting time in minutes of the day
 int endTime = endHour*60 + endMinute; //ending time in minutes of the day
 int durationTotal = endTime - startTime; //duration in minutes
 int durationHours = durationTotal/60; //duration whole hours
 int durationMin = durationTotal%60; // rest of duration in minutes
 Serial.print("Training lasts ");
 Serial.print(durationHours);
 Serial.print(" hours and ");
 Serial.print(durationMin);
 Serial.println(" minutes");

}
