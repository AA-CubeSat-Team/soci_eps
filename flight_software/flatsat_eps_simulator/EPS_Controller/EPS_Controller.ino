/*

  Simple controller for flatsat EPS through serial monitor (PDM on/off and current sense)

  Michael Stickels

*/




// Pinouts
#define PDM_1 10
#define PDM_2 9
#define PDM_3 8
#define PDM_4 7
#define PDM_5 6
#define PDM_6 5
#define CUR_1 A7
#define CUR_2 A6
#define CUR_3 A3
#define CUR_4 A2
#define CUR_5 A1
#define CUR_6 A0
#define LED_pin 13

// Parameters
#define SENSE_R 10  // Current Sensor R_sense in mOhm


// Setup
int pdm_pins[] = {PDM_1, PDM_2, PDM_3, PDM_4, PDM_5, PDM_6}; // array of PDM pins for switching method
int pdm_init_state[] = {1, 1, 1, 1, 1, 1}; // ??what is PDM initial state behavior of EPS?
int pdm_state[] = {1, 1, 1, 1, 1, 1};
int current_pins[] = {CUR_1, CUR_2, CUR_3, CUR_4, CUR_5, CUR_6};

void setup() {

  // start serial port
  Serial.begin(9600);

  // set pinmodes for output pins
  pinMode(LED_pin, OUTPUT);
  for (int i = 0; i < 6; i++) {
    pinMode(pdm_pins[i], OUTPUT);
  }

  // set pin initial outputs
  digitalWrite(LED_pin, LOW);
  for (int i = 0; i < 6; i++) {
    if (pdm_state[i] == 1) {
      digitalWrite(pdm_pins[i], HIGH);
      pdm_state[i] = 1;
    }
    else {
      digitalWrite(pdm_pins[i], LOW);
      pdm_state[i] = 0;
    }
  }

  Serial.println("Initialized");
  printPDMState();


}



void loop() {}


}



//******** Helper Methods ********\\


// Assumes that parameter recieved is a byte with bits 0-5 corresponding to PDMs 1-6, ignors bits 6&7
// param: 000000XX
// 0 = off, 1 = on for each PDM
void switchPDM(byte param) {

  printPDMState();
  Serial.println("switching PDMs...");

  // iterates across param byte from most siginificant, stops at bit 5
  for (int i = 1, mask = 128; i <= 6; i++, mask = mask >> 1) {

    if (param & mask) {

      //turn PDM i on
      digitalWrite(pdm_pins[i - 1], HIGH);
      pdm_state[i - 1] = 1;

    } else {

      //turn PDM i off
      digitalWrite(pdm_pins[i - 1], LOW);
      pdm_state[i - 1] = 0;

    }

  }

  printPDMState();

}





//
void currentSense() {

  Serial.println("Sending Current Readings...");
  
  int bytes = 6;
  float data = 0;

  for (int i = 0; i < bytes; i++) {

    data = analogRead(current_pins[i]) * (5.0 / 1023) / 100 / (SENSE_R) * 1000;
    Wire.write((int) round(data));
    Serial.print("Byte sent... "); Serial.println(data);
    
  }

  Serial.println("complete");

}




// prints the state of each PDM to the serial console (0=off, 1=onn)
void printPDMState () {

  Serial.println("PDM States: ");
  Serial.print(pdm_state[0]);
  Serial.print(pdm_state[1]);
  Serial.print(pdm_state[2]);
  Serial.print(pdm_state[3]);
  Serial.print(pdm_state[4]);
  Serial.println(pdm_state[5]);

}
