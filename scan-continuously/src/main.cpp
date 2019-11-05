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


const char *ssid = "aicv_devices";  //wifi network name
const char *password = "AICVLab1!"; //wifi network password
const uint16_t port = 8090;         //port to connect to
const char *host = "192.168.0.100"; //host ip address (ifconfig on the host pc)

int scanTime = 0; //In seconds
int beaconArrayCount = 0;
int beaconsIdArray[MAX_BEACONS]; //holds all the beacon ids found
int beaconsRSSIArray[MAX_BEACONS]; //holds all rssi found
BLEScan *pBLEScan;

char *substring(char *string, int position, int length);
String concatenateArray(int *array1Name, int *array2Name, int numElements);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    int rssiRecieved = 0;
    //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    if (advertisedDevice.haveRSSI())
    {
      rssiRecieved = (int)advertisedDevice.getRSSI();
    }
    if (advertisedDevice.haveManufacturerData())
    {
      char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t *)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
      char *major = substring(pHex, 41, 4);
      char *minor = substring(pHex, 45, 4);
      //Serial.println(pHex);
      //Serial.println(major);
      if (strcmp(major, "1388") == 0 && beaconArrayCount < MAX_BEACONS)
      {
        //Serial.println(major);
        //Serial.println(minor);
        //Serial.println(rssiRecieved);
        beaconsIdArray[beaconArrayCount] = atoi(minor);
        beaconsRSSIArray[beaconArrayCount] = rssiRecieved;
        beaconArrayCount += 1;
      }
      free(pHex);
    }
  }
};

void setup()
{
  Serial.begin(115200);

  //setup wifi
   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("connecting to wifi...");
  }

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(95); // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(1);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");
  WiFiClient client;
  if (!client.connect(host, port)){
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }
  client.print(concatenateArray(beaconsIdArray, beaconsRSSIArray, beaconArrayCount));
  client.stop();


  beaconArrayCount = 0;
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(1);
}

char *substring(char *string, int position, int length)
{
  char *pointer;
  int c;

  pointer = (char *)malloc(length + 1);

  if (pointer == NULL)
  {
    Serial.println("Unable to allocate memory.\n");
    exit(1);
  }

  for (c = 0; c < length; c++)
  {
    *(pointer + c) = *(string + position - 1);
    string++;
  }
  *(pointer + c) = '\0';
  return pointer;
}

String concatenateArray(int *array1Name, int *array2Name, int numElements){
  String toSend = "Beacon,RSSI\n";
  for (int x = 0; x < numElements; x++){
    toSend += String(array1Name[x]);
    toSend += ',';
    toSend += String(array2Name[x]);
    toSend += '\n';
  }
  Serial.println(toSend);
  return toSend;
}