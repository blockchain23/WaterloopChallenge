#include <Wire.h>

const int devAddr = 0x5A;
const int commBytes = 32;

struct pod {
  long vel;
  long pos;
  long accel;
  int state;
};

void calcPosVel() {
  pod.accel = (dataPacket.accelFront + dataPacket.accelMid + dataPacket.accelRear)/3.0;
  pod.pos += vel*0.002 + 0.5*accel*0.002*0.002;
  pod.vel = vel + accel*0.002;  
}

void sendToServer() {
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["velocity"] = pod.vel;
  root["distance"] = pod.pos;
  root["acceleration"] = pod.accel;
  root["propulsion_temp"] = dataPacket.propulsionTemp;
  root["braking_temp"] = dataPacket.brakingTemp;
  root["motherboard_temp"] = dataPacket.motherboardTemp;
  root["pod_state"] = pod.state;
  root["time_since_start"] = dataPacket.time;
  root.printTo(Serial);
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(devAddr);
  pod.vel = pod.pos = 0;
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
