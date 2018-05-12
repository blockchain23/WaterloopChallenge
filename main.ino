#include <Wire.h>
#include <ArduinoJson.h>

const int devAddr = 0x5A;
const int commBytes = 32;

typedef struct Pod {
  float vel;
  float pos;
  float accel;
  float prevAvg;
  int state;
  byte sendData[3];
};

typedef struct DataPacket {
  uint8_t startingByte;
  uint8_t errorStates;
  uint8_t currentPodState;

  //Where each element is a byte for the 32 bit data.
  float timeStamp;
  float accelFront;
  float accelMiddle;
  float accelRear;
  float tempBraking;
  float tempPropul;
  float tempMotherboard;
  uint8_t endingByte;
};

DataPacket *dataPacket;
Pod pod;

float filter(float num, float prevAvg){
  return (num + prevAvg)/2;
}

void calcPosVel(DataPacket* dataPacket) {
  pod.accel = (dataPacket->accelFront + dataPacket->accelMiddle + dataPacket->accelRear)/3.0;
  pod.accel = filter(pod.accel, pod.prevAvg);
  pod.vel += pod.accel*0.02;
  pod.pos += pod.vel*0.02 + 0.5*pod.accel*0.02*0.02;
}

void sendToServer() {
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["velocity"] = pod.vel;
  root["distance"] = pod.pos;
  root["acceleration"] = pod.accel;
  root["propulsion_temp"] = dataPacket->tempPropul;
  root["braking_temp"] = dataPacket->tempBraking;
  root["motherboard_temp"] = dataPacket->tempMotherboard;
  root["pod_state"] = pod.state;
  root["time_since_start"] = dataPacket->timeStamp;
  root.printTo(Serial);
}

DataPacket* readData(DataPacket *dataPacket) {
  uint8_t buffer[32];
  Wire.requestFrom(0x5A,32);
  for (int i = 0; i < 32; i++) {
    buffer[i] = Wire.read();
    //Serial.println(buffer[i]);
  }
  //Serial.println(buffer[3]);
  dataPacket = (DataPacket*)buffer;
  //memcpy(&buffer,&dataPacket,sizeof(dataPacket));
  //Serial.println(dataPacket->timeStamp);
  //Serial.print("\n");
  return dataPacket;
}


void controlPod() {
  if (pod.pos > (30000000-5000000) && (pod.state != 4 || pod.state != 5)) {
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
  pod.sendData[1] = 1;
  pod.sendData[2] = 0x23;
  pod.vel = pod.pos = 0;
  Wire.write(pod.sendData,3);
  Wire.endTransmission();
  //delay(100);
  Wire.beginTransmission(devAddr);
  pod.state = 1;
  pod.sendData[1] = 0x02;
  Wire.write(pod.sendData,3);
  pod.state = 2;
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:
  pod.vel = 0.001;
  pod.prevAvg = 0;
  while (pod.vel > 0) {
    dataPacket = readData(dataPacket);
    //Serial.println(dataPacket->timeStamp);
    calcPosVel(dataPacket);
    //sendToServer();
    controlPod();
    Serial.print("vel: ");
    Serial.print(pod.vel);
    Serial.print(" accel: ");
    //Serial.print(dataPacket->accelRear);
    Serial.print(pod.accel);
    Serial.print(" pos: ");
    Serial.print(pod.pos);
    Serial.print(" time: ");
    Serial.print(dataPacket->timeStamp);
    Serial.print(" state: ");
    Serial.print(dataPacket->currentPodState);
    Serial.print("\n");//,pod.accel,pod.pos);// + pod.vel + " accel: " + pod.accel + "pos: " + pod.pos);
    delay(20);
  }
}
