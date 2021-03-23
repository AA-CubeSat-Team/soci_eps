/*

  DHV EPS i2c command and telemetry simulator

  Michael Stickels





  Commands:                           Code:       Bytes Returned:   Status:
    Request (response)
      Power Module Status               0x01            2             written
      Battery Module Status             0x02            2             written
      FDIR                              0x03            1             written
      ID                                0x04            1             written
      Get Telemetry Group               0x0B            n             written
      Current Sensor Readout            0x0C            6             written, untested

    Recieve (no response)
      Set Watchdog Period               0x05            0             -
      Set PDMs Initial State            0x06            0             hard-coded
      Reset PDMs                        0x07            0             -
      Switch On/Off PDMs                0x08            0             written
      Set Housekeeping Period           0x09            0             -
      Set Safety Hazard Environment     0x0A            0             -
      Fixed Power Bus Reset             0xFE            0             -
      Manual Reset                      0xFF            0             -



*/




#include <Wire.h>

#define I2C_ADDRESS 0x51

// Pinouts
#define PDM_1 1
#define PDM_2 2
#define PDM_3 3
#define PDM_4 4
#define PDM_5 5
#define PDM_6 6
#define CUR_1 7
#define CUR_2 8
#define CUR_3 9
#define CUR_4 10
#define CUR_5 11
#define CUR_6 12
#define LED_pin 13

// Parameters
#define SENSE_R 10  // Current Sensor R_sense in mOhm

// i2c commands
#define POWER_MODULE_STATUS             0x01
#define BATTERY_MODULE_STATUS           0x02
#define FDIR                            0x03
#define ID                              0x04
#define SET_WATCHDOG_PERIOD             0x05
#define SET_PDMS_INITIAL_STATE          0x06
#define RESET_PDMS                      0x07
#define SWITCH_PDMS                     0x08
#define SET_HOUSEKEEPING_PERIOD         0x09
#define SET_SAFETY_HAZARD_ENVIRONMENT   0x0A
#define GET_TELEMETRY_GROUP             0x0B
#define FIXED_POWER_BUS_RESET           0xFE    //8-bit command may not work with Wire library?
#define MANUAL_RESET                    0xFF    //8-bit command may not work with Wire library?
#define CURRENT_SENSE                   0x0C

// Telemetry registries
#define BCRS                      0x00
#define SOLAR_PANEL_SENSORS       0x01
#define POWER_BUSES               0x02
#define SWITCHABLE_POWER_BUSES    0x03
#define BATTERY_MODULE            0x04
#define SYSTEM_DATA               0x05


byte command_rcv;               // command received from I2C bus
byte param_rcv;                 // paramater received from I2C bus
byte registry_rqst;
byte param_rqst;
int stat_LED;                   // status of LED: 1 = ON, 0 = OFF


// Setup
int pdm_pins[] = {PDM_1, PDM_2, PDM_3, PDM_4, PDM_5, PDM_6}; // array of PDM pins for switching method
int pdm_init_state[] = {0, 0, 0, 0, 0, 0}; // ??what is PDM initial state behavior of EPS?
int pdm_state[] = {0, 0, 0, 0, 0, 0};
int current_pins[] = {CUR_1, CUR_2, CUR_3, CUR_4, CUR_5, CUR_6};

void setup() {

  // join i2c as listener
  Wire.begin(I2C_ADDRESS);

  // event handler initializations
  Wire.onReceive(dataRcv);
  Wire.onRequest(dataRqst);

  // start serial port
  Serial.begin(9600);

  // initialize global variables
  command_rcv = 0;
  param_rcv = 255;
  registry_rqst = 0;
  param_rqst = 0;
  stat_LED = 0;

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



//******** Event Handler Methods ********\\


// Received data handler function
// This method assumes that one command byte and one parameter byte will be recieved and does
//    not yet handle exceptions.
void dataRcv(int numBytes) {

  // turn on lED indicator
  digitalWrite(LED_pin, HIGH);

  command_rcv = Wire.read();    // reads the first byte which is the command
  param_rcv = Wire.read();      // reads the second byte which is the data parameter

  // ?? How will Wire.read() respond if only 1 byte is recieved?

  printRcv(command_rcv, param_rcv);

  switch (command_rcv) {


    // Read Parameters
    case POWER_MODULE_STATUS:               registrySet(command_rcv, param_rcv);        break;
    case BATTERY_MODULE_STATUS:             registrySet(command_rcv, param_rcv);        break;
    case FDIR:                              registrySet(command_rcv, param_rcv);        break;
    case ID:                                registrySet(command_rcv, param_rcv);        break;
    case GET_TELEMETRY_GROUP:               registrySet(command_rcv, param_rcv);        break;

    // Write Commands
//    case SET_WATCHDOG_PERIOD:                                                           break;
//    case SET_PDMS_INITIAL_STATE:                                                        break;
//    case RESET_PDMS:                                                                    break;
    case SWITCH_PDMS:                       switchPDM(param_rcv);                       break;
//    case SET_HOUSEKEEPING_PERIOD:                                                       break;
//    case SET_SAFETY_HAZARD_ENVIRONMENT:                                                 break;
//    case FIXED_POWER_BUS_RESET:                                                         break;
//    case MANUAL_RESET:                                                                  break;
    default:                                Serial.println("command not recognized");   break;

  }

  // turn off LED indicator
  digitalWrite(LED_pin, LOW);

}

// requests data handler function
void dataRqst() {

  // turn on lED indicator
  digitalWrite(LED_pin, HIGH);

  switch (registry_rqst) {

    case POWER_MODULE_STATUS:               power_module_status();                      break;
    case BATTERY_MODULE_STATUS:             battery_module_status();                    break;
    case FDIR:                              fdir();                                     break;
    case ID:                                id();                                       break;
    case GET_TELEMETRY_GROUP:               telemetryGroup(param_rqst);                 break;
    case CURRENT_SENSE:                     currentSense();                             break;
    default:                                Serial.println("command not recognized");   break;
    
  }

  // turn off LED indicator
  digitalWrite(LED_pin, LOW);

}



//******** Command Methods ********\\


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
void power_module_status () {
 
  Serial.println("Enter Power Module Status...");
  Serial.println("Input must be 0-255");
  
  int bytes = 2;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}


//
void battery_module_status () {
 
  Serial.println("Enter Power Module Status...");
  Serial.println("Input must be 0-255");
  
  int bytes = 2;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}


//
void fdir() {
 
  Serial.println("Enter Power Module Status...");
  Serial.println("Input must be 0-255");
  
  int bytes = 2;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");

}


//
void id() {
 
  Serial.println("Enter Power Module Status...");
  Serial.println("Input must be 0-255");
  
  int bytes = 2;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");

}


//
void telemetryGroup(byte param) {

  Serial.println("Sending Telemetry...");

  switch (param) {

    case BCRS:                        bcr_telem();                                      break;
    case SOLAR_PANEL_SENSORS:         pv_telem();                                       break;
    case POWER_BUSES:                 pwrbus_telem();                                   break;
    case SWITCHABLE_POWER_BUSES:      switchpwrbus_telem();                             break;
    case BATTERY_MODULE:              battmodule_telem();                               break;
    case SYSTEM_DATA:                 systemdata_telem();                               break;
    default:                          Serial.println("telem. param. not recognized");   break;
    
  }

}


//
void currentSense() {

  Serial.println("Sending Current Readings...");
  
  int bytes = 6;
  float data = 0;

  for (int i = 0; i < bytes; i++) {

    data = analogRead(current_pins[i]) * (5.0 / 1023) / 100 / (SENSE_R);
    Wire.write(round(data));
    Serial.println("Byte sent... "); Serial.print(data);
    
  }

  Serial.println("complete");

}



//******** Helper Methods ********\\


// Sends 2 bytes of data back to master for response to request
void i2cwrite (byte data0, byte data1) {

  Wire.write(data0);
  Wire.write(data1);

}

// prints command and parameter received
void printRcv(byte command_rcv, byte param_rcv) {

  Serial.println(">>> Command Recieved...");
  Serial.print("Command: ");
  Serial.println(command_rcv, HEX);
  Serial.print("Parameter: ");
  Serial.println(param_rcv, BIN);

}


// prints rquest command
void printRqst (byte command) {

  Serial.println("Write Request Received...");

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


void registrySet(byte command, byte param) {

  registry_rqst = command;
  param_rqst = param;
  
}





//******** Telemetry Helper Methods ********\\


// 24 bytes total, each telemetry is 2 bytes
void bcr_telem() {

  Serial.println("Enter BCRs Telemetry...");
  Serial.println("Input must be 0-255");
  
  int bytes = 24;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}

// 10 bytes total, each telemetry is 2 bytes
void pv_telem() {

  Serial.println("Enter Solar Panel Sensors Telemetry...");
  Serial.println("Input must be 0-255");
  
  int bytes = 10;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}

// 24 bytes total, each telemetry is 2 bytes
void pwrbus_telem() {

  Serial.println("Enter Power Buses Telemetry...");
  Serial.println("Input must be 0-255");
  
  int bytes = 24;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}

// 24 bytes total, each telemery is 2 bytes
void switchpwrbus_telem() {

  Serial.println("Enter Switchable Power Buses Telemetry...");
  Serial.println("Input must be 0-255");
  
  int bytes = 24;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}

// 16 bytes total, each telemetry is 2 bytes
void battmodule_telem() {

  Serial.println("Enter Battery Module Telemetry...");
  Serial.println("Input must be 0-255");
  
  int bytes = 16;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) { /* wait for user input */ }
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");
    
  }

  Serial.println("complete");
  
}

// 12 bytes total, each telemetry is 2 bytes
void systemdata_telem() {

  Serial.println("Enter Sysetm Data Telemetry...");
  Serial.println("Input must be 0-255");
  
  int bytes = 12;
  byte data = 0;

  for (int i = 0; i < bytes; i++) {

    Serial.println("Byte "); Serial.print(i+1); Serial.print(": ");
    while (Serial.available() == 0) {
    data = Serial.parseInt();
    Wire.write(data);
    Serial.println("Byte sent...");

    }
  }

  Serial.println("complete");
  
}
