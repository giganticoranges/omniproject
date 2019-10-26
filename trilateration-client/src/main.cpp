#include <Arduino.h>

#include <WiFi.h>

//bluetooth imports
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//#include <stdio.h>
#include <string.h>
//end bluetooth imports

#define MAX_BEACONS 20

//bluetooth variables
int scanTime = 1; //In seconds
BLEScan *pBLEScan;
//end bluetooth variables

const char *ssid = "aicv_devices";  //wifi network name
const char *password = "AICVLab1!"; //wifi network password

const uint16_t port = 8090;         //port to connect to
const char *host = "192.168.0.100"; //host ip address (ifconfig on the host pc)
int x = 3;                          //counter
int y = 0;

int beacons[MAX_BEACONS/2];

//substring function prototype
char *substring(char *string, int position, int length);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  int beaconCount = 0;
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
      char *minor = substring(pHex, 45, 4); //don't really care which beacons are close (on client side)
      if (strcmp(major, "1388") == 0)
      {
        if(beaconCount < MAX_BEACONS/2)
        {
          Serial.print("beacon addded, beaconCount: ");
          Serial.println(beaconCount);
          beacons[beaconCount] = atoi(minor);
          beacons[beaconCount + 1] = rssiRecieved;
          beaconCount += 2;
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
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

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

  //bluetooth scan
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());

  //create new wifi client
  WiFiClient client;

  Serial.println("beacons:\n");
  for (int a = 0; a<8; a ++)
  {
    
    Serial.println(beacons[a]);
  }

  if (!client.connect(host, port))
  {

    Serial.println("Connection to host failed");

    delay(1000);
    return;
  }

  Serial.println("Connected to server successful!");

  for (x = 0; x < 8; x++)
  {
    client.print(beacons[x]);
    client.print("\n");
  }
  Serial.println("Disconnecting...");
  client.stop();

  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}

char *substring(char *string, int position, int length)
{
  char *pointer;
  int c;

  pointer = (char *)malloc(length + 1);

  if (pointer == NULL)
  {
    printf("Unable to allocate memory.\n");
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