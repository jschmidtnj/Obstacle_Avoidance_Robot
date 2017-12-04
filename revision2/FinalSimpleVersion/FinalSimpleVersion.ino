#include <ArduinoInit.h>
#include <SendOnlySoftwareSerial.h>

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
unsigned int fullRotationDuration = 800; //for 360 degree rotation
//light sensors:
unsigned int threshold = 0; //threshold for directional and middle light sensors
//floor sensor:
unsigned int floorThreshold = 20000; //black = 20000
unsigned int sensor = 0; //input from floor sensor
unsigned int initialPosition = 0; //initial field position (1 = black, 0 = white)
unsigned int currentFieldPosition = 0; //current field position
unsigned int positionChanged = 0; //position changed boolean

void setup() {
  // put your setup code here, to run once:
  //initialize arduino baseline configuration
  configArduino ( );
  //turn on light on the bottom
  pinMode(bottomLight, OUTPUT);
  digitalWrite(bottomLight, HIGH);
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
  attachInterrupt(0,rightBumperHit, LOW); //routine for D2 bumper
  attachInterrupt(1,leftBumperHit, LOW); //routine for D3 bumper
  initialization();
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
  Serial.println(initialPosition);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (positionChanged == 0){
    //if the robot is still on its side of the field
    goToCorrectDirection(directionalLightSensor); //gets the robot in the right direction
    forward(100); //forward max
    delay(10000); //go forward for this duration (ms)
    floorsensor(); //checks if position on field changed
  }
  else{
    //IN ENEMY TERRITORY!!!
    delay(1000); //keep going for 1 second
    motorsOff(); //stop
    goToCorrectDirection(directionalLightSensor); //gets the robot in the right direction
    forward(100); //go forward max
    delay(10000); //go forward for this duration (ms)
    floorsensor(); //checks to see if position on field changed again
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
    pause(5);
    while(readInput(leftBumper) == 0 || readInput(rightBumper) == 0){
      backward(100); //go backwards
      pause(10);
    }
    motorsOff(); //turn both motors off
    rotateRight(100); //turn right
    pause(250); //duration
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
    pause(5);
    while(readInput(leftBumper) == 0 || readInput(rightBumper) == 0){
      backward(100); //go backwards
      pause(5);
    }
    motorsOff(); //turn both motors off
    rotateLeft(100); //turn left
    pause(250); //duration
    motorsOff(); //stop
  }
}

void bothBumpersHit( )
{
  Serial.println("both_bumpers_hit");
  motorsOff(); //turn both motors off
  pause(100); //pause 100 ms
  backward(100);
  pause(5);
  while(readInput(leftBumper) == 0 || readInput(rightBumper) == 0){
    backward(100); //go backwards
    pause50);
  }
  motorsOff(); //turn both motors off
  rotateLeft(100); //turn left
  pause(250); //duration
  motorsOff(); //stop
  return;
}


