//www.elegoo.com
//2016.12.08
#include <ArduinoInit.h>
#include <SendOnlySoftwareSerial.h>
#include "SR04.h"

//pins:
unsigned int bottomLight = 4;
unsigned int leftBumper = 3;
unsigned int rightBumper = 2;
unsigned int floorLightSensor = 0;
unsigned int leftLightSensor = 1;
unsigned int middleLightSensor = 2;
unsigned int rightLightSensor = 3;
unsigned int directionalLightSensor = 4;

//Distance Sensors
//left sensor
unsigned int leftSensorTrigPin = 7;
unsigned int leftSensorEchoPin = 8;
//middle sensor
unsigned int middleSensorTrigPin = 9;
unsigned int middleSensorEchoPin = 10;
//right sensor
unsigned int rightSensorTrigPin = 11;
unsigned int rightSensorEchoPin = 12;

long a;
unsigned int sensor;

void setup() {
  configArduino ( );
  Serial.begin(115200);
  void ultraSensors();
  void lightSensors();
  pinMode(bottomLight, OUTPUT);
  digitalWrite(bottomLight, HIGH);
}

void loop() {
  ultraSensors();
  //delay(2000);
  lightSensors();
  delay(1000);
}

void ultraSensors() {
  SR04 leftUltraSensor = SR04(leftSensorEchoPin, leftSensorTrigPin);
  SR04 middleUltraSensor = SR04(middleSensorEchoPin, middleSensorTrigPin);
  SR04 rightUltraSensor = SR04(rightSensorEchoPin, rightSensorTrigPin);
  a = leftUltraSensor.Distance();
  Serial.print(a);
  Serial.print("cm left ");
  a = middleUltraSensor.Distance();
  Serial.print(a);
  Serial.print("cm middle ");
  a = rightUltraSensor.Distance();
  Serial.print(a);
  Serial.println("cm right ");
  //delay(2000);

}

void lightSensors() {
  
  sensor = readADC(floorLightSensor);
  Serial.print(sensor);
  Serial.print("FloorValue ");
  sensor = readADC(leftLightSensor);
  Serial.print(sensor);
  Serial.print("leftValue ");
  sensor = readADC(middleLightSensor);
  Serial.print(sensor);
  Serial.print("middleValue ");
  sensor = readADC(rightLightSensor);
  Serial.print(sensor);
  Serial.println("rightValue ");
  
  //delay(2000);
}

