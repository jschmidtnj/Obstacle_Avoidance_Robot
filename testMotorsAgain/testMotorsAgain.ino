#include <ArduinoInit.h>
#include <SendOnlySoftwareSerial.h>

void setup() {
  // put your setup code here, to run once:
  configArduino();
}

void loop() {
  // put your main code here, to run repeatedly:
  turnRight(100);
}

void turnLeft(int theSpeed){
  Serial.println("rotateLeft");
  motors('2', 'A', theSpeed); //turn left
}

void turnRight(int theSpeed){
  Serial.println("rotateRight");
  motors('1', 'A', theSpeed); //turn right
}
