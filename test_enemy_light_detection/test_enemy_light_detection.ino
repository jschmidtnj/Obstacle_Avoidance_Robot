#include <ArduinoInit.h>
#include <SendOnlySoftwareSerial.h>

//pins:
unsigned int bottomLight = 4;
unsigned int leftBumper = 3;
unsigned int rightBumper = 2;
unsigned int floorLightSensor = 0;
unsigned int leftLightSensor = 1;
unsigned int middleLightSensor = 2;
unsigned int rightLightSensor = 3;
unsigned int directionalLightSensor = 4;

//speed:
unsigned int fullRotationDuration = 800; //for 360 degree rotation
//light sensors:
unsigned int threshold = 0; //threshold for directional and middle light sensors
//floor sensor:
unsigned int floorThreshold = 1800; //white threshold
unsigned int sensor = 0; //input from floor sensor
unsigned int initialPosition = 0; //initial field position (1 = black, 0 = white)
unsigned int currentFieldPosition = 0; //current field position
unsigned int positionChanged = 0; //position changed boolean

unsigned int middleLightThreshold = 800;
unsigned int a = 2;

//ULTRA sensors:
unsigned int thresholdEnemyLight = 1000;
double magnitudeResultantVector = 0;
double angleUltraSensors = 31;
double angleOfBestMovement = 0; //angle to move in
unsigned int right = 0;
unsigned int left = 0;
//left sensor
unsigned int leftSensorTrigPin = 7;
unsigned int leftSensorEchoPin = 8;
//middle sensor
unsigned int middleSensorTrigPin = 9;
unsigned int middleSensorEchoPin = 10;
//right sensor
unsigned int rightSensorTrigPin = 11;
unsigned int rightSensorEchoPin = 12;

unsigned int numSamplesInAverage = 10;
unsigned int multiplierUltra = 1.5;
unsigned int ultraCusion = 4; //cm



void setup() {
  // put your setup code here, to run once:
  configArduino ( );
  digitalWrite(bottomLight, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  moveWithLight();
  //pause(100);
}

void moveWithLight(){
  //get average value
  unsigned int leftValue;
  for (int i = 0; i < numSamplesInAverage; i++) {
    leftValue = readADC (leftLightSensor);
  }
  leftValue -= 15000;
  Serial.print("left: " + String(leftValue));

  unsigned int middleValue;
  for (int i = 0; i < numSamplesInAverage; i++) {
    middleValue = readADC (middleLightSensor);
  }
  Serial.print("middle: " + String(middleValue));

  unsigned int rightValue;
  for (int i = 0; i < numSamplesInAverage; i++) {
    rightValue = readADC (rightLightSensor);
  }
  rightValue -= 30000;
  Serial.print("right: " + String(rightValue) + "\n");

  int amountLeft;
  int amountRight;
  if (leftValue < rightValue) {
    //space to the left
    amountRight = 100;
    amountLeft = (100 * float(leftValue) / rightValue - 75) * 2;
  }
  else {
    amountRight = (100 * float(rightValue) / leftValue - 75) * 2;
    amountLeft = 100;
  }
  if (middleValue < middleLightThreshold){
    amountRight = 100;
    amountLeft = 100;
  }
  Serial.print("left: " + String(amountLeft) + " right: " + String(amountRight) + "\n");
  turnLeft(amountLeft);
  turnRight(amountRight);
  pause(100);
}

//Robot Moving Statements:
void motorsOff( ) {
  Serial.println("motorsOff");
  motors('B', 'O', 100); //turn both motors off
}

void forward(int theSpeed) {
  if (theSpeed < 25) {
    theSpeed = 25;
  }
  else if (theSpeed > 100) {
    theSpeed = 100;
  }
  Serial.println("forward");
  motors('B', 'A', theSpeed); //turn left
}

void backward(int theSpeed) {
  if (theSpeed < 25) {
    theSpeed = 25;
  }
  else if (theSpeed > 100) {
    theSpeed = 100;
  }
  Serial.println("backward");
  motors('B', 'B', theSpeed); //turn left
}

void rotateLeft(int theSpeed) {
  if (theSpeed < 25) {
    theSpeed = 25;
  }
  else if (theSpeed > 100) {
    theSpeed = 100;
  }
  Serial.println("rotateLeft");
  motors('2', 'B', theSpeed); //turn left
  motors('1', 'A', theSpeed); //turn right
}

void rotateRight(int theSpeed) {
  if (theSpeed < 25) {
    theSpeed = 25;
  }
  else if (theSpeed > 100) {
    theSpeed = 100;
  }
  Serial.println("rotateRight");
  motors('1', 'B', theSpeed); //turn left
  motors('2', 'A', theSpeed); //turn right
}

void turnLeft(int theSpeed) { //speed for right motor to turn
  Serial.println("rotateLeft");
  if (theSpeed > 100) {
    motors('2', 'A', 100); //turn left
  }
  else if (theSpeed < 30) {
    motors('2', 'O', 25); //turn off
  }
  else {
    motors('2', 'A', theSpeed); //turn left
  }
}

void turnRight(int theSpeed) { //speed for right motor to turn
  Serial.println("rotateRight");
  if (theSpeed > 100) {
    motors('1', 'A', 100); //turn right
  }
  else if (theSpeed < 30) {
    motors('1', 'O', 25); //turn off
  }
  else {
    motors('1', 'A', theSpeed); //turn right
  }
}
