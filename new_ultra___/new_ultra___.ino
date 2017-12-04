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
unsigned int floorThreshold = 2500; //white threshold
unsigned int sensor = 0; //input from floor sensor
unsigned int initialPosition = 0; //initial field position (1 = black, 0 = white)
unsigned int currentFieldPosition = 0; //current field position
unsigned int positionChanged = 0; //position changed boolean

unsigned int middleLightThreshold = 800;
unsigned int a = 2;

//ULTRA sensors:
unsigned int thresholdEnemyLight = 10000;
//double magnitudeResultantVector = 0;
//double angleUltraSensors = 31;
//double angleOfBestMovement = 0; //angle to move in
//unsigned int right = 0;
//unsigned int left = 0;
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
  attachInterrupt(1, leftBumperHit, LOW); //routine for left bumper
  attachInterrupt(0, rightBumperHit, LOW); //routine for right bumper
  digitalWrite(bottomLight, HIGH);
  pause(100);
  initialization();
}


void loop() {
  // put your main code here, to run repeatedly:
  if (positionChanged == 0){
    //if the robot is still on its side of the field
    goToCorrectDirection(directionalLightSensor); //gets the robot in the right direction
    for(int i = 0; i < 30; i ++){
      Serial.println("Move with Ultra");
      moveWithUltra();
    }
    floorsensor(); //checks if position on field changed
  }
  else{
    //IN ENEMY TERRITORY!!!
    for(int i = 0; i < 30; i ++){
      unsigned int middleValue;
      for (int i = 0; i < numSamplesInAverage / 2; i++) {
        middleValue = readADC (middleLightSensor);
      }
      if (middleValue < thresholdEnemyLight){
        Serial.println("Move with Light_Sensors");
        moveWithLight();
      }
      else{
        Serial.println("Move with Ultra");
        moveWithUltra();
      }
    }
    floorsensor(); //checks to see if position on field changed again
  }
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
  unsigned int initialTime = millis();
  while(correctInitialDirection == 0){
    rotateLeft(80); //rotate left
    sensor = readADC(pin);
    if (sensor < threshold + 3000){ //if sensor reading is greater than threshold
      motorsOff();
      Serial.println("correct direction");
      correctInitialDirection = 1;  //in right direction
    }
    if (millis() - initialTime > 1500){
      motorsOff();
      Serial.println("give up");
      correctInitialDirection = 1;  //in right direction
    }
  }
}

//FLOOR sensor
void initialization()
{
  unsigned int numSamples = 10;
  unsigned int average = 0;
  for (int i = 0; i < numSamples; i++){
    average += readADC (floorLightSensor);
  }
  average /= numSamples;
  if (average > floorThreshold){
    initialPosition = 1; //in white territory
  }
  else{
    initialPosition = 0; //in black territory
  }
  Serial.println(initialPosition);
}


void floorsensor( )
{
  sensor = readADC(floorLightSensor);
  if (sensor > floorThreshold){
    currentFieldPosition = 1; //in white territory
  }
  else{
    currentFieldPosition = 0; //in black territory
  }
  //checks if position changed
  if (currentFieldPosition != initialPosition){
    positionChanged = 1;
  }
  if (currentFieldPosition == initialPosition){
    positionChanged = 0;
  }
}

void moveWithUltra() {
  //get average value
  unsigned int leftValue;
  for (int i = 0; i < numSamplesInAverage; i++) {
    leftValue = echo(leftSensorTrigPin, leftSensorEchoPin);
  }
  //Serial.print("left: " + String(leftValue));

  unsigned int middleValue;
  for (int i = 0; i < numSamplesInAverage; i++) {
    middleValue = echo(middleSensorTrigPin, middleSensorEchoPin);
  }
  //Serial.print("middle: " + String(middleValue));

  unsigned int rightValue;
  for (int i = 0; i < numSamplesInAverage; i++) {
    rightValue = echo(rightSensorTrigPin, rightSensorEchoPin);
  }

  if (middleValue < ultraCusion + 2 && rightValue < ultraCusion + 2 && leftValue < ultraCusion + 2) {
    //stuck in a corner!!!
    motorsOff();
    backward(100);
    pause(1000);
    rotateRight(100); //make this better later
    pause(250);
  }
  else {
    //prevent hitting walls
    if (middleValue < ultraCusion) {
      backward(100);
      pause(100);
      if (leftValue > rightValue) {
        rotateLeft(100);
        pause(100);
      }
      else {
        rotateRight(100);
        pause(100);
      }
    }
    else if (leftValue < ultraCusion) {
      backward(1000);
      pause(100);
      rotateRight(100);
      pause(100);
    }
    else if (rightValue < ultraCusion) {
      backward(100);
      pause(100);
      rotateLeft(100);
      pause(100);
    }
  }

  int amountLeft;
  int amountRight;
  if (leftValue > rightValue) {
    //space to the left
    amountRight = 100;
    amountLeft = 100 * rightValue / leftValue * multiplierUltra;
  }
  else {
    amountRight = 100 * leftValue / rightValue * multiplierUltra;
    amountLeft = 100;
  }
  //Serial.print("left: " + String(amountLeft) + " right: " + String(amountRight) + "\n");
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

//get distance:
long echo(int trig, int echo) {
  pinMode(trig, OUTPUT);// set the trig pin to output (Send sound waves)
  pinMode(echo, INPUT);// set the echo pin to input (recieve sound waves)
  unsigned int duration, distance; // start the scan
  digitalWrite(trig, LOW);
  delayMicroseconds(2); // delays are required for a succesful sensor operation.
  digitalWrite(trig, HIGH);

  delayMicroseconds(10); //this delay is required as well!
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration / 2) / 29.1;   // convert the distance to centimeters.
  //Serial.print(String(distance) + "    ");
  return distance;
}





//Interrupts:
void leftBumperHit( )
{
  Serial.println("left_bumper_hit");
  if (readInput(leftBumper) == 1 && readInput(rightBumper) == 1) {
    //no bumper was hit
    pause(10);
  }
  else if (readInput(rightBumper) == 0) {
    bothBumpersHit();
  }
  else {
    motorsOff(); //turn both motors off
    pause(10); //pause 10 ms
    backward(100);
    pause(1000);
    motorsOff(); //turn both motors off
    rotateRight(100); //turn right
    pause(100); //duration
    motorsOff();
  }
}

void rightBumperHit( )
{
  Serial.println("right_bumper_hit");
  if (readInput(leftBumper) == 1 && readInput(rightBumper) == 1) {
    //no bumper was hit
    pause(10);
  }
  else if (readInput(leftBumper) == 0) {
    bothBumpersHit();
  }
  else {
    motorsOff(); //turn both motors off
    pause(10); //pause 10 ms
    backward(100);
    pause(1000);
    motorsOff(); //turn both motors off
    rotateLeft(100); //turn left
    pause(100); //duration
    motorsOff();
  }
}

void bothBumpersHit()
{
  motorsOff(); //turn both motors off
  pause(10); //pause 10 ms
  backward(100);
  pause(1000);
  motorsOff(); //turn both motors off
  rotateLeft(250); //turn left
  pause(100); //duration
  motorsOff();
}

