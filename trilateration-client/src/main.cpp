#include <Arduino.h>
#include <WiFi.h>

//bluetooth imports
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//#include <stdio.h>
#include <string.h>

#define MAX_BEACONS 20

//#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif


//bluetooth variables
int scanTime = 1; //In seconds
BLEScan *pBLEScan;

const char *ssid = "aicv_devices";  //wifi network name
const char *password = "AICVLab1!"; //wifi network password

const uint16_t port = 8090;         //port to connect to
const char *host = "192.168.0.100"; //host ip address (ifconfig on the host pc)

int beaconArrayCount;
int beaconsIdArray[MAX_BEACONS]; //holds all the beacon ids found
int beaconsRSSIArray[MAX_BEACONS]; //holds all rssi found
int topBeaconsIDArray[3]; //holds the ids of the strongest 3 beacons recieved
int topBeaconsRSSIArray[3]; //holds the RSSIs of the strongest 3 beacons recieved

//function prototypes
char *substring(char *string, int position, int length);
void clearArray(int *arrayName, int numElements);
String concatenateArray(int *array1Name, int *array2Name, int numElements);
void getStrongestThree(int *iDArray, int *rssiArray, int *strongestIDArray, int *strongestRSSIArray);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    DEBUG_PRINTLN("beaconArrayCount reset");
    int rssiRecieved = 0;
    //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    if (advertisedDevice.haveRSSI()){
      rssiRecieved = (int)advertisedDevice.getRSSI();
    }
    if (advertisedDevice.haveManufacturerData()){
      char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t *)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
      char *major = substring(pHex, 41, 4);
      char *minor = substring(pHex, 45, 4); //don't really care which beacons are close (on client side)
      if (strcmp(major, "1388") == 0){
        if (beaconArrayCount < MAX_BEACONS){
          DEBUG_PRINT("beacon addded, beaconArrayCount: ");
          DEBUG_PRINTLN(beaconArrayCount);
          beaconsIdArray[beaconArrayCount] = atoi(minor);
          beaconsRSSIArray[beaconArrayCount] = rssiRecieved;
          beaconArrayCount += 1;
        }
      }
      free(pHex);
    }
  }
};

void setup()
{

  Serial.begin(115200); //set this serial rate in platformio.ini

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    DEBUG_PRINTLN("...");
  }

  DEBUG_PRINT("WiFi connected with IP: ");
  DEBUG_PRINTLN(WiFi.localIP());

  //bluetooth stuff
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(95); // less or equal setInterval value
}

void loop()
{
  //clear beacons array
  clearArray(beaconsIdArray, beaconArrayCount);
  clearArray(beaconsRSSIArray, beaconArrayCount);
  clearArray(topBeaconsRSSIArray, 3);
  clearArray(topBeaconsIDArray, 3);

  beaconArrayCount = 0;

  //bluetooth scan
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());

  //create new wifi client
  WiFiClient client;
  if (!client.connect(host, port)){
    DEBUG_PRINTLN("Connection to host failed");
    delay(1000);
    return;
  }
  DEBUG_PRINTLN("Connected to server successful!");

  getStrongestThree(beaconsIdArray, beaconsRSSIArray, topBeaconsIDArray, topBeaconsRSSIArray);
  for (int x = 0; x < 3; x++){
    Serial.print(x+1);
    Serial.print(". Beacon ID: ");
    Serial.print(topBeaconsIDArray[x]);
    Serial.print("  Beacon RSSI: ");
    Serial.println(topBeaconsRSSIArray[x]);
  }
    Serial.print("**\n");

  client.print(concatenateArray(topBeaconsIDArray, topBeaconsRSSIArray, 3));
  //client.print('\n');

  
  //DEBUG_PRINTLN("Disconnecting...");
  client.stop();

  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(50);
}

char *substring(char *string, int position, int length){
  char *pointer;
  int c;

  pointer = (char *)malloc(length + 1);

  if (pointer == NULL){
    DEBUG_PRINTLN("Unable to allocate memory.\n");
    exit(1);
  }

  for (c = 0; c < length; c++){
    *(pointer + c) = *(string + position - 1);
    string++;
  }
  *(pointer + c) = '\0';
  return pointer;
}

void clearArray(int *arrayName, int numElements){
  for (int x = 0; x < numElements; x++)
  {
    DEBUG_PRINTLN("abbout to reset: ");
    DEBUG_PRINTLN(arrayName[x]);
    arrayName[x] = 0;
  }
}

String concatenateArray(int *array1Name, int *array2Name, int numElements){
  String toSend = "";
  for (int x = 0; x < numElements; x++){
    toSend += String(array1Name[x]);
    toSend += ',';
    toSend += String(array2Name[x]);
    toSend += ',';
  }
  Serial.print("Sending: ");
  Serial.println(toSend);
  return toSend;
}

void getStrongestThree(int *iDArray, int *rssiArray, int *strongestIDArray, int *strongestRSSIArray){
  int indexOfStrongest = -999;
  int strongestRSSI = -999;

  int indexOfSecondStrongest = -999;
  int secondStrongestRSSI = -999; 

  int indexOfThirdStrongest = -999;
  int thirdStrongestRSSI = -999;

  for(int x = 0; x < beaconArrayCount; x++){
    if(rssiArray[x] > strongestRSSI){
      strongestRSSI = rssiArray[x];
      indexOfStrongest = iDArray[x];
    }
    else if(rssiArray[x] > secondStrongestRSSI){
      secondStrongestRSSI = rssiArray[x];
      indexOfSecondStrongest = iDArray[x];
    }
    else if(rssiArray[x] > thirdStrongestRSSI){
      thirdStrongestRSSI = rssiArray[x];
      indexOfThirdStrongest = iDArray[x];
    }
  }

  strongestIDArray[0] = indexOfStrongest;
  strongestRSSIArray[0] = strongestRSSI;

  strongestIDArray[1] = indexOfSecondStrongest;
  strongestRSSIArray[1] = secondStrongestRSSI;

  strongestIDArray[2] = indexOfThirdStrongest;
  strongestRSSIArray[2] = thirdStrongestRSSI;

}