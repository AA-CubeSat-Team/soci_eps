// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

// for read_________________________________________________
#include <Wire.h>
#include <stdint.h>

#define I2C_EPS_ADDR 0x51

// Pinouts
#define PDM_1 1
#define PDM_2 2
#define PDM_3 3
#define PDM_4 4
#define PDM_5 5
#define PDM_6 6
#define LED_pin 13

#define I2C_EPS_CMD_POWER_MODULE_STATUS 0x01
#define I2C_EPS_CMD_BATTERY_MODULE_STATUS 0x02
#define I2C_EPS_CMD_FDIR 0x03
#define I2C_EPS_CMD_ID 0x04
#define I2C_EPS_CMD_SET_WATCHDOG_PERIOD 0x05
#define I2C_EPS_CMD_SET_PDMS_INTIAL_STATE 0x06
#define I2C_EPS_CMD_RESET_PDMS 0x07
// stopped here
#define I2C_EPS_CMD_SWITCH_ON_OFF_PDMS 0x08
#define I2C_EPS_CMD_SET_HOUSEKEEPING_PERIOD 0x09
#define I2C_EPS_CMD_SET_SAFETY_HAZARD_ENVIRONMENT 0x0A
#define I2C_EPS_CMD_GET_TELEMETRY_GROUP 0x0B
#define I2C_EPS_CMD_FIXED_POWER_BUS_RESET 0xFE
#define I2C_EPS_CMD_MANUAL_RESET 0xFF

// telemetry pg 14
// check if thinking about correctly 
// for param[1] (where in I2C is parameter?) 
// also how do you read the byte and bit 
#define I2C_EPS_TELE_BCRS 0x00
#define I2C_EPS_TELE_SOLAR_PANEL_SENSORS 0x01
#define I2C_EPS_TELE_POWER_BUSES 0x02
#define I2C_EPS_TELE_SWITCHABLE_POWER_BUSES 0x03
#define I2C_EPS_TELE_BATTERY_MODULE 0x04
#define I2C_EPS_TELE_SYSTEM_DATA 0x05

uint16_t g_master_buff0, g_master_buff1, g_master_buff2, g_master_buff3;
uint32_t g_slave_buff;
uint32_t adc_count;

// for write_________________________________________________
void setup()
{
  Serial.begin(9600);
}

void loop()
{
  i2c_eps_powerModuleStatus();
  i2c_eps_batteryModuleStatus();
  i2c_eps_FDIRflag();
  i2c_eps_idRegister();
  i2c_eps_watchdogPeriod(0x01);           // set to period -> 1 minute        //byte period);
  i2c_eps_setPdmsInitialState(0x01);      // set to pdm1                      //byte pdm_state);    
  i2c_eps_resetPdm();
  i2c_eps_switchOnOffPdms(0x02);          // set to pdm2                      //byte newPdmState);
  i2c_eps_setHousekeepingPeriod(0x02);    // set to period -> 2 minute        //byte period);
  i2c_eps_setSafetyHazardEnvironment();
  i2c_eps_fixedPowerBusReset(0x00);       // 3V3 Reset bit 1                  //byte busReset);
  i2c_eps_manualReset();
}

// EDITED 1/23___________________________________________________________________________
void i2c_write (byte addr, byte command, byte data)
{
  Wire.begin();
  Wire.beginTransmission(addr);
  Wire.write(command);
  Wire.write(data);
  Wire.endTransmission();
}

void i2c_read (byte addr, byte returned_value)
{
  int bytes = 0;
  if ((addr == 0x01) | (addr == 0x02)) {
    bytes = 2;
  } else {
    bytes = 1;
  }
  Wire.requestFrom(addr, bytes);    // request potentiometer position from slave 0x08
  if(Wire.available()) {       // read response from slave 0x08
    returned_value = Wire.read();
  }
}

uint32_t i2c_read_write_helper(uint16_t gmb_0, uint16_t gmb_1, uint16_t gmb_2, uint32_t gsb) {
  printf("Master will send data :");
  printf("%d-%d-%d", gmb_0, gmb_1, gmb_2);
    
  // i2c write
  i2c_write(gmb_0, gmb_1, gmb_2);
 
  //i2c read
  uint32_t adc_count = 0;
  if ((gmb_0 == 0x01) || (gmb_0 == 0x02) || (gmb_0 == 0x03) || (gmb_0 == 0x04)) {
      i2c_read(gmb_0, gsb);
      printf("Master received data from slave: ");
      printf("%d", gsb);

      /* get the ADC count returned using bitwise operations
       * adc_count will be a 32-bit int of the form:
       * {0x00     0x00     g_slave_buff[1]     g_slave_buff[0]}
       * I don't know if this calculation is correct
       */
      //adc_count = (g_slave_buff[1] << 8) | g_slave_buff[0]);
      adc_count = gsb;
  }
  return adc_count;
}

// METHOD STARTS HERE ________________________________________________________________________
void i2c_eps_powerModuleStatus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_POWER_MODULE_STATUS; // i2c command = get EPS telemetry

    // send and read
    // returns 2 bytes <- remember to adjust so it can return multiple different selection of bytes
    // delay is unknown?
    // thought: regardless if all bytes are moved, it should still return the same adc_count 
    //          which can still be checked, so it would be best to adjust so all bytes are moved 
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);

    //char string[1000];
    if (adc_count & (1<<0)) 
    {
        printf("3V3 output error\n");
    } 
    if (adc_count & (1<<1)) 
    {
        printf("5V output error\n");
    } 
    if (adc_count & (1<<2)) 
    {
        printf("12V output error\n");
    } 
    if (adc_count & (1<<8)) 
    {
        printf("PDM1 error\n");
    } 
    if (adc_count & (1<<9)) 
    {
        printf("PDM2 error\n");
    } 
    if (adc_count & (1<<10)) 
    {
        printf("PDM3 error\n");
    } 
    if (adc_count & (1<<11)) 
    {
        printf("PDM4 error\n");
    } 
    if (adc_count & (1<<12)) 
    {
        printf("PDM5 error\n");
    } 
    if (adc_count & (1<<13)) 
    {
        printf("PDM6 error\n");
    } 
    if (adc_count && 0x0000) {
        printf("No error\n");
    }

    //return string;
}

void i2c_eps_batteryModuleStatus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_BATTERY_MODULE_STATUS; // i2c command = get EPS telemetry

    // send and read
    // returns 2 bytes <- remember to adjust so it can return multiple different selection of bytes
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);
  
    //char string[1000];
    if (adc_count & (1<<0)) 
    {
        printf("CC, Charge Control Flag. Set if battery charge is disabled.\n");
    } 
    if (adc_count & (1<<1)) 
    {
        printf("DC, Discharge Control Flag. Set if battery discharge is disabled.\n");
    } 
    if (adc_count & (1<<2)) 
    {
        printf(" CHGTF, Charge-Termination Flag. Set if battery is fully charged.\n");
    } 
    if (adc_count & (1<<4)) 
    {
        printf( "SEF, Standby-Empty Flag. Set if capacity is below 10%%, unset if above 15 %%.\n");
    } 
    if (adc_count & (1<<8)) 
    {
        printf("Set if heater is active.\n");
    } 
    if (adc_count & (1<<12)) 
    {
        printf("Set if battery balancing is happening from top cell to bottom cell.\n");
    } 
    if (adc_count & (1<<13)) 
    {
        printf("Set if battery balancing is happening from bottom cell to top cell.\n");
    } 
    if (adc_count && 0x0000) {
        printf("No error\n");
    }
}

void i2c_eps_FDIRflag()
{
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_FDIR; // i2c command = get EPS telemetry

    // send and read
    // returns 1 bytes <- remember to adjust so it can return multiple different selection of bytes
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);

    //char string[1000];
    if (adc_count & (1<<0)) 
    {
        printf("0 Set if last was command unknown\n");
    } 
    if (adc_count & (1<<1)) 
    {
        printf("Set if last command parameter was invalid.\n");
    } 
    if (adc_count & (1<<2)) 
    {
        printf("Set if watchdog was triggered.\n");
    } 
    if (adc_count & (1<<3)) 
    {
        printf("Set if BOR was triggered.\n");
    } 
    if (adc_count & (1<<5)) 
    {
        printf("Set if battery manager is unavailable.\n");
    } 
    if (adc_count & (1<<6)) 
    {
        printf("Set if VBAT1_ADC is out of range.\n");
    } 
    if (adc_count & (1<<7)) 
    {
        printf("Set if VBAT2_ADC is out of range.\n");
    } 
    if (adc_count & (1<<8)) 
    {
        printf("Set if IBAT_BM is out of range.\n");
    } 
    if (adc_count & (1<<9)) 
    {
        printf("Set if TEMP_BM is out of range.\n");
    } 
    if (adc_count & (1<<10)) 
    {
        printf("Set if TEMP_MB is out of range.\n");
    } 
    if (adc_count & (1<<11)) 
    {
        printf("Set if TEMP_DB1 is out of range.\n");
    } 
    if (adc_count && 0x0000) {
        printf("No error\n");
    }

    //return string;
}

void i2c_eps_idRegister()
{
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_ID; // i2c command = get EPS telemetry

    // send and read
    // returns 1 bytes <- remember to adjust so it can return multiple different selection of bytes
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);

    bool verified_com;
    verified_com = adc_count && 0x67D6;
    //char string[100];
    if (verified_com) {
        printf("Communication is verified with module \n");
    } else {
        printf("Communication is NOT verified with module \n");
    }
    //return string;
}

// for watchdog, userinput period
// only accepts 1, 2, 4 for each period in minutes
void i2c_eps_watchdogPeriod (byte period)
{
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_SET_WATCHDOG_PERIOD; // i2c command = get EPS telemetry
    g_master_buff2 = 0x0000 | period;
    //g_master_buff[3] = period;
    
    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);

    delay(5000);

    // dont need to read bc no data returned
}

// Set PDM initial state has input for initial state
// pdm_state = set bit: 00******
// bit 0 effects PDM1 state, bit 1 effects PDM2 state, and so on...
void i2c_eps_setPdmsInitialState(byte pdm_state)
{   
    uint32_t intial_pdms = 0x00 | pdm_state; // bit 6 and 7 are reserved 
                                             // should become 0x00(6 bit pdm)

    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_SET_PDMS_INTIAL_STATE; // i2c command = get EPS telemetry
    g_master_buff2 = 0x00;
    g_master_buff3 = intial_pdms;
    
    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);

    delay(5000);

    // dont need to read bc no data returned
}

void i2c_eps_resetPdm()
{   
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_RESET_PDMS; // i2c command = get EPS telemetry
    g_master_buff2 = 0x00;
    g_master_buff3 = 0xFF;
    
    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);


    delay(5000);
    
    // dont need to read bc no data returned
}

// added 12/6/20
// will skip on doing telemtry for now until answers on how it outputs is provided

// for this newPdmState will turn on specific pdm 1-6 which is assigned from bit 0-5 in that order
void i2c_eps_switchOnOffPdms(byte newPdmState)
{   
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_SWITCH_ON_OFF_PDMS; // i2c command = get EPS telemetry
    g_master_buff2 = 0x00;
    g_master_buff3 = newPdmState;
    
    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);


    delay(5000);
    
    // dont need to read bc no data returned
}

// period can be 1 min 2 min or 4 min
void i2c_eps_setHousekeepingPeriod(byte period)
{   
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_SET_HOUSEKEEPING_PERIOD; // i2c command = get EPS telemetry
    g_master_buff2 = 0x00;
    g_master_buff3 = period;
    
    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);


    delay(5000);
    
    // dont need to read bc no data returned
}

// QUESTION: how is the i2c write structured
// is it 7 bit address -> command -> param [1] -> param [0] (each a byte)
// IF SO -> then what for PDM initial state why is it Byte 0 and not Param[0] (typo? as nothing is returned)

void i2c_eps_setSafetyHazardEnvironment()
{   
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_SET_SAFETY_HAZARD_ENVIRONMENT; // i2c command = get EPS telemetry
    g_master_buff2 = 0x00; 
    g_master_buff3 = 0xFF;

    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);


    delay(5000);
    
    // dont need to read bc no data returned
}

// some ideas about telemetry
// for telemetry, need clarification 

// for bus Reset
// ALSO ASSUMPTION: when asking for byte i am assuming its talking about only one byte of parameter
// otherwise when it uses the term parameter, I am assuming there are 2 bytes of parameter
// bit 1 and 0 = 1 then 3v3
// bit 3 and 2 = 1 then 5v
// bit 5 and 4 = 1 then 12v
// bit 7 and 6 = 1 then Vbat
void i2c_eps_fixedPowerBusReset(byte busReset)
{   
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_FIXED_POWER_BUS_RESET; // i2c command = get EPS telemetry
    g_master_buff2 = 0x00 | busReset; 


    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);


    delay(5000);
    
    // dont need to read bc no data returned
}

void i2c_eps_manualReset()
{   
    /* Set up i2c master to send data to slave */
    g_master_buff0 = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff1 = I2C_EPS_CMD_MANUAL_RESET; // i2c command = get EPS telemetry
    g_master_buff2 = 0xFF; 
    
    // i2c write
    i2c_read_write_helper(g_master_buff0, g_master_buff1, g_master_buff2, g_slave_buff);


    delay(5000);
    
    // dont need to read bc no data returned
}
