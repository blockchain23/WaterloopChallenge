#include <Wire.h>

const int devAddr = 0x5A;
const int commBytes = 32;

struct pod {
  long vel;
  long pos;
  int state;
};

void calcPosVel() {
  long accel = (dataPacket.accelFront + dataPacket.accelMid + dataPacket.accelRear)/3.0;
  pod.pos += vel*0.002 + 0.5*accel*0.002*0.002;
  pod.vel = vel + accel*0.002;  
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(devAddr);
  dataPacket.vel = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (vel > 0) {
    readData();
    calcPosVel();
    sendToServer();
    controlPod();
    delay(20);
  }
}
