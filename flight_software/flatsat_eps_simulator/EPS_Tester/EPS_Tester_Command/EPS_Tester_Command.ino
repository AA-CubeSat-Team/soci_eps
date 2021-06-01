#include <Wire.h>

#define EPS_ADDRESS 0x51


//  Set the desired command here
#define COMMAND 0x08

//  Set desired data parameter here



void setup() {

  //  Set desired data parameter here
  byte DATA = B11111100;

  Wire.begin();
  Serial.begin(9600);

  Serial.println("Initialized");

  Wire.beginTransmission(EPS_ADDRESS);
  Serial.println(EPS_ADDRESS);
  Wire.write(COMMAND);
  Serial.println(COMMAND);
  Wire.write(DATA);
  Serial.println(DATA);
  Wire.endTransmission();

  Serial.println("Command sent");

  

}

void loop() {}
