//#include <Seeed_BME280.h>

/*
  TODO: add usefull description
*/


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>  // Address changed to BMP280_ADDRESS (0x76)

Adafruit_BMP280 bme; // I2C

#include <SoftwareSerial.h>

#include "DHT.h"
#define DHTPIN 12  
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


#include "setup.h"
char SSID_ESP[LINE_LENGTH];
char SSID_KEY[LINE_LENGTH];

// Inits
SoftwareSerial ESP8266(10, 11); // RX, TX

//BME280 bme280;
DHT dht(DHTPIN, DHTTYPE);


// Configuration Contants
// =====================================================
//const char SSID_ESP[] = "Wifi ssid";
//const char SSID_KEY[] = "Wifi passphrase";
const char csHost[] = "192.168.14.100";
const int  ciPort=8080;
const int idSensor1=274;
const int idSensor2=275;
const int idSensor3=278;
const int idSensor4=279;
// =====================================================



const char keyword_rn[] = "\r\n";
const long interval = 30000;
unsigned long previousMillis = 0;

// Setup instance
Setup stp;


//functions
int getESP_Status();
boolean isESP_OK();
boolean setup_ESP();
boolean restartESP();
void serial_flush_ESP();
void showScratchData();
boolean connectAP();
void showScratchData(boolean zeroData);
boolean sendData(char sHost[],int iPort, char sUri);
boolean read_until_ESP(const char keyword1[], int key_size, int timeout_val, byte mode);

//global vars
char scratch_data_from_ESP[80]; //first byte is the length of bytes
unsigned long timeout_start_val;
char s_temp[16];

boolean bBMP280=true;

//
// Initialise the whole stuff 
//
void setup() {
   pinMode(13, OUTPUT);
   digitalWrite(13, LOW);
    
  stp.begin(SSID_ESP,SSID_KEY);
  stp.getSettings();
   
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  if (!bme.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    bBMP280=false;
  }
 
  dht.begin();
    
  Serial.println("ESP8266_Serial  Version: " __DATE__);

  // set the data rate for the SoftwareSerial port
  ESP8266.begin(9600);

  setup_ESP();
}


//
// Main loop, which hopefully never ends
//
void loop() {
  float pressure280=0;
  float temp280=0;
  unsigned long currentMillis = millis();

  if (stp.getSerialChar()=='M') {
    stp.menu();
  }


  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("DHT read err.");
  }
  else {
      Serial.print("dhtH=");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("dhtT=");
      Serial.print(t);
      Serial.println(" *C ");  
  }
  

  if (bBMP280) {
    temp280=bme.readTemperature();
    pressure280=bme.readPressure()/100;
  
    Serial.print(F("T280="));
    Serial.print(temp280);
    Serial.println(" *C");
    Serial.print(F("P280="));
    Serial.print(pressure280);
    Serial.println(" Pa");    
    Serial.println();
  }


  /*
   * read all the switches.  D2..D9
   * TODO: D2 & D3 on interrupt?
   */
   





  //===============[ Send data to ESP ]================
  boolean bContinue=true;
  digitalWrite(13, HIGH); 
  if (getESP_Status()=='5') {
     Serial.println("ESP is up.  Starting cycle");   
  }
  else {
    // Reset ESP
    bContinue=setup_ESP();
  }

  if (bContinue) {
    // LDR
    int iLdr = analogRead(0);
    String sUri = "/json.htm?type=command&param=udevice&idx="; 
    sUri = sUri + idSensor1 + "&nvalue=0&svalue=" + iLdr;
    Serial.println(sUri);
    delay(500);
    if (sendData("csHost",ciPort,sUri)) {
      Serial.println("SendData1 ok");
    }
    digitalWrite(13, LOW);
    delay(1000);
    digitalWrite(13, HIGH);

    // Sensor2
    //json.htm?type=command&param=udevice&idx=275&nvalue=0&svalue=22.2;44.4;0
    //
    sUri = "/json.htm?type=command&param=udevice&idx="; 
    sUri = sUri + idSensor2 + "&nvalue=0&svalue=" + t + ";" + h + ";" + "0";
    Serial.println(sUri);
    delay(500);
    if (sendData(csHost,ciPort,sUri)) {
      Serial.println("SendData2 ok");
    }

    if (bBMP280) {
      digitalWrite(13, LOW);
      delay(1000);
      digitalWrite(13, HIGH);
      // Sensor3
      ///json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=BAR;5
      //
      sUri = "/json.htm?type=command&param=udevice&idx="; 
      sUri = sUri + idSensor3 + "&nvalue=0&svalue=" + pressure280 + ";" + "0";
      Serial.println(sUri);
      delay(500);
      if (sendData(csHost,ciPort,sUri)) {
        Serial.println("SendData3 ok");
      }
    
      digitalWrite(13, LOW);
      delay(1000);
      digitalWrite(13, HIGH);
    
      // sensor4
      // json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP    
      //
      sUri = "/json.htm?type=command&param=udevice&idx="; 
      sUri = sUri + idSensor4 + "&nvalue=0&svalue=" + temp280;
      Serial.println(sUri);
      delay(500);
      if (sendData(csHost,ciPort,sUri)) {
        Serial.println("SendData4 ok");
      }
    } 
  }

 digitalWrite(13, LOW);

 // delay without delay
 if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  } 
 //delay(30000);
}

