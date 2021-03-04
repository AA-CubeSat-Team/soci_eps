#include <Wire.h>

#define EPS_ADDRESS 0x51


//  Set the desired command here
#define COMMAND 0x08

//  Set desired data parameter here
//#define DATA 11001100


void setup() {

  byte DATA = B11001100;

  Wire.begin();
  Serial.begin(9600);

  Serial.println("Initialized");

  Wire.beginTransmission(EPS_ADDRESS);
  Wire.write(COMMAND);
  Wire.write(DATA);
  Wire.endTransmission();

  Serial.println("Command sent");

  

}

void loop() {}
