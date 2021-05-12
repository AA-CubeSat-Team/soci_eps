#include <Wire.h>

#define EPS_ADDRESS 0x51


//  Set the desired command here
#define COMMAND 0x0c

//  Set desired registry parameter here
#define REGISTRY 0x00

#define EXPECTED_BYTES 6


void setup() {

  Wire.begin();
  Serial.begin(9600);
  Serial.println("Initialized...");

  Wire.beginTransmission(EPS_ADDRESS);
  Serial.println(EPS_ADDRESS);
  Wire.write(COMMAND);
  Serial.println(COMMAND);
  Wire.write(REGISTRY);
  Serial.println(REGISTRY);
  Wire.endTransmission();

  delay(500);

  Wire.requestFrom(EPS_ADDRESS, EXPECTED_BYTES);

  while (Wire.available()) { 
    char c = Wire.read(); 
    Serial.print(c);
  }

  Serial.println("");

  

}

void loop() {}
