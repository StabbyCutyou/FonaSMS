/***************************************************
  This is a library for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#ifndef FonaSMS_H
#define FonaSMS_H

#if (ARDUINO >= 100)
  #include "Arduino.h"
  #ifndef __SAM3X8E__  // Arduino Due doesn't support SoftwareSerial
    #include <SoftwareSerial.h>
  #endif
#else
  #include "WProgram.h"
  #include <NewSoftSerial.h>
#endif

#define FonaSMS_DEBUG

#define FONA800L 1
#define FONA800H 6

#define FONA808_V1 2
#define FONA808_V2 3

#define FONA3G_A 4
#define FONA3G_E 5

#define FONA_DEFAULT_TIMEOUT_MS 500

class FonaSMS : public Stream {
 public:
  FonaSMS(int8_t r);
  boolean begin(Stream &port);
  uint8_t type();

  // Stream
  int available(void);
  size_t write(uint8_t x);
  int read(void);
  int peek(void);
  void flush();

  // IMEI
  uint8_t getIMEI(char *imei);

  // SMS handling
  boolean sendSMS(char *smsaddr, char *smsmsg);

  // Helper functions to verify responses.
  boolean expectReply(const __FlashStringHelper *reply, uint16_t timeout = 10000);
  boolean sendCheckReply(char *send, char *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *send, const __FlashStringHelper *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);


 protected:
  int8_t _rstpin;
  uint8_t _type;

  char replybuffer[255];

  void flushInput();
  uint16_t readRaw(uint16_t b);
  uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);
  uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *prefix, char *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *prefix, int32_t suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout); // Don't set default value or else function call is ambiguous.
  uint8_t getReplyQuoted(const __FlashStringHelper *prefix, const __FlashStringHelper *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);

  boolean sendCheckReply(const __FlashStringHelper *prefix, char *suffix, const __FlashStringHelper *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *prefix, int32_t suffix, const __FlashStringHelper *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *prefix, int32_t suffix, int32_t suffix2, const __FlashStringHelper *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReplyQuoted(const __FlashStringHelper *prefix, const __FlashStringHelper *suffix, const __FlashStringHelper *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);


  boolean parseReply(const __FlashStringHelper *toreply,
          uint16_t *v, char divider  = ',', uint8_t index=0);
  boolean parseReply(const __FlashStringHelper *toreply,
          char *v, char divider  = ',', uint8_t index=0);
  boolean parseReplyQuoted(const __FlashStringHelper *toreply,
          char *v, int maxlen, char divider, uint8_t index);

  boolean sendParseReply(const __FlashStringHelper *tosend,
       const __FlashStringHelper *toreply,
       uint16_t *v, char divider = ',', uint8_t index=0);

  Stream *mySerial;
};

#endif
