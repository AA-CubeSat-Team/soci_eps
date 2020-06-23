
/* This is a source file for testing the I2C commands required for the flight computer to
 * communicate with the EPS board and the battery
 * 
 * As of 6/23/2020, everything in this file is being written without any access to hardware, and is completely untested.
 * It should not be assumed to be functional.
 * 
 * Jerrold Erickson, 6/23/2020
 */

#include "eps.h"

uint8_t g_slave_buff[I2C_DATA_LENGTH];
uint8_t g_master_buff[I2C_DATA_LENGTH];

uint32_t master_rtos_handle = 0; // fake variable for RTOS stuff
status_t status = 0; // fake variable for RTOS stuff
uint8_t kLPI2C_Write = 1; // not sure where this is defined, but I think it differentiates between read/write commands
uint8_t kLPI2C_Read = 0; // not sure where this is defined, but I think it differentiates between read/write commands

/* the I2C_read_write_lp function is designed to write to registers within the i2c slave device, so writing i2c data looks like this:
 * [device address -> register address -> data]
 * The EPS/battery do not use registers like this, and instead just follow a format like:
 * [device address -> command -> parameters]
 * So, this register address is just temporary. A more complete i2c library is needed.
 */
uint8_t I2C_EPS_REG_ADDR = 1;
size_t datasize = 1U;



int main(void)
{
  uint32_t i = 0;

  /* Init board hardware. */
  BOARD_ConfigMPU();
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();

  /*Clock setting for LPI2C*/
  CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT);
  CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);

  float eps_temperature;
  eps_temperature = i2c_eps_temperature();

  printf("EPS temperature is: %f\n", eps_temperature);

  while(1)
  {
  }  
}

/* This is an example of a function that the onboard computer could call if
 * it wanted to know the temperature of the EPS motherboard, for the purpose
 * of downlinking that data, or any other reason. Similar functions are required to
 * define the interface between the OBC and the EPS board/battery. For example, a
 * key parameter of our concept of operations is the voltage of the battery. To get
 * this voltage, the OBC should be able to call a function  called"i2c_bat_voltage"
 * or similar, that operates very similarly to the function written here.
 */
float i2c_eps_temperature()
{
  /* Set up i2c master to send data to slave */
  g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
  g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
  g_master_buff[2] = I2C_EPS_TELE_TEMPERATURE_MOTHERBOARD_0; // telemetry = motherboard temperature, first byte
  g_master_buff[3] = I2C_EPS_TELE_TEMPERATURE_MOTHERBOARD_1; // telemetry = motherboard temperature, second byte

  printf("Master will send data :");
  print_i2c_data(g_master_buff);

  // i2c write
  I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Write, I2C_EPS_REG_ADDR, g_master_buff[0], datasize);
  I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Write, I2C_EPS_REG_ADDR, g_master_buff[1], datasize);
  I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Write, I2C_EPS_REG_ADDR, g_master_buff[2], datasize);
  
  delay();

  //i2c read
  I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Read, I2C_EPS_REG_ADDR, g_slave_buff, datasize);

  delay();

  printf("Master received data from slave: ");
  print_i2c_data(g_slave_buff);

  uint32_t adc_count = 0;
  /* get the ADC count returned using bitwise operations
   * adc_count will be a 32-bit int of the form:
   * {0x00     0x00     g_slave_buff[1]     g_slave_buff[0]}
   * I don't know if this calculation is correct
   */
  adc_count = (g_slave_buff[1] << 8) | g_slave_buff[0];

  float temperature = (0.372434 * adc_count) - 273.15; // from eps datasheet
  return temperature;
}

void delay()
{
  for (int i = 0; i < 5000; i++)
  {
    // delay 6ms ish
  }
}

void print_i2c_data(uint8_t buffer)
{
  for (i = 0; i < I2C_DATA_LENGTH; i++)
  {
    if (i % 8 == 0)
    {
      printf("\r\n");
    }
    printf("0x%2x  ", buffer[i]);
  }
  printf("\r\n\r\n");
}