
#include <SendOnlySoftwareSerial.h>

#include <ArduinoInit.h>


void setup()

{

  configArduino();

}

void loop()

{

  turnLeft(100);
}


void rotateLeft(int theSpeed){
  if(theSpeed < 25){
    theSpeed = 25;
  }
  else if(theSpeed > 100){
    theSpeed = 100;
  }
  Serial.println("rotateLeft");
  motors('2', 'B', theSpeed); //turn left
  motors('1', 'A', theSpeed); //turn right
}

void rotateRight(int theSpeed){
  if(theSpeed < 25){
    theSpeed = 25;
  }
  else if(theSpeed > 100){
    theSpeed = 100;
  }
  Serial.println("rotateRight");
  motors('1', 'B', theSpeed); //turn left
  motors('2', 'A', theSpeed); //turn right
}

void turnLeft(int theSpeed){
  Serial.println("rotateLeft");
  if(theSpeed > 100){
    motors('1', 'A', 100); //turn left
  }
  else if(theSpeed < 30){
    motors('1', 'O', 25); //turn off
  }
  else{
    motors('1', 'A', theSpeed); //turn left
  }
}
