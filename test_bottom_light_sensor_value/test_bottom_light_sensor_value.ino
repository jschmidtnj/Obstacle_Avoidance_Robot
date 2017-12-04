#include <ArduinoInit.h>
#include <SendOnlySoftwareSerial.h>

unsigned int floorLightSensor = 0;
//floor sensor:
unsigned int floorThreshold = 2500; //white threshold
unsigned int sensor = 0; //input from floor sensor
unsigned int initialPosition = 0; //initial field position (1 = black, 0 = white)
unsigned int currentFieldPosition = 0; //current field position
unsigned int positionChanged = 0; //position changed boolean

void setup() {
  // put your setup code here, to run once:
  unsigned int bottomLight = 4;
  digitalWrite(bottomLight, HIGH);
  configArduino ( );
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  Serial.println(readADC (floorLightSensor));
  pause(500);
  */
  initialization();
  Serial.println(initialPosition);
  pause(1000);
}

void initialization()
{
  unsigned int numSamples = 10;
  unsigned int average = 0;
  for (int i = 0; i < numSamples; i++){
    average += readADC (floorLightSensor);
  }
  average /= numSamples;
  Serial.println(average);
  if (average > floorThreshold){
    initialPosition = 1; //in white territory
  }
  else{
    initialPosition = 0; //in black territory
  }
  Serial.println(initialPosition);
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
