#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//bluetooth imports
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

int scanTime = 3; //In seconds
int counter = 0;
String toPrint;
BLEScan *pBLEScan;

char *substring(char *string, int position, int length);
void printText(String text);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
      if (strcmp(major, "1388") == 0 && strcmp(minor, "0001") == 0)
      {
        //Serial.println(rssiRecieved);
        toPrint = rssiRecieved;
      }
      free(pHex);
    }
  }
};

void printText(String text);

void loop() {
  printText(toPrint);
  display.setCursor(0,30);             // Start at top-left corner
  display.println(counter);
  display.display();
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  counter++;
}



void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(95); // less or equal setInterval value

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

void printText(String text){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println((text));
  display.display();
}