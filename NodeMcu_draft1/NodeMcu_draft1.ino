#include <SoftwareSerial.h>
//#include <SD.h>
#include <TinyGPS++.h>
//#include <SPI.h>
#include <LittleFS.h>


//for SIM900
SoftwareSerial SIM900(D5, D6); //(Rx, Tx)

// The TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(D2, D1); // Choose two Arduino pins to use for software serial for GPS

void setup() {
  SPIFFSConfig cfg;
  cfg.setAutoFormat(false);
  SPIFFS.setConfig(cfg);
  
  SIM900.begin(9600);
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  gpsSerial.begin(9600);  // Start the software serial port at the GPS's(Neo-6M) default baud
  LittleFS.begin();
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  while(!gpsSerial){
    ;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()>0)
   switch(Serial.read())
  {
    case 's':
      startProcess();
      break;
  }
}

//---------------------------------------------------------------------------------
bool simResponse(unsigned long wait = 10000, char * s = "OK"){ // char * s
  String a;
  //Serial.print("Keyword is: ");
  //Serial.print(s);
  unsigned long timeout = millis()+wait;
  //Serial.print(", Timeout is at: ");
  //Serial.println(timeout);
  while(millis()<timeout){
    //Serial.println("in time");
    delay(100);
    while (SIM900.available()!=0){
      //Serial.println("reading data");
      a = SIM900.readString();
      Serial.print(a);
    }
    
    if (a.indexOf(s) >= 0){
        //Serial.println("found");
        return true;
     }
  }
  a ="";
  Serial.println("timeout over");
  timeout = millis()+4000;
  SIM900.println("AT+SAPBR=0,1"); /* Close any existing GPRS context */
  while(millis()<timeout){
    //Serial.println("in time");
    delay(100);
    while (SIM900.available()!=0){
      //Serial.println("reading data");
      a = SIM900.readString();
      Serial.print(a);
    }
    if (a.indexOf("OK") >= 0 || a.indexOf("ERROR") >= 0){
       Serial.println("GPRS Context handled");
       break; //return false;
     }
  }
  a ="";
  SIM900.println("AT+HTTPTERM");  /* Terminate any existing HTTP service */
  timeout = millis()+4000;
  while(millis()<timeout){
    //Serial.println("in time");
    delay(100);
    while (SIM900.available()!=0){
      //Serial.println("reading data");
      a = SIM900.readString();
      Serial.print(a);
    }
    if (a.indexOf("OK") >= 0 || a.indexOf("ERROR") >= 0){
       Serial.println("HTTP service handled");
       break;
       //return false;
     }
  }
  return false;

}

//---------------------------------------------------------------------------------
bool sendMessage(String latlng, bool backup=false){
  String data;
  Serial.println("\nSending Message: ");
  Serial.println(latlng);
  Serial.println("\n");
  SIM900.println("AT");
  if (!simResponse()){
    return false;
  }
  //Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" printed");
  SIM900.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); /* Connection type GPRS */
  if (!simResponse()){
    return false;
  }

  //Serial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\" printed");
  SIM900.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");  /* APN of the provider */
  if (!simResponse()){
    return false;
  }

  SIM900.println("AT+SAPBR=1,1");  /* Open GPRS context */
  if (!simResponse()){
    return false;
  }
  SIM900.println("AT+SAPBR=2,1");  /* Query the GPRS context */
  if (!simResponse()){
    return false;
  }
  
  SIM900.println("AT+HTTPINIT"); /* Initialize HTTP service */
  if (!simResponse()){
    return false;
  }
  SIM900.println("AT+HTTPPARA=\"CID\",1");  /* Set parameters for HTTP session */
  if (!simResponse()){
    return false;
  }
  SIM900.println("AT+HTTPPARA=\"URL\",\"455b89095e04.ngrok.io/data\"");  /* Set parameters for HTTP session */
  if (!simResponse()){
    return false;
  }

  if (backup){
    data = "a1b2c,"+String(millis())+",1,;"+latlng; //backup file 1
  }
  else{
    data = "a1b2c,"+String(millis())+",0,"+latlng; //gps live 0
  }
  //data = "a1b2c,"+String(millis())+","+latlng;
  //data = "{\"d_id\":\"a1b2c3\",\"time\":\""+String(millis())+"\",\"location\":\""+latlng+"\"}";
  //data = "{'time':"+String(millis())+",'location':"+latlng+"}";
  SIM900.println("AT+HTTPDATA="+String(data.length())+",10000");  /* POST data of n Bytes with maximum latency time of 10seconds for inputting the data*/
  if (!simResponse(10000,"DOWNLOAD")){
    return false;
  }
  SIM900.println(data);
  if (!simResponse()){
    return false;
  }
  SIM900.println("AT+HTTPACTION=1");  /* Start POST session */
  if (!simResponse(10000, "1,200,")){
    return false;
  }
  SIM900.println("AT+HTTPREAD");
  if (!simResponse()){
    return false;
  }
  SIM900.println("AT+HTTPTERM");  /* Terminate HTTP service */
  if (!simResponse()){
    return false;
  }
  SIM900.println("AT+SAPBR=0,1"); /* Close GPRS context */
  if (!simResponse()){
    return false;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------

void startProcess()
{
  Serial.println("started..");

  while (true){
    //check for existing file
    Serial.println("Checking for backup file");
    if (LittleFS.exists("loc.txt")){
      Serial.println("Backup found");
      File f = LittleFS.open("/loc.txt", "r");
      if (!f) {
        Serial.println("file open failed");
      }
      else{
        if (sendMessage(f.readString(), true)){
          Serial.println("Backupdata sucessfully sent.. removing backup file..");
          if (LittleFS.remove("loc.txt")){
            Serial.println("File removed");
          }
          else{
            Serial.println("couldn't remove file");
          }
        }
        else{
          Serial.println("Couldn't send backup data..");
        }
      }
    }
    else{
      Serial.println("No existing Backup file");
    }
    delay(1000);
    //start listening to gpsSerial
    gpsSerial.listen();
    if (gpsSerial.isListening()){
      Serial.println("listening to GPS for 10 seconds");
    }
    
    //get GPS data for 5 seconds
    unsigned long timeout = millis()+10000;
    while (millis()< timeout){
      while (gpsSerial.available()){
        //Serial.println("here");
        gps.encode(gpsSerial.read());
        }
    }
    if (gps.location.isValid()){ //
      if (gps.location.isUpdated()){ //
        //start listening to SIM900
        Serial.println("Updated GPS data location found");
        SIM900.listen();
        if (SIM900.isListening()){
          Serial.println("listening to SIM900");
        }
        //Serial.print(gps.location.lat(), 6);
        //Serial.print(gps.location.lng(), 6);
        //Serial.print(gps.altitude.meters(), 6);
        //Serial.print(gps.altitude.meters(), 6);
        //Serial.println("HDOP:"+String(gps.hdop.value()));
        if (sendMessage( String(gps.location.lng(),6)+","+ String(gps.location.lat(),6), false )){
        //if (sendMessage("12.123456,13.123456")){
          Serial.println("Message Sent");
          delay(1000);
        }
        else{
          Serial.println("Saving message in SPIFFS");
          if (saveOnFs( String(gps.location.lng(), 6)+","+ String(gps.location.lat() , 6))){ //
            Serial.println("location saved on file");
          }
          else{
            Serial.println("location couldn't be saved");
          }
        }
      }
    }
    Serial.println("Power saving mode for 15 seconds");
    Serial.println("----------------------------------------");
    delay(15000);
  }
  Serial.print("I am here");
}

//--------------------------------------------------------------------------------------------------

bool saveOnFs(String latlng){
  
  File f = LittleFS.open("/loc.txt", "a");
  if (!f) {
    Serial.println("file open failed");
    return false;
  }
  else{
    Serial.println("File opened sucessfully, appending");
    if (f.println(String(millis())+","+latlng)){
      Serial.println("data appended");
      f.close();
      return true;
    }
    else{
      Serial.println("Could not appended data");
      f.close();
      return false;
    }
  }
}
