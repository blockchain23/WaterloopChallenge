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
  for (int i = 0; i < 32; i++) {
    DataPacket dataPacket;
    if(i==0{
      dataPacket.startingByte = Wire.read()
    }
    else if(i==1){
      dataPacket.errorStates = Wire.read()
    }
    else if(i==2){
      dataPacket.currentPodState = Wire.read()
    }
     else if(7 > i > 2){
      dataPacket.timeStamp[i-3] = Wire.read()
    }
    else if(11 > i > 6){
      dataPacket.accelFront[i-7] = Wire.read()
    }
    else if(15 > i > 10){
      dataPacket.accelMiddle[i-11] = Wire.read()
    }
    else if(19 > i > 14){
      dataPacket.accelRear[i-15] = Wire.read()
    }
    else if(23 > i > 18){
      dataPacket.tempBraking[i-19] = Wire.read()
    }
    else if(27 > i > 22){
      dataPacket.tempPropul[i-23] = Wire.read()
    }
    else if(31 > i > 26){
      dataPacket.tempMotherboard[i-23] = Wire.read()
    }
    else{
      dataPacket.endingByte = Wire.read()
    }
  }
  return dataPacket;
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
    dataPacket = readData();
    calcPosVel();
    sendToServer();
    controlPod();
    delay(20);
  }
}
