#include <Wire.h>

const int devAddr = 0x5A;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(devAddr);
  Wire.requestFrom(devAddr,);
  Wire.
}

void loop() {
  // put your main code here, to run repeatedly:

}
