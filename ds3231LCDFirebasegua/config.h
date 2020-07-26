// Adafruit IO Feed Reading
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-feeds/sharing-a-feed
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Brent Rubell for Adafruit Industries
// Copyright (c) 2018 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.
/************************ Adafruit IO Config *******************************/

#define IO_USERNAME   "ongzz"
#define IO_KEY        "aio_Bkqg46JccYr1i6dhBNMq4fHhKUcG"

#define WIFI_SSID   "ZONGZ"
#define WIFI_PASS   "zz12343705"

#include "AdafruitIO_WiFi.h"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE)
  // Configure the pins used for the ESP32 connection
  #if !defined(SPIWIFI_SS) // if the wifi definition isnt in the board variant
    // Don't change the names of these #define's! they match the variant ones
    #define SPIWIFI SPI
    #define SPIWIFI_SS 10  // Chip select pin
    #define NINA_ACK 9    // a.k.a BUSY or READY pin
    #define NINA_RESETN 6 // Reset pin
    #define NINA_GPIO0 -1 // Not connected
  #endif
  AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS, NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
  AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif
