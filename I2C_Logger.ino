#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
RTC_DS3231 rtc;

int port = 1; //Sets up the port for ID
int i = 0; //A global itterator 
int nextNum = 1; //The number of the next match
byte id[8]; //Contains both the ID and the Data
char descriptor[256];//Contains the string that describes the data

bool received = false; //When true activates the writing to SD
bool firstTime = true; //When true delays to allow data to be written to the arrays.
char matchLog[] = "MATCH00.TXT"; 

char charNum[3];

File dataFile;
File numFile;

void setup() {
  //Sets up serial printing and the I2C line
  Serial.begin(9600);
  Wire.begin(port);

  //Lets user know if SD is valid
  Serial.print("Initializing SD card...");

  if (!SD.begin(10, 11, 12, 13)) {
    Serial.println("initialization failed!");
    return;
  }else{
    Serial.println("Success");
  }
  //getMatchNumber(); //Currently doesn't work
  //The time will be off unless the RTC is powered by it's own battery.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.onReceive(receiveEvent);
  if(received){
    
    if(firstTime){
      delay(1000);
      firstTime = false;
    } else{
    Serial.print("[0]: ");
    Serial.println(id[0]);
    
    Serial.print("[1]: ");
    Serial.print(((float)(((short)id[1]<<8)+(short)id[2]))/10.0);
    
    
    
    
    dataFile = SD.open("matchLog.txt", FILE_WRITE);
    if (dataFile){
    dataFile.print("ID: ");
    dataFile.print(id[0]);
    dataFile.print(" ");
    description();
    dataFile.print("Data: ");
    dataFile.print(((float)(((short)id[1]<<8)+(short)id[2]))/10.0);
    dataFile.print(" ");
    }
        
    timeStamp();
    
    dataFile.close();
    received = false;
    i = 0;
    }
  }
}

void receiveEvent(int howMany){
  while(Wire.available()){
    //Serial.println(Wire.read());
    if(i <= 2){
      id[i] = Wire.read();
    }else{
      descriptor[i] = Wire.read();
    }
    
    
    i++;
    if(i == howMany){
      received = true;
    }
  }
  
         
}

void timeStamp(){
  DateTime now = rtc.now();
  dataFile.print("Time: ");
  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.print(now.second(), DEC);
  dataFile.println();
}

void description(){
  for(int ii = 2; ii < i; ii++){
    Serial.print(descriptor[ii]);
    dataFile.print(descriptor[ii]);
  }
}

void getMatchNumber(){
  Serial.print("Getting match number...");
  
  numFile = SD.open("Number.txt");
  if (numFile){
      while (numFile.available()) {
      numFile.read(charNum,2);
      int dig1 = charNum[0] - '0';
      int dig2 = charNum[1] - '0';
      nextNum = (dig1*10)+dig2+1;
      matchLog[5] = charNum[0];
      matchLog[6] = charNum[1]; 
      Serial.print(nextNum);    
  }
  numFile.close();
  Serial.print("  ");
  Serial.print(SD.remove("Number.txt"));

  numFile = SD.open("Number.txt", FILE_WRITE);
  if(nextNum<10){
    numFile.print(0);
    numFile.print(nextNum);
  }else{
    numFile.print(nextNum);
  }
  
  numFile.close();
  Serial.print("Finished");
  }else{
    Serial.print("FAILED");
  }
  
}

