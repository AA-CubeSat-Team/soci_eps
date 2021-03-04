#include <Wire.h>

#define EPS_ADDRESS 0x51


//  Set the desired command here
#define COMMAND 0x01

//  Set desired registry parameter here
#define REGISTRY 0x01

#define EXPECTED_BYTES 24


void setup() {

  Wire.begin();
  Serial.begin(9600);

  Wire.beginTransmission(EPS_ADDRESS);
  Wire.write(COMMAND);
  Wire.write(REGISTRY);
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
