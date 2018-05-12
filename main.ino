#include <Wire.h>

const int devAddr = 0x5A;
const int commBytes = 32;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(devAddr);
  Wire.requestFrom(devAddr, commBytes);
  for (int i = 0; i < 32; i++) {
    Serial.println(Wire.read());
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
