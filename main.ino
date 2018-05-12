#include <Wire.h>

const int devAddr = 0x5A;
const int commBytes = 32;

struct pod {
  int vel;
  int pos;
  int state;
};

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(devAddr);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (vel > 0) {
    readData();
    calcPosVel();
    sendToServer();
    controlPod();
  }
}
