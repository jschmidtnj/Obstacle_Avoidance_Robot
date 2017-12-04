#include <ArduinoInit.h>
#include <SendOnlySoftwareSerial.h>
#include "SR04.h"

/* Parameters:
   a = forward direction
   b = backward direction
   motor 1 = left motor
   motor 2 = right motor
   100 = full speed
   25 = lowest speed
*/

//Define all variables in this program here:
//use unsigned ints for all light sensors
//all are set to 16 bits

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
unsigned int fullRotationDuration = 100; //for 360 degree rotation
//light sensors:
unsigned int threshold = 0; //threshold for directional and middle light sensors
//floor sensor:
unsigned int floorThreshold = 20000; //black = 20000
unsigned int sensor = 0; //input from floor sensor
unsigned int initialPosition = 0; //initial field position (1 = black, 0 = white)
unsigned int currentFieldPosition = 0; //current field position
unsigned int positionChanged = 0; //position changed boolean
double angleLightSensors = 30;
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
//initialization of ultra sensors
SR04 leftUltraSensor = SR04(leftSensorEchoPin,leftSensorTrigPin);
SR04 middleUltraSensor = SR04(middleSensorEchoPin,middleSensorTrigPin);
SR04 rightUltraSensor = SR04(rightSensorEchoPin,rightSensorTrigPin);
//other
unsigned int fix = 0;
int timer = 0;

void setup() {
  // put your setup code here, to run once:
  //initialize arduino baseline configuration
  configArduino ( );
  //identify all subroutines here:
  void motorsOff();  //turn motors off
  void forward(int theSpeed);  //go forward at theSpeed
  void rotateLeft(int theSpeed);  //go left at theSpeed
  void rotateRight(int theSpeed);  //go right at theSpeed
  void backward(int theSpeed);  //go backward at theSpeed
  void leftBumperHit(); //left bumper hit
  void rightBumperHit();  //right bumper hit
  void bothBumpersHit();  //both bumpers hit
  void initialization();  //runs once for initial position
  void goToCorrectDirection(int pin);  //rotates robot to optimal direction given light sensor pin #
  void floorsensor(); //checks if position on field changed
  //DIFFERENT
  void smartMove(String typeOfSensor);
  void getAngleForMotors(double leftSensorValue, double middleSensorValue, double rightSensorValue, double angle, String typeOfSensor);
  void motorFromAngle (int angle);
  attachInterrupt(0,leftBumperHit, LOW); //routine for left bumper
  attachInterrupt(1,rightBumperHit, LOW); //routine for right bumper
  initialization();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (positionChanged == 0){
    fix = 0;
    //if the robot is still on its side of the field
    goToCorrectDirection(directionalLightSensor); //gets the robot in the right direction
    int start = millis(); //start time
    int finish = millis() + 5000; //duration (ms)
    while(start < finish){
      smartMove("ultra");
      timer = millis();
    }
    floorsensor(); //checks if position on field changed
  }
  else{
    if (fix == 0){
      //just went through if statement
      //IN ENEMY TERRITORY!!!
      delay(1000); //keep going for duration
      motorsOff(); //stop
    }
    int start = millis(); //start time
    int finish = millis() + 3000; //duration (ms)
    while(start < finish){
      if (thresholdEnemyLight > magnitudeResultantVector){
        smartMove("ultra");
      }
      else{
        smartMove("light");
      }
      timer = millis();
    }
    if (thresholdEnemyLight < magnitudeResultantVector){
        goToCorrectDirection(middleLightSensor);
      }
    floorsensor(); //checks to see if position on field changed again
    fix = 1;
  }
}


void smartMove(String typeOfSensor){
  if (typeOfSensor == "ultra"){
    getAngleForMotors(leftUltraSensor.Distance(), middleUltraSensor.Distance(), rightUltraSensor.Distance(), angleUltraSensors, typeOfSensor);
    motorFromAngle(angleOfBestMovement);
    rotateLeft(left);
    rotateLeft(right);
  }
  else if(typeOfSensor == "light"){
    getAngleForMotors(readADC(leftLightSensor), readADC(middleLightSensor), readADC(rightLightSensor), angleLightSensors, typeOfSensor);
    motorFromAngle(angleOfBestMovement);
    rotateLeft(left);
    rotateLeft(right);
  }
}

//SMART SENSORS!!!!! (ULTRA + LIGHT)
void getAngleForMotors(double leftSensorValue, double middleSensorValue, double rightSensorValue, double angle, String typeOfSensor)
{
  //left ultra sensor
  double lx = leftSensorValue * cos(angle);
  double ly = leftSensorValue * sin(angle);
  //middle ultra sensor
  double m = middleSensorValue;
  //right ultra sensor
  double rx = rightSensorValue * cos(angle);
  double ry = rightSensorValue * sin(angle);
  //resultant vector
  double resultantX = rx - lx;
  double resultantY = ly + m + ry;
  if (typeOfSensor == "ultra"){
    magnitudeResultantVector = tan(resultantY/resultantX);
  }
  angleOfBestMovement = round(atan2(resultantY, resultantX) * 180 / PI);
  if (angleOfBestMovement < 0){
    angleOfBestMovement = 180 + angleOfBestMovement;
  }
}

//Angle to motor output
void motorFromAngle (int angle)
{
  //angle is relative to forward on robot
  if (angle >= 90){
    right = 100;
  }
  else{
    right = angle / 90 * 100;
  }
  if (angle > 90){
    left = (180 - angle) / 90 * 100;
  }
  else{
    left = 100;
  }
}

//FLOOR sensor
void initialization( )
{
  sensor = readADC (floorLightSensor);
  if (sensor > floorThreshold){
    initialPosition = 1; //in black territory
  }
  else{
    initialPosition = 0; //in white territory
  }
}

//gets the maximum value of light sensor by rotating 360 degrees and sets it to a value
void goToCorrectDirection(int pin){
  Serial.println("goCorrectDirection");
  sensor = readADC(pin);
  unsigned int count = 10; //number of samples to test
  rotateLeft(80);
  for(int i = 0; i < count; i++){
    sensor = readADC(pin);
    if(sensor < threshold){
      threshold = sensor;
    }
    delay(fullRotationDuration / count);
  }
  
  unsigned int correctInitialDirection = 0; //not in right direction
  while(correctInitialDirection == 0){
    rotateLeft(80); //rotate left
    sensor = readADC(pin);
    if (sensor < threshold + 7000){ //if sensor reading is greater than threshold
      motorsOff();
      Serial.println("correct direction");
      correctInitialDirection = 1;  //in right direction
    }
  }
}

void floorsensor( )
{
  sensor = readADC(floorLightSensor);
  if (sensor > floorThreshold){
    currentFieldPosition = 1; //in black territory
  }
  else{
    currentFieldPosition = 0; //in white territory
  }
  //checks if position changed
  if (currentFieldPosition != initialPosition){
    positionChanged = 1;
  }
  if (currentFieldPosition == initialPosition){
    positionChanged = 0;
  }
}

//Robot Moving Statements:
void motorsOff( ){
  Serial.println("motorsOff");
  motors('B', 'O', 100); //turn both motors off
}

void forward(int theSpeed){
  Serial.println("forward");
  motors('B', 'A', theSpeed); //turn left
}

void backward(int theSpeed){
  Serial.println("backward");
  motors('B', 'B', theSpeed); //turn left
}

void rotateLeft(int theSpeed){
  Serial.println("rotateLeft");
  motors('1', 'A', theSpeed); //turn left
  motors('2', 'B', theSpeed); //turn right
}

void rotateRight(int theSpeed){
  Serial.println("rotateRight");
  motors('2', 'A', theSpeed); //turn left
  motors('1', 'B', theSpeed); //turn right
}

//Interrupts:
void leftBumperHit( )
{
  Serial.println("left_bumper_hit");
  pause(5); //pause 5ms to allow for bumper contact to bounse
  if(readInput(leftBumper) == 1 && readInput(rightBumper) == 1){
    //no bumper was hit
    pause(10);
  }
  else if(readInput(rightBumper) == 0){
    bothBumpersHit( );
  }
  else{
    motorsOff(); //turn both motors off
    pause(100); //pause 100 ms
    backward(100);
    pause(750);
    while(readInput(leftBumper) == 0 || readInput(rightBumper) == 0){
      backward(100); //go backwards
      pause(10);
    }
    motorsOff(); //turn both motors off
    rotateRight(100); //turn right
    pause(500); //duration
    motorsOff(); //stop
  }
}

void rightBumperHit( )
{
  Serial.println("right_bumper_hit");
  pause(5); //pause 5ms to allow for bumper contact to bounse
  if(readInput(leftBumper) == 1 && readInput(rightBumper) == 1){
    //no bumper was hit
    pause(5);
  }
  else if(readInput(leftBumper) == 0){
    bothBumpersHit( );
  }
  else{
    motorsOff(); //turn both motors off
    pause(100); //pause 100 ms
    backward(100);
    pause(750);
    while(readInput(leftBumper) == 0 || readInput(rightBumper) == 0){
      backward(100); //go backwards
      pause(10);
    }
    motorsOff(); //turn both motors off
    rotateLeft(100); //turn left
    pause(500); //duration
    motorsOff(); //stop
  }
}

void bothBumpersHit( )
{
  Serial.println("both_bumpers_hit");
  motorsOff(); //turn both motors off
  pause(100); //pause 100 ms
  backward(100);
  pause(750);
  while(readInput(leftBumper) == 0 || readInput(rightBumper) == 0){
    backward(100); //go backwards
    pause(10);
  }
  motorsOff(); //turn both motors off
  rotateRight(100); //turn left
  pause(500); //duration
  motorsOff(); //stop
  return;
}


