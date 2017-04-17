

#ifndef Setup_h
#define Setup_h

#include <Arduino.h>
#include <EEPROM.h>

#define ASCII_ENTER        0x0D
// Max lenght of array is 63. Do not get over this value otherwise settings are nog stored.
#define LINE_LENGTH          32

class Setup
{
  public:
    Setup ();                          // Constructor
    begin(char* ssid, char* pasp);
    void menu();
    void flushSerial(); 
    void getSettings();
    char getSerialChar();

  private:
    char *_ssid;
    char *_pasp;
    char _incomingLine[LINE_LENGTH];              // Incoming line (over the serial port)
    uint8_t _lineLength;            
    
    void enterSSID();
    void enterPassP();
    char waitSerialChar();
    void readLine( char* strLine); 
    void putSettings();
};
#endif
