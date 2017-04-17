
//
// setupESP
//
//  1. resetESP
//  2. espOK?
//  3. connectAP
//  4. gotIP?
//  5. resturnOK
//
boolean setup_ESP(){
  boolean bContinue=true;     
  delay(500);
  ESP8266.print("=\"\"\r\n");
  serial_flush_ESP();
  ESP8266.print("=\"OK1\"\r\n");
  if(read_until_ESP("OK",sizeof("OK"),2000,1)){ 
      Serial.println("Talking to ESP");
  }
  delay(500);
  serial_flush_ESP();
  if (restartESP()==true)
    Serial.println("ESP Succesfully restarted");

  delay(500);
  if (isESP_OK()==true) {
    Serial.println("ESP is responding ok");
  }
  else {
    bContinue=false;
  }
  
  if (bContinue)
    if (connectAP()==true) {
      Serial.println("ConnectAP=OK");
    }
    else {
      Serial.println("ConnectAP=NOK");
      bContinue=false;
    }

  if (bContinue) {
    delay(500);
    if (getESP_Status()=='5') {
      Serial.println("ESP up and running");
    }
    else {
      bContinue=false;
    }
  }
  serial_flush_ESP();
  return bContinue;
}

//
// ESP:connectAP("MySSID","AP-Password",10)  returns "Connected" or "Failed"
// TODO: optimise Timeout varable
//
boolean connectAP() {
  boolean bRetval=false;
  serial_flush_ESP();
  ESP8266.print("connectAP(\"");
  ESP8266.print(SSID_ESP);
  ESP8266.print("\",\"");
  ESP8266.print(SSID_KEY);
  ESP8266.print("\",10)");
  ESP8266.print("\r\n");
  
  if(read_until_ESP("Connected",sizeof("Connected"),10000,0)){ 
    bRetval=true;
  }
  serial_flush_ESP();
  return bRetval;
}

//
// =sendData(host,port,"/uri")
//
boolean sendData(char sHost[],int iPort, String sUri) {
  boolean bRetval=false;
  serial_flush_ESP();
  ESP8266.print("=sendData(\"");
  ESP8266.print(sHost);
  ESP8266.print("\",");
  ESP8266.print(iPort);
  ESP8266.print(",\"");
  ESP8266.print(sUri);
  ESP8266.print("\")");
  ESP8266.print("\r\n");
  
  if(read_until_ESP("HTTP",sizeof("HTTP"),10000,0)){ 
    bRetval=true;
  }
  delay(500);
  serial_flush_ESP();
  return bRetval;
}

//
// Not used yet
//
boolean waitforSerialData(int timeout) {
  int secs=0;
  boolean bRetval = false;
  while (secs < timeout) {
    if (ESP8266.available()>0){
        secs=timeout;   
    }
    else {
      secs++;
      delay(1000);
    }   
  }
}


//
// Get wifi status 
// 0: STA_IDLE,
// 1: STA_CONNECTING,
// 2: STA_WRONGPWD,
// 3: STA_APNOTFOUND,
// 4: STA_FAIL,
// 5: STA_GOTIP.
//
int getESP_Status() {
  serial_flush_ESP();
  int iRetval=0;
  ESP8266.print("=wifi.sta.status()\r\n");
  if(read_until_ESP(keyword_rn,sizeof(keyword_rn),1000,1)) {
    //regel gevonden
    if (scratch_data_from_ESP[0]>0) {
      iRetval=scratch_data_from_ESP[1];
    }
  } 
  else {
    Serial.println("ERR: wifi.sta.status");
  }
  return iRetval;
}

// check if ESP is responding
boolean isESP_OK() {
  boolean bRetval=false;
  ESP8266.print("=\"OK\"\r\n");
  if(read_until_ESP("OK",sizeof("OK"),2000,0)) {
    bRetval=true;
    Serial.println("ESP responding");
  }
  else {
    Serial.println("ESP not responding");
  }
 serial_flush_ESP(); 
 return bRetval;
}

//
// check if ESP is responding
//
boolean restartESP() {
  boolean bRetval=false;
  // send some data to terminate pevious if exists
  ESP8266.print("=\"OK\"\r\n\r\n");
 serial_flush_ESP(); 
  ESP8266.print("node.restart()\r\n");
  
  if(read_until_ESP("eady",sizeof("eady"),4000,1)) {
    bRetval=true;
    Serial.println("ESP restart OK");
  }
  else {
    Serial.println("ESP restart failed");
  }
 serial_flush_ESP(); 
 return bRetval;
}


//
// flush all data from softserial buffer
//
void serial_flush_ESP(){
  char temp;
  while(ESP8266.available()){
    temp =ESP8266.read();
    //Serial.write(temp);  //debug only
    delay(1);//could play around with this value if buffer overflows are occuring
  }
}

//
// show data in scratch buffer - for debugging only.
//
void showScratchData(boolean zeroData) {
  int i=0;
  for (i = 0; i < sizeof(scratch_data_from_ESP) - 1; i++){
    Serial.print(i, DEC);
    Serial.print(" = ");
    Serial.print(scratch_data_from_ESP[i],HEX);
    Serial.println();
    scratch_data_from_ESP[i]=0;
  }
  
}

