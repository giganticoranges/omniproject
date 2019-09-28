#include <Arduino.h>

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(2,HIGH);
  Serial.println("Led on");
  delay(1000);
  digitalWrite(2,LOW);
  Serial.println("Led off");
  delay(1000);
}