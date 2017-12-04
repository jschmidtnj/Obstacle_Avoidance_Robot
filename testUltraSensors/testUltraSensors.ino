//www.elegoo.com
//2016.12.08
#include "SR04.h"
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

SR04 leftUltraSensor = SR04(leftSensorEchoPin,leftSensorTrigPin);
SR04 middleUltraSensor = SR04(middleSensorEchoPin,middleSensorTrigPin);
SR04 rightUltraSensor = SR04(rightSensorEchoPin,rightSensorTrigPin);
long a;

void setup() {
   Serial.begin(9600);
   delay(1000);
}

void loop() {
  /*
   a=leftUltraSensor.Distance();
   Serial.print(a);
   Serial.println("cm left");
   a=middleUltraSensor.Distance();
   Serial.print(a);
   Serial.println("cm middle");
   */
   a=rightUltraSensor.Distance();
   Serial.print(a);
   Serial.println("cm left");
   delay(10);
}
