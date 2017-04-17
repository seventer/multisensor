/*
 * 
 * Setup module for: 
 *    - entering SSID and Passphrase
 *    - store settings in EEPROM
 *    - retreive settings from EEPROM
 */

#include "Setup.h"

Setup::Setup() {
  // construction
}

Setup::begin(char* ssid, char* pasp) {
  _ssid = ssid;
  _pasp = pasp;
}

//
// setup menu for SSID and Passphrase
//
void Setup::menu() {
  boolean inMenu = true;
  do {
    flushSerial();  
    Serial.println(">S/P/W/Q:");

    char m = waitSerialChar();

    switch (m) {
        case 's':
        case 'S':
          // ssid
          //Serial.println("SSID");
          enterSSID();
          break;
        case 'p':
        case 'P':
          // passphrase
          //Serial.println("PassP");
          enterPassP();
          break;
        case 'w':
        case 'W':
          // write
          Serial.println("Write");
          putSettings();
          break;
        case 'g':
        case 'G':
          // write
          Serial.println("Get");
          getSettings();
          break;
        case 'q':
        case 'Q':
          // quit
          Serial.println("Quit");
          inMenu = false;
          break;
      }
  } while(inMenu);
}
/*
 *  enter SSID via serial blocking read
 */
void Setup::enterSSID() {
  Serial.print("SSID: ");
  readLine(SSID_ESP);
  Serial.print("S=");
  Serial.println(SSID_ESP);
}

/*
 *  enter Passphrase via serial blocking read
 */
void Setup::enterPassP() {
  Serial.print("Pass: ");
  readLine(SSID_KEY);
  Serial.print("P=");
  Serial.println(SSID_KEY);
}


/*
 * wait for serial char - blocking
 */
char Setup::waitSerialChar() {
  char m;
  while(!Serial.available());  
  if(Serial.available() > 0 ) {
      m = Serial.read();
  }
  return m;
}

/*
 * wait for serial char - blocking
 */
char Setup::getSerialChar() {
  char m;
  if(Serial.available() > 0 ) {
      m = Serial.read();
  }
  return m;
}

/*
 * wait for serial line - blocking
 */
void Setup::readLine( char* strLine) {
  flushSerial();
  _lineLength=0;

  do
  {
    strLine[_lineLength] = waitSerialChar();
    _lineLength++;
  } while (strLine[_lineLength-1] != ASCII_ENTER  && (_lineLength < LINE_LENGTH) );
  strLine[_lineLength-1] = '\0'; 
}


/*
 * flush all data from serial buffer
 */
void Setup::flushSerial(){
    if(Serial.available() > 0 ) {
      Serial.read();
      delay(10);
  }
}


/*
 * getSettings from eeprom
 * store in SSID & Passphrase array
 * display to serial
 * 
 *  eeprom_layout
 * [0]=length byte
 * [1..63] string chars
 * [64]=length byte
 * [65..127]
 */
void Setup::putSettings(){

  int len = strlen(SSID_ESP);
  Serial.print("S[");
  Serial.print(len);
  Serial.print("]");
  EEPROM.write(0, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(i+1, SSID_ESP[i]);
    Serial.print(SSID_ESP[i]);
  }
  Serial.println("");

  len = strlen(SSID_KEY);
  Serial.print("P[");
  Serial.print(len);
  Serial.print("]");
  EEPROM.write(64, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(i+65, SSID_KEY[i]);
    Serial.print(SSID_KEY[i]);
  }
  Serial.println("");

}

/*
 * getSettings from eeprom
 * store in SSID & Passphrase array
 * display to serial
 */
void Setup::getSettings() {
  int len = EEPROM.read(0);
  Serial.print("S[");
  Serial.print(len);
  Serial.print("]");
  for (int i = 0; i < len; i++) {
    SSID_ESP[i] = EEPROM.read(i+1);
    Serial.print(SSID_ESP[i]);
  }
  Serial.println("");

  len = EEPROM.read(64);
  Serial.print("P[");
  Serial.print(len);
  Serial.print("]");
  for (int i = 0; i < len; i++) {
    SSID_KEY[i] = EEPROM.read(i+65);
    Serial.print(SSID_KEY[i]);
  }
  Serial.println("");
}



