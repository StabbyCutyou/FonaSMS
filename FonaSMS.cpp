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
#include <avr/pgmspace.h>
    // next line per http://postwarrior.com/arduino-ethershield-error-prog_char-does-not-name-a-type/
#define prog_char  char PROGMEM

#if (ARDUINO >= 100)
  #include "Arduino.h"
  #ifndef __SAM3X8E__  // Arduino Due doesn't support SoftwareSerial
    #include <SoftwareSerial.h>
  #endif
#else
  #include "WProgram.h"
  #include <NewSoftSerial.h>
#endif

#include "FonaSMS.h"

FonaSMS::FonaSMS(int8_t rst)
{
  _rstpin = rst;

  mySerial = 0;
}

uint8_t FonaSMS::type(void) {
  return _type;
}

boolean FonaSMS::begin(Stream &port) {
  mySerial = &port;

  pinMode(_rstpin, OUTPUT);
  digitalWrite(_rstpin, HIGH);
  delay(10);
  digitalWrite(_rstpin, LOW);
  delay(100);
  digitalWrite(_rstpin, HIGH);

  // give 7 seconds to reboot
  delay(7000);

  while (mySerial->available()) mySerial->read();

  sendCheckReply(F("AT"), F("OK"));
  delay(100);
  sendCheckReply(F("AT"), F("OK"));
  delay(100);
  sendCheckReply(F("AT"), F("OK"));
  delay(100);

  // turn off Echo!
  sendCheckReply(F("ATE0"), F("OK"));
  delay(100);

  if (! sendCheckReply(F("ATE0"), F("OK"))) {
    return false;
  }

  // turn on hangupitude
  sendCheckReply(F("AT+CVHU=0"), F("OK"));

  delay(100);
  flushInput();

#ifdef FonaSMS_DEBUG
    Serial.print("\t---> "); Serial.println("ATI");
#endif

  mySerial->println(F("ATI"));

  readline(500, true);

#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif

  if (strstr_P(replybuffer, (prog_char *)F("SIM808 R14")) != 0) {
    _type = FONA808_V2;
  } else if (strstr_P(replybuffer, (prog_char *)F("SIM808 R13")) != 0) {
    _type = FONA808_V1;
  } else if (strstr_P(replybuffer, (prog_char *)F("SIM800 R13")) != 0) {
    _type = FONA800L;
  } else if (strstr_P(replybuffer, (prog_char *)F("SIMCOM_SIM5320A")) != 0) {
    _type = FONA3G_A;
  } else if (strstr_P(replybuffer, (prog_char *)F("SIMCOM_SIM5320E")) != 0) {
    _type = FONA3G_E;
  }

  return true;
}


/********* Serial port ********************************************/

/********* Real Time Clock ********************************************/

/* Gutted */

/********* BATTERY & ADC ********************************************/

/* Gutted */

/********* SIM ***********************************************************/

/* Gutted */

/********* IMEI **********************************************************/

uint8_t FonaSMS::getIMEI(char *imei) {
  getReply(F("AT+GSN"));

  // up to 15 chars
  strncpy(imei, replybuffer, 15);
  imei[15] = 0;

  readline(); // eat 'OK'

  return strlen(imei);
}

/********* NETWORK *******************************************************/


/********* AUDIO *******************************************************/


/********* FM RADIO *******************************************************/

/* Gutted */

/********* PWM/BUZZER **************************************************/

/* Gutted */

/********* CALL PHONES **************************************************/

/********* SMS **********************************************************/

boolean FonaSMS::sendSMS(char *smsaddr, char *smsmsg) {
  if (! sendCheckReply(F("AT+CMGF=1"), F("OK"))) return -1;

  char sendcmd[30] = "AT+CMGS=\"";
  strncpy(sendcmd+9, smsaddr, 30-9-2);  // 9 bytes beginning, 2 bytes for close quote + null
  sendcmd[strlen(sendcmd)] = '\"';

  if (! sendCheckReply(sendcmd, "> ")) return false;
#ifdef ADAFRUIT_FONA_DEBUG
  Serial.print(F("> ")); Serial.println(smsmsg);
#endif
  mySerial->println(smsmsg);
  mySerial->println();
  mySerial->write(0x1A);
#ifdef ADAFRUIT_FONA_DEBUG
  Serial.println("^Z");
#endif
  if ( (_type == FONA3G_A) || (_type == FONA3G_E) ) {
    // Eat two sets of CRLF
    readline(200);
    //Serial.print("Line 1: "); Serial.println(strlen(replybuffer));
    readline(200);
    //Serial.print("Line 2: "); Serial.println(strlen(replybuffer));
  }
  readline(10000); // read the +CMGS reply, wait up to 10 seconds!!!
  //Serial.print("Line 3: "); Serial.println(strlen(replybuffer));
  if (strstr(replybuffer, "+CMGS") == 0) {
    return false;
  }
  readline(1000); // read OK
  //Serial.print("* "); Serial.println(replybuffer);

  if (strcmp(replybuffer, "OK") != 0) {
    return false;
  }

  return true;
}

/********* GPS **********************************************************/

/********* TCP FUNCTIONS  ************************************/

/********* HELPERS *********************************************/

boolean FonaSMS::expectReply(const __FlashStringHelper *reply,
                                   uint16_t timeout) {
  readline(timeout);
#ifdef FonaSMS_DEBUG
  Serial.print(F("\t<--- ")); Serial.println(replybuffer);
#endif
  return (strcmp_P(replybuffer, (prog_char*)reply) == 0);
}

/********* LOW LEVEL *******************************************/

inline int FonaSMS::available(void) {
  return mySerial->available();
}

inline size_t FonaSMS::write(uint8_t x) {
  return mySerial->write(x);
}

inline int FonaSMS::read(void) {
  return mySerial->read();
}

inline int FonaSMS::peek(void) {
  return mySerial->peek();
}

inline void FonaSMS::flush() {
  mySerial->flush();
}

void FonaSMS::flushInput() {
    // Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(available()) {
            read();
            timeoutloop = 0;  // If char was received reset the timer
        }
        delay(1);
    }
}

uint16_t FonaSMS::readRaw(uint16_t b) {
  uint16_t idx = 0;

  while (b && (idx < sizeof(replybuffer)-1)) {
    if (mySerial->available()) {
      replybuffer[idx] = mySerial->read();
      idx++;
      b--;
    }
  }
  replybuffer[idx] = 0;

  return idx;
}

uint8_t FonaSMS::readline(uint16_t timeout, boolean multiline) {
  uint16_t replyidx = 0;

  while (timeout--) {
    if (replyidx >= 254) {
      //Serial.println(F("SPACE"));
      break;
    }

    while(mySerial->available()) {
      char c =  mySerial->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;

        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      replybuffer[replyidx] = c;
      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);
      replyidx++;
    }

    if (timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = 0;  // null term
  return replyidx;
}

uint8_t FonaSMS::getReply(char *send, uint16_t timeout) {
  flushInput();

#ifdef FonaSMS_DEBUG
    Serial.print(F("\t---> ")); Serial.println(send);
#endif

  mySerial->println(send);

  uint8_t l = readline(timeout);
#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif
  return l;
}

uint8_t FonaSMS::getReply(const __FlashStringHelper *send, uint16_t timeout) {
  flushInput();

#ifdef FonaSMS_DEBUG
  Serial.print(F("\t---> ")); Serial.println(send);
#endif

  mySerial->println(send);

  uint8_t l = readline(timeout);
#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif
  return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t FonaSMS::getReply(const __FlashStringHelper *prefix, char *suffix, uint16_t timeout) {
  flushInput();

#ifdef FonaSMS_DEBUG
  Serial.print(F("\t---> ")); Serial.print(prefix); Serial.println(suffix);
#endif

  mySerial->print(prefix);
  mySerial->println(suffix);

  uint8_t l = readline(timeout);
#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif
  return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t FonaSMS::getReply(const __FlashStringHelper *prefix, int32_t suffix, uint16_t timeout) {
  flushInput();

#ifdef FonaSMS_DEBUG
  Serial.print(F("\t---> ")); Serial.print(prefix); Serial.println(suffix, DEC);
#endif

  mySerial->print(prefix);
  mySerial->println(suffix, DEC);

  uint8_t l = readline(timeout);
#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif
  return l;
}

// Send prefix, suffix, suffix2, and newline. Return response (and also set replybuffer with response).
uint8_t FonaSMS::getReply(const __FlashStringHelper *prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout) {
  flushInput();

#ifdef FonaSMS_DEBUG
  Serial.print(F("\t---> ")); Serial.print(prefix);
  Serial.print(suffix1, DEC); Serial.print(","); Serial.println(suffix2, DEC);
#endif

  mySerial->print(prefix);
  mySerial->print(suffix1);
  mySerial->print(',');
  mySerial->println(suffix2, DEC);

  uint8_t l = readline(timeout);
#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif
  return l;
}

// Send prefix, ", suffix, ", and newline. Return response (and also set replybuffer with response).
uint8_t FonaSMS::getReplyQuoted(const __FlashStringHelper *prefix, const __FlashStringHelper *suffix, uint16_t timeout) {
  flushInput();

#ifdef FonaSMS_DEBUG
  Serial.print("\t---> "); Serial.print(prefix);
  Serial.print('"'); Serial.print(suffix); Serial.println('"');
#endif

  mySerial->print(prefix);
  mySerial->print('"');
  mySerial->print(suffix);
  mySerial->println('"');

  uint8_t l = readline(timeout);
#ifdef FonaSMS_DEBUG
    Serial.print (F("\t<--- ")); Serial.println(replybuffer);
#endif
  return l;
}

boolean FonaSMS::sendCheckReply(char *send, char *reply, uint16_t timeout) {
  getReply(send, timeout);

/*
  for (uint8_t i=0; i<strlen(replybuffer); i++) {
    Serial.print(replybuffer[i], HEX); Serial.print(" ");
  }
  Serial.println();
  for (uint8_t i=0; i<strlen(reply); i++) {
    Serial.print(reply[i], HEX); Serial.print(" ");
  }
  Serial.println();
  */
  return (strcmp(replybuffer, reply) == 0);
}

boolean FonaSMS::sendCheckReply(const __FlashStringHelper *send, const __FlashStringHelper *reply, uint16_t timeout) {
  getReply(send, timeout);
  return (strcmp_P(replybuffer, (prog_char*)reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
boolean FonaSMS::sendCheckReply(const __FlashStringHelper *prefix, char *suffix, const __FlashStringHelper *reply, uint16_t timeout) {
  getReply(prefix, suffix, timeout);
  return (strcmp_P(replybuffer, (prog_char*)reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
boolean FonaSMS::sendCheckReply(const __FlashStringHelper *prefix, int32_t suffix, const __FlashStringHelper *reply, uint16_t timeout) {
  getReply(prefix, suffix, timeout);
  return (strcmp_P(replybuffer, (prog_char*)reply) == 0);
}

// Send prefix, suffix, suffix2, and newline.  Verify FONA response matches reply parameter.
boolean FonaSMS::sendCheckReply(const __FlashStringHelper *prefix, int32_t suffix1, int32_t suffix2, const __FlashStringHelper *reply, uint16_t timeout) {
  getReply(prefix, suffix1, suffix2, timeout);
  return (strcmp_P(replybuffer, (prog_char*)reply) == 0);
}

// Send prefix, ", suffix, ", and newline.  Verify FONA response matches reply parameter.
boolean FonaSMS::sendCheckReplyQuoted(const __FlashStringHelper *prefix, const __FlashStringHelper *suffix, const __FlashStringHelper *reply, uint16_t timeout) {
  getReplyQuoted(prefix, suffix, timeout);
  return (strcmp_P(replybuffer, (prog_char*)reply) == 0);
}


boolean FonaSMS::parseReply(const __FlashStringHelper *toreply,
          uint16_t *v, char divider, uint8_t index) {
  char *p = strstr_P(replybuffer, (prog_char*)toreply);  // get the pointer to the voltage
  if (p == 0) return false;
  p+=strlen_P((prog_char*)toreply);
  //Serial.println(p);
  for (uint8_t i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
    //Serial.println(p);

  }
  *v = atoi(p);

  return true;
}

boolean FonaSMS::parseReply(const __FlashStringHelper *toreply,
          char *v, char divider, uint8_t index) {
  uint8_t i=0;
  char *p = strstr_P(replybuffer, (prog_char*)toreply);
  if (p == 0) return false;
  p+=strlen_P((prog_char*)toreply);

  for (i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
  }

  for(i=0; i<strlen(p);i++) {
    if(p[i] == divider)
      break;
    v[i] = p[i];
  }

  v[i] = '\0';

  return true;
}

// Parse a quoted string in the response fields and copy its value (without quotes)
// to the specified character array (v).  Only up to maxlen characters are copied
// into the result buffer, so make sure to pass a large enough buffer to handle the
// response.
boolean FonaSMS::parseReplyQuoted(const __FlashStringHelper *toreply,
          char *v, int maxlen, char divider, uint8_t index) {
  uint8_t i=0, j;
  // Verify response starts with toreply.
  char *p = strstr_P(replybuffer, (prog_char*)toreply);
  if (p == 0) return false;
  p+=strlen_P((prog_char*)toreply);

  // Find location of desired response field.
  for (i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
  }

  // Copy characters from response field into result string.
  for(i=0, j=0; j<maxlen && i<strlen(p); ++i) {
    // Stop if a divier is found.
    if(p[i] == divider)
      break;
    // Skip any quotation marks.
    else if(p[i] == '"')
      continue;
    v[j++] = p[i];
  }

  // Add a null terminator if result string buffer was not filled.
  if (j < maxlen)
    v[j] = '\0';

  return true;
}

boolean FonaSMS::sendParseReply(const __FlashStringHelper *tosend,
				      const __FlashStringHelper *toreply,
				      uint16_t *v, char divider, uint8_t index) {
  getReply(tosend);

  if (! parseReply(toreply, v, divider, index)) return false;

  readline(); // eat 'OK'

  return true;
}
