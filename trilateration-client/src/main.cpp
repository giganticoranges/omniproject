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
int beaconsDataArray[MAX_BEACONS * 2];

//function prototypes
char *substring(char *string, int position, int length);
void clearArray(int *arrayName, int numElements);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    DEBUG_PRINTLN("beaconArrayCount reset");
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
      char *minor = substring(pHex, 45, 4); //don't really care which beacons are close (on client side)
      if (strcmp(major, "1388") == 0)
      {
        if (beaconArrayCount/2 < MAX_BEACONS)
        {
          DEBUG_PRINT("beacon addded, beaconArrayCount: ");
          DEBUG_PRINTLN(beaconArrayCount);
          beaconsDataArray[beaconArrayCount] = atoi(minor);
          beaconsDataArray[beaconArrayCount + 1] = rssiRecieved;
          beaconArrayCount += 2;
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
  while (WiFi.status() != WL_CONNECTED)
  {
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
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
  //clear beacons array
  clearArray(beaconsDataArray, beaconArrayCount);
  beaconArrayCount = 0;

  //bluetooth scan
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  DEBUG_PRINT("Devices found: ");
  DEBUG_PRINTLN(foundDevices.getCount());

  //create new wifi client
  WiFiClient client;
  if (!client.connect(host, port))
  {
    DEBUG_PRINTLN("Connection to host failed");
    delay(1000);
    return;
  }
  DEBUG_PRINTLN("Connected to server successful!");

  Serial.println("beacons:");
  for (int a = 0; a < beaconArrayCount; a++)
  {
    Serial.println(beaconsDataArray[a]);
  }

  for (int x = 0; x < beaconArrayCount; x++)
  {
    client.print(beaconsDataArray[x]);
    client.print("\n");
  }
  DEBUG_PRINTLN("Disconnecting...");
  client.stop();

  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(200);
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

void clearArray(int *arrayName, int numElements)
{
  for (int x = 0; x < numElements; x++)
  {
    DEBUG_PRINT("abbout to reset: ");
    DEBUG_PRINTLN(arrayName[x]);
    arrayName[x] = 0;
  }
}