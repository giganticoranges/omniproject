#include <Arduino.h>


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <stdio.h>
#include <string.h>

int scanTime = 1; //In seconds
BLEScan* pBLEScan;

char *substring(char *string, int position, int length);

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      int rssiRecieved = 0;
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      if (advertisedDevice.haveRSSI()){
        rssiRecieved = (int)advertisedDevice.getRSSI();
      }
      if (advertisedDevice.haveManufacturerData()){
        char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
        char* major = substring( pHex, 41, 4);
        char* minor = substring( pHex, 45, 4);
        if(strcmp(major, "1388") == 0)
        {
          if(strcmp(minor, "0004") == 0)
          {
            if(rssiRecieved > -70)
            {
              digitalWrite(13, HIGH);
            } 
            else
            {
              digitalWrite(13, LOW);
            }
          }
          if(strcmp(minor, "0003") == 0)
          {
            if(rssiRecieved > -70)
            {
              digitalWrite(12, HIGH);
            } 
            else
            {
              digitalWrite(12, LOW);
            }
          }
          if(strcmp(minor, "0002") == 0)
          {
            if(rssiRecieved > -70)
            {
              digitalWrite(2, HIGH);
            } 
            else
            {
              digitalWrite(2, LOW);
            }
          }
          printf("major: %s \n", major);
          printf("minor: %s \n", minor);
          printf("rssi: %d \n", rssiRecieved);
        }
        free(pHex);
      }
    }
};


void setup() {
  Serial.begin(115200);
  //Serial.println("Scanning...");

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(2, OUTPUT);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  //delay(2000);
}

char *substring(char *string, int position, int length){
   char *pointer;
   int c;
 
   pointer = (char*)malloc(length+1);
   
   if (pointer == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(1);
   }
 
   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *(string+position-1);      
      string++;  
   }
 
   *(pointer+c) = '\0';
 
   return pointer;
}