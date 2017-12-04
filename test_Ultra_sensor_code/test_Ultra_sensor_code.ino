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


void setup() {
  // put your setup code here, to run once:
  configArduino ( );
  void turnLeft(int theSpeed);
  void turnRight(int theSpeed);
  void motorsOff();  //turn motors off
  void forward(int theSpeed);  //go forward at theSpeed
  void rotateLeft(int theSpeed);  //go left at theSpeed
  void rotateRight(int theSpeed);  //go right at theSpeed
  void backward(int theSpeed);  //go backward at theSpeed
  void leftBumperHit(); //left bumper hit
  void rightBumperHit();  //right bumper hit
  void bothBumpersHit();  //both bumpers hit
  
  attachInterrupt(0,leftBumperHit, LOW); //routine for left bumper
  attachInterrupt(1,rightBumperHit, LOW); //routine for right bumper
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

void turnLeft(int theSpeed){
  Serial.println("rotateLeft");
  motors('1', 'A', theSpeed); //turn left
}

void turnRight(int theSpeed){
  Serial.println("rotateRight");
  motors('2', 'A', theSpeed); //turn left
}
















void smartMove(String typeOfSensor){
  if (typeOfSensor == "ultra"){
    
    //get average value
    unsigned int leftValue;
    for(int i = 0; i < numSamplesInAverage; i++){
      leftValue = echo(leftSensorTrigPin, leftSensorEchoPin);
    }
    //Serial.print("left: " + String(leftValue));
    
    unsigned int middleValue;
    for(int i = 0; i < numSamplesInAverage; i++){
      middleValue = echo(middleSensorTrigPin, middleSensorEchoPin);
    }
    //Serial.print("middle: " + String(middleValue));
    
    unsigned int rightValue;
    for(int i = 0; i < numSamplesInAverage; i++){
      rightValue = echo(rightSensorTrigPin, rightSensorEchoPin);
    }
    //Serial.print("right: " + String(rightValue) + "\n");
    
    getAngleForMotors(leftValue, middleValue, rightValue, angleUltraSensors, typeOfSensor);
    motorFromAngle(angleOfBestMovement);
    //turnLeft(left);
    //turnRight(right);
    delay(50);
  }
  /*
  else if(typeOfSensor == "light"){
    getAngleForMotors(readADC(leftLightSensor), readADC(middleLightSensor), readADC(rightLightSensor), angleLightSensors, typeOfSensor);
    motorFromAngle(angleOfBestMovement);
    turnLeft(left);
    turnLeft(right);
  }
  */
}

//SMART SENSORS!!!!! (ULTRA + LIGHT)
void getAngleForMotors(double leftSensorValue, double middleSensorValue, double rightSensorValue, double angle, String typeOfSensor)
{
  //left ultra sensor
  double lx = leftSensorValue * cos(angle);
  double ly = leftSensorValue * sin(angle);
  //middle ultra sensor
  double m = middleSensorValue; //always positive
  //right ultra sensor
  double rx = rightSensorValue * cos(angle); //these are both negative
  double ry = rightSensorValue * sin(angle);
  //resultant vector
  double resultantX = -1 * abs(lx) + abs(rx);
  double resultantY = abs(ly) + abs(ry);
  Serial.println("x " + String(lx) + ' ' + String(m) + ' ' + String(rx));
  Serial.println("y " + String(ly) + ' ' + String(m) + ' ' + String(ry) + '\n');
  //Serial.println("x " + String(resultantX));
  //Serial.println("y " + String(resultantY) + "\n");
  if (typeOfSensor == "ultra"){
    magnitudeResultantVector = tan(resultantY/resultantX);
    //Serial.println(magnitudeResultantVector);
  }
  angleOfBestMovement = atan2(resultantY, resultantX) * 180 / PI;
  /*
  if (angleOfBestMovement < 0){
    angleOfBestMovement = 90 + angleOfBestMovement;
  }
  */
}


//Angle to motor output
void motorFromAngle (int angle)
{
  //Serial.println(angle);
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


//get distance:
long echo(int trig, int echo){
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


void loop() {
  // put your main code here, to run repeatedly:
  smartMove("ultra");
}

