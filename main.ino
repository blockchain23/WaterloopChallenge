#include <Wire.h>

const int devAddr = 0x5A;
const int commBytes = 32;
DataPacket dataPacket;
Pod pod;

struct Pod {
  long vel;
  long pos;
  long accel;
  int state;
  byte sendData[3];
};

struct DataPacket {
  byte startingByte;
  byte errorStates;
  byte currentPodState;
  
  //Where each element is a byte for the 32 bit data.
  byte timeStamp[4];
  byte accelFront[4];
  byte accelMiddle[4];
  byte accelRear[4];
  byte tempBraking[4];
  byte tempPropul[4];
  byte tempMotherboard[4];
  byte endingByte;
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

DataPacket readData () {
  uint8_t &buffer[32];
  for (int i = 0; i < 32; i++) {
    buffer[i] = Wire.read();
  }
  dataPacket = (DataPacket)buffer
  return dataPacket;
}

struct DataPacket {
  uint8_t startingByte;
  uint8_t errorStates;
  uint8_t currentPodState;

  //Where each element is a byte for the 32 bit data.
  float32_t timeStamp;
  float32_t accelFront;
  float32_t accelMiddle;
  float32_t accelRear;
  float32_t tempBraking;
  float32_t tempPropul;
  float32_t tempMotherboard;
  uint8_t endingByte;
};

controlPod() {
  if (pos > (30000000-50000) && (state != 4 || state != 5)) {
    pod.sendData[1] = 0x04;
    pod.state = 4;
    Wire.write(pod.sendData,3);
  }
}

void setup() {
  // put your setup code here, to run once:
  pod.state = 0;
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(devAddr);
  pod.sendData[0] = 0x56;
  pod.sendData[1] = 0x01;
  pod.sendData[2] = 0x23;
  pod.vel = pod.pos = 0;
  Wire.write(pod.sendData,3);
  pod.state = 1;
  pod.sendData[1] = 0x02;
  Wire.write(pod.sendData,3);
  pod.state = 2;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (vel > 0) {
    dataPacket = readData();
    calcPosVel();
    sendToServer();
    controlPod();
    delay(20);
  }
}
