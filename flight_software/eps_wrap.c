/* This is a source file for testing the I2C commands required for the flight computer to
 * communicate with the EPS board and the battery
 *
 * As of 6/23/2020, everything in this file is being written without any access to hardware, and is completely untested.
 * It should not be assumed to be functional.
 *
 * Jerrold Erickson, 6/23/2020
 * Ryan Park, 2/27/2021
 * Pin #75 GPIO_AD_B1_14	I2C1_SCL
 * Pin #74 GPIO_AD_B1_15	I2C1_SDA
 */
#include <stdbool.h>
#include "eps_wrap.h"
#include "fsl_lpi2c.h" //need these here? already in peripherals
#include "fsl_lpi2c_freertos.h"
#include "peripherals.h"

#include "fsl_debug_console.h"

#define I2C_EPS_CMD_POWER_MODULE_STATUS 0x01
#define I2C_EPS_CMD_BATTERY_MODULE_STATUS 0x02
#define I2C_EPS_CMD_FDIR 0x03
#define I2C_EPS_CMD_ID 0x04
#define I2C_EPS_CMD_SET_WATCHDOG_PERIOD 0x05
#define I2C_EPS_CMD_SET_PDMS_INTIAL_STATE 0x06
#define I2C_EPS_CMD_RESET_PDMS 0x07
#define I2C_EPS_CMD_SWITCH_ON_OFF_PDMS 0x08
#define I2C_EPS_CMD_SET_HOUSEKEEPING_PERIOD 0x09
#define I2C_EPS_CMD_SET_SAFETY_HAZARD_ENVIRONMENT 0x0A

 // put telemetry
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

#define I2C_EPS_BYTE_0 0x00
#define I2C_EPS_BYTE_2 0x02
#define I2C_EPS_BYTE_4 0x04
#define I2C_EPS_BYTE_6 0x06
#define I2C_EPS_BYTE_8 0x08
#define I2C_EPS_BYTE_10 0x10
#define I2C_EPS_BYTE_12 0x12
#define I2C_EPS_BYTE_14 0x14
#define I2C_EPS_BYTE_16 0x16
#define I2C_EPS_BYTE_18 0x18
#define I2C_EPS_BYTE_20 0x20
#define I2C_EPS_BYTE_22 0x22

#define VERIFIED_COM_MASK 0x67D6
#define NO_RETURN 9999


#define EPS_SLAVE_ADDR 0 //change
#define I2C_DATA_LENGTH 4 //?

uint8_t buffer[I2C_DATA_LENGTH];

static uint32_t adc_count;

bool eps_healthcheck() {
	PRINTF("checking the health of eps!\r\n");
	i2c_eps_batteryModuleStatus();
	i2c_eps_powerModuleStatus();
	return true;
}


//uint8_t kLPI2C_Write = 1; // not sure where this is defined, but I think it differentiates between read/write commands
//uint8_t kLPI2C_Read = 0; // not sure where this is defined, but I think it differentiates between read/write commands
//
///* the I2C_read_write_lp function is designed to write to registers within the i2c slave device, so writing i2c data looks like this:
// * [device address -> register address -> data]
// * The EPS/battery do not use registers like this, and instead just follow a format like:
// * [device address -> command -> parameters]
// * So, this register address is just temporary. A more complete i2c library is needed.
// */

//uint8_t I2C_EPS_REG_ADDR = 1;
//size_t datasize = 1U;
//
//
//
//int main(void)
//{
//    uint32_t i = 0;
//
//    /* Init board hardware. */
//    BOARD_ConfigMPU();
//    BOARD_InitPins();
//    BOARD_BootClockRUN();
//    BOARD_InitDebugConsole();
//
//    /*Clock setting for LPI2C*/
//    CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT);
//    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);
//
//
//    //EPS telemetry
//    char eps_powerModuleStatus[1000];
//    char eps_batteryModuleStatus[1000];
//    char eps_FDIRflag[1000];
//    char eps_idRegister[1000];
//
//    eps_powerModuleStatus = i2c_eps_powerModuleStatus();
//    eps_batteryModuleStatus = i2c_eps_batteryModuleStatus();
//    eps_FDIRflag = i2c_eps_FDIRflag();
//    eps_idRegister = i2c_eps_idRegister();
//    //enter telemetry here;
//
//    PRINTF("%s \n", eps_powerModuleStatus);
//    PRINTF("%s \n", eps_batteryModuleStatus);
//    PRINTF("%s \n", eps_FDIRflag);
//    PRINTF("%s \n", eps_idRegister);
//
//    while (1)
//    {
//    }
//}
//
static void i2c_read_write_helper(uint8_t* i2c_send_buffer, size_t data_size, uint32_t * i2c_recv_buffer, time_t d) {

	// send gmb0, gmb1, gmb2, gmb3 in order
	size_t n;
	// TODO: send the delay as well
	LPI2C1_send_receive(EPS_SLAVE_ADDR, i2c_send_buffer, data_size, &adc_count, &n); //n necessary?

	if (d != NO_RETURN) {
		//delay(d);
	}

	adc_count = 0;
	//LPI2C1_receive(...)
//
//
//	//OBC: getStatus() in idle_task -> helper -> FreeRTOS_send gmb
//	//EPS: gets a message via I2C -> I need to send back the status -> I2C send(status)
//	//OBC: received status -> read g_slave_buff -> use that to do the next operation
//
//
//	delay(d);
//
//	//i2c read
//	I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Read, I2C_EPS_REG_ADDR, g_slave_buff, datasize);
//
//	delay(d);
//
//	PRINTF("Master received data from slave: ");
//	print_i2c_data(g_slave_buff);
//
//	uint32_t adc_count = 0;
//	/* get the ADC count returned using bitwise operations
//	 * adc_count will be a 32-bit int of the form:
//	 * {0x00     0x00     g_slave_buff[1]     g_slave_buff[0]}
//	 * I don't know if this calculation is correct
//	 */

//	adc_count = (g_slave_buff[1] << 8) | g_slave_buff[0]);

	return adc_count;
}

// added 11/24/20
//__________________________________________________________________________________________________________________

double i2c_eps_getBatteryLevel()
{
	PRINTF("get the battery value!\r\n");
	return 0.0;
}
void i2c_eps_powerModuleStatus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_POWER_MODULE_STATUS;

	// send and read
	// returns 2 bytes <- remember to adjust so it can return multiple different selection of bytes
	// delay is unknown?
	// thought: regardless if all bytes are moved, it should still return the same adc_count
	//          which can still be checked, so it would be best to adjust so all bytes are moved
	i2c_read_write_helper(buffer, 4, &adc_count, 5000);


	if (adc_count & (1 << 0))
	{
		PRINTF("3V3 output error\n");
	}
	if (adc_count & (1 << 1))
	{
		PRINTF("5V output error\n");
	}
	if (adc_count & (1 << 2))
	{
		PRINTF("12V output error\n");
	}
	if (adc_count & (1 << 8))
	{
		PRINTF("PDM1 error\n");
	}
	if (adc_count & (1 << 9))
	{
		PRINTF("PDM2 error\n");
	}
	if (adc_count & (1 << 10))
	{
		PRINTF("PDM3 error\n");
	}
	if (adc_count & (1 << 11))
	{
		PRINTF("PDM4 error\n");
	}
	if (adc_count & (1 << 12))
	{
		PRINTF("PDM5 error\n");
	}
	if (adc_count & (1 << 13))
	{
		PRINTF("PDM6 error\n");
	}
	return;
}

void i2c_eps_batteryModuleStatus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_BATTERY_MODULE_STATUS;

	// send and read
	//  returns 2 bytes <- remember to adjust so it can return multiple different selection of bytes
	// delay is unknown?
	// thought: regardless if all bytes are moved, it should still return the same adc_count
	//          which can still be checked, so it would be best to adjust so all bytes are moved
	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	if (adc_count & (1 << 0))
	{
		PRINTF("CC, Charge Control Flag. Set if battery charge is disabled.\n");
	}
	if (adc_count & (1 << 1))
	{
		PRINTF("DC, Discharge Control Flag. Set if battery discharge is disabled.\n");
	}
	if (adc_count & (1 << 2))
	{
		PRINTF(" CHGTF, Charge-Termination Flag. Set if battery is fully charged.\n");
	}
	if (adc_count & (1 << 4))
	{
		PRINTF("SEF, Standby-Empty Flag. Set if capacity is below 10%%, unset if above 15 %%.\n");
	}
	if (adc_count & (1 << 8))
	{
		PRINTF("Set if heater is active.\n");
	}
	if (adc_count & (1 << 12))
	{
		PRINTF("Set if battery balancing is happening from top cell to bottom cell.\n");
	}
	if (adc_count & (1 << 13))
	{
		PRINTF("Set if battery balancing is happening from bottom cell to top cell.\n");
	}
	return;
}

void i2c_eps_FDIRflag()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_FDIR;

	// send and read
	// returns 1 bytes <- remember to adjust so it can return multiple different selection of bytes
	// delay is unknown?
	// thought: regardless if all bytes are moved, it should still return the same adc_count
	//          which can still be checked, so it would be best to adjust so all bytes are moved
	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	if (adc_count & (1 << 0))
	{
		PRINTF("0 Set if last was command unknown\n");
	}
	if (adc_count & (1 << 1))
	{
		PRINTF("Set if last command parameter was invalid.\n");
	}
	if (adc_count & (1 << 2))
	{
		PRINTF("Set if watchdog was triggered.\n");
	}
	if (adc_count & (1 << 3))
	{
		PRINTF("Set if BOR was triggered.\n");
	}
	if (adc_count & (1 << 5))
	{
		PRINTF("Set if battery manager is unavailable.\n");
	}
	if (adc_count & (1 << 6))
	{
		PRINTF("Set if VBAT1_ADC is out of range.\n");
	}
	if (adc_count & (1 << 7))
	{
		PRINTF("Set if VBAT2_ADC is out of range.\n");
	}
	if (adc_count & (1 << 8))
	{
		PRINTF("Set if IBAT_BM is out of range.\n");
	}
	if (adc_count & (1 << 9))
	{
		PRINTF("Set if TEMP_BM is out of range.\n");
	}
	if (adc_count & (1 << 10))
	{
		PRINTF("Set if TEMP_MB is out of range.\n");
	}
	if (adc_count & (1 << 11))
	{
		PRINTF("Set if TEMP_DB1 is out of range.\n");
	}
	return;
}

void i2c_eps_idRegister() ///make bool?
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_ID;

	// send and read
	// returns 1 bytes <- remember to adjust so it can return multiple different selection of bytes
	// delay is unknown?
	// thought: regardless if all bytes are moved, it should still return the same adc_count
	//          which can still be checked, so it would be best to adjust so all bytes are moved
	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	bool verified_com;
	verified_com = adc_count && VERIFIED_COM_MASK; // TODO: gorkem - are we checking only the first bit here? then the mask is unnecessary
	if (verified_com) {
		PRINTF("Communication is verified with module \n");
	}
	else {
		PRINTF("Communication is NOT verified with module \n");
	}
	return;
}
//
//// for watchdog, userinput period
//// only accepts 1, 2, 4 for each period in minutes
void i2c_eps_watchdogPeriod(uint8_t period)
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_ID;
	buffer[2] = 0x0000 | period;
	//buffer[3] = period;
	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused
	return;
}


// Set PDM initial state has input for initial state
// pdm_state = set bit: 00******
// bit 0 effects PDM1 state, bit 1 effects PDM2 state, and so on...
void i2c_eps_setPdmsInitialState(uint8_t pdm_state)
{
	uint32_t intial_pdms = 0x00 | pdm_state; // bit 6 and 7 are reserved
											// should become 0x00(6 bit pdm)
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_SET_PDMS_INTIAL_STATE;
	buffer[2] = 0x00;
	buffer[3] = intial_pdms;
	//buffer[3] = period;
	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused
	return;
}

void i2c_eps_resetPdm()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_RESET_PDMS;
	buffer[2] = 0x00;
	buffer[3] = 0xFF; //all ON?

	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused
	return;
}

// added 12/6/20
// will skip on doing telemtry for now until answers on how it outputs is provided
//
// for this newPdmState will turn on specific pdm 1-6 which is assigned from bit 0-5 in that order
void i2c_eps_switchOnOffPdms(uint8_t newPdmState)
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_SWITCH_ON_OFF_PDMS;
	buffer[2] = 0x00;
	buffer[3] = newPdmState;

	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused

	// TODO: do we want an acknowledgement?
	return;
}

// period can be 1 min 2 min or 4 min
void i2c_eps_setHousekeepingPeriod(uint8_t period)
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_SET_HOUSEKEEPING_PERIOD;
	buffer[2] = 0x00;
	buffer[3] = period;

	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused

	return;
}

// QUESTION: how is the i2c write structured
// is it 7 bit address -> command -> param [1] -> param [0] (each a byte)
// IF SO -> then what for PDM initial state why is it Byte 0 and not Param[0] (typo? as nothing is returned)

void i2c_eps_setSafetyHazardEnvironment()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_SET_SAFETY_HAZARD_ENVIRONMENT;
	buffer[2] = 0x00;
	buffer[3] = 0xFF;

	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused

	return;
}

// TELEMETRY____________________________________________________________________________________________________________
// Would like a check from CDH
// added 2/20/21
// for telemetry the buffer needed for the complete family is different
// may want to add a user input line instead of families input

// families selects which family
// branch selects which branch in family


//CREATE TELEMETRY INDIVIDUAL
// BCR FAMILY________________________________________________________________________________
int i2c_eps_telemetry_bcr8w_1_inputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);
	
	uint16_t tm = adc_count & 0x11;
	return tm * .008;

}

int i2c_eps_telemetry_bcr8w_1_inputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count >> 16) & 0x11; 
	return tm * 2;
}

int i2c_eps_telemetry_bcr8w_1_outputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 1) & 0x11; // I am unsure but I think + 1 should move to the next 4 bytes of array in read data
	return tm * .008; 

}

int i2c_eps_telemetry_bcr8w_1_outputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 1) >> 16) & 0x11;
	return tm * 2;
}

int i2c_eps_telemetry_bcr8w_2_inputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 2) & 0x11;
	return tm * .008;
}

int i2c_eps_telemetry_bcr8w_2_inputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 2) >> 16) & 0x11;
	return tm * 2;
}

int i2c_eps_telemetry_bcr8w_2_outputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 3) & 0x11;
	return tm * .008;
}

int i2c_eps_telemetry_bcr8w_2_outputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 3) >> 16) & 0x11;
	return tm * 2;
}

int i2c_eps_telemetry_bcr3w_inputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 4) & 0x11;
	return tm * .008;
}

int i2c_eps_telemetry_bcr3w_inputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 4) >> 16) & 0x11;
	return tm * 2;
}

int i2c_eps_telemetry_bcr3w_outputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 5) & 0x11;
	return tm * .008;
}

int i2c_eps_telemetry_bcr3w_outputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 5) >> 16) & 0x11;
	return tm * 2;
}

//END___________________________________________________________________________________

//SOLAR PANEL TEMPERATURE_______________________________________________________________

// twos compliment times 0.5 which is needed to convert the temperatures below
int twosCompAndMultHalf(uint16_t x)
{
	// turn into 2s compliment
	for (int i = 15; i >= 0; i--)
	{
		if (x[i] == '1')
		{
			x[i] = '0';
		}
		else
		{
			x[i] = '1';
		}
	}
	x = x + 1;

	// convert by mult by .5/bit
	// check with members if multiple each 1 bit with .5 and add together or to multiply the whole thing by .5
	y = x * 0.5;

	return y;
}

int i2c_eps_telemetry_M_SP_tempXplus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SOLAR_PANEL_SENSORS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = adc_count & 0x11;
	int celcius = twosCompMultHalf(tm);
	return celcius;

}

int i2c_eps_telemetry_M_SP_tempXminus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SOLAR_PANEL_SENSORS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count >> 16) & 0x11;
	int celcius = twosCompMultHalf(tm);
	return celcius;
}

int i2c_eps_telemetry_M_SP_tempYplus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SOLAR_PANEL_SENSORS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 1) & 0x11;
	int celcius = twosCompMultHalf(tm);
	return celcius;
}

int i2c_eps_telemetry_M_SP_tempYminus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SOLAR_PANEL_SENSORS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 1) >> 16) & 0x11;
	int celcius = twosCompMultHalf(tm);
	return celcius;
}

int i2c_eps_telemetry_M_SP_tempZplus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SOLAR_PANEL_SENSORS;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 2) & 0x11;
	int celcius = twosCompMultHalf(tm);
	return celcius;
}

//END

// POWER BUSES______________________________________________________________________

int i2c_eps_telemetry_bcr_outputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = adc_count & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_bcr_outputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count >> 16) & 0x11;
	return tm * 0.0020676;
}

int i2c_eps_telemetry_pcm_inputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 1) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_pcm_inputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 1) >> 16) & 0x11;
	return tm * 0.0020676;
}

int i2c_eps_telemetry_3v3_powerBusVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 2) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_3v3_powerBusCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 2) >> 16) & 0x11;
	return tm * 0.0020676;
}

int i2c_eps_telemetry_5v_powerBusVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 3) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_5v_powerBusCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 3) >> 16) & 0x11;
	return tm * 0.0020676;
}

int i2c_eps_telemetry_Vbat_powerBusVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 4) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_Vbat_powerBusCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 4) >> 16) & 0x11;
	return tm * 0.0020676;
}

int i2c_eps_telemetry_12v_powerBusVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 5) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_12v_powerBusCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 5) >> 16) & 0x11;
	return tm * 0.0020676;
}



//END___________________________________________________________________________________________

// SWITCHABLE POWER BUSES_______________________________________________________________________

int i2c_eps_telemetry_sw1_v()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_sw1_c()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count >> 16) & 0x11;
	return tm * 0.0008336 - 0.010;
}

int i2c_eps_telemetry_sw2_v()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 1) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_sw2_c()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 1) >> 16) & 0x11;
	return tm * 0.0008336 - 0.010;
}

int i2c_eps_telemetry_sw3_v()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 2) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_sw3_c()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 2) >> 16) & 0x11;
	return tm * 0.0008336 - 0.010;
}

int i2c_eps_telemetry_sw4_v()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 3) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_sw4_c()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 3) >> 16) & 0x11;
	return tm * 0.0008336 - 0.010;
}

int i2c_eps_telemetry_sw5_v()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 4) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_sw5_c()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 4) >> 16) & 0x11;
	return tm * 0.0008336 - 0.010;
}

int i2c_eps_telemetry_sw6_v()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 5) & 0x11;
	return tm * 0.0030945;
}

int i2c_eps_telemetry_sw6_c()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SWITCHABLE_POWER_BUSES;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 5) >> 16) & 0x11;
	return tm * 0.0008336 - 0.010;
}

//END_______________________________________________________________________________________

// BATTERY MODULE TELEMETRY_________________________________________________________________

// twos compliment times 0.5 which is needed to convert the temperatures below
int twosComp (uint16_t x)
{
	// turn into 2s compliment
	for (int i = 15; i >= 0; i--)
	{
		if (x[i] == '1')
		{
			x[i] = '0';
		}
		else
		{
			x[i] = '1';
		}
	}
	x = x + 1;

	return x;
}


int i2c_eps_telemetry_vbat_1()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count) & 0x11;
	return tm * 4.883;

}

int i2c_eps_telemetry_vbat_2()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count >> 16) & 0x11;
	return tm * 4.883;

}	

int i2c_eps_telemetry_ibat()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 1) & 0x11;
	int value = twosComp(tm);
	return value * 0.26;
}

int i2c_eps_telemetry_bat_temp()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 1) >> 16) & 0x11;
	int value = twosComp(tm);
	return value * 0.10;
}

int i2c_eps_telemetry_pcb_temp()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 2) & 0x11;
	int value = twosComp(tm);
	return value * 0.125;
}

int i2c_eps_telemetry_availableCap()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 2) >> 16) & 0x11;
	return tm * 1.6;

}

int i2c_eps_telemetry_remainingCap()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 3) & 0x11;
	return tm;
}

int i2c_eps_telemetry_accumulatedBatteryCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 3) >> 16) & 0x11;
	return tm * 1.042;
}

//END__________________________________________________________________________________

// SYSTEM DATA__________________________________________________________________________

int i2c_eps_telemetry_watchdogPeriod()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SYSTEM_DATA;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count) & 0x11;
	return data;
}

int i2c_eps_telemetry_pdmsInitialState()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SYSTEM_DATA;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count) >> 16) & 0x11;
	return data;
}

int i2c_eps_telemetry_pdmsState()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SYSTEM_DATA;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 1) & 0x11;
	return data;
}

int i2c_eps_telemetry_housekeepingPeriod()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SYSTEM_DATA;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = ((adc_count + 1) >> 16) & 0x11;
	return data;
}

bool i2c_eps_telemetry_batteryHeaterStatus()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_SYSTEM_DATA;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count + 2) & 0x11;
	if (tm == 0x00) 
	{
		return false;
	}
	else 
	{
		return true;
	}
}

int i2c_eps_telemetry_safetyHazardEnvironment()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	uint16_t tm = ((adc_count + 2) >> 16) & 0x11;
	return data;
}


// END ________________________________________________________________________________




int i2c_eps_getTelemetryGroup(families, branch)
{
	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry

	// possible families
	// 0x00 BCRs
	// 0x01 Solar Panel Sensors
	// 0x02 Power Buses
	// 0x03 Switchable Power Buses
	// 0x04 Battery Module
	// 0x05 System Data

	// add user input here
	buffer[2] = families;


	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	/* get the ADC count returned using bitwise operations
	 * adc_count will be a 32-bit int of the form:
	 * {0x00     0x00     g_slave_buff[1]     g_slave_buff[0]}
	 * I don't know if this calculation is correct
	 */


	 // Depending on family, there is a limit to which branch can be selected
	 // 24 bytes		BCRs
	 // 10 bytes		Solar Panel Temperature
	 // 24 bytes		Power Module
	 // 24 bytes		Switchable Power Buses
	 // 16 bytes		Battery Module Telemetry
	 // 12 bytes		System Data

	if (branch == 0)
	{
		adc_count = (g_slave_buff[1] << 8) | (g_slave_buff[0]);
	}
	else if (branch == 2)
	{
		adc_count = (g_slave_buff[3] << 8) | (g_slave_buff[2]);
	}
	else if (branch == 4)
	{
		adc_count = (g_slave_buff[5] << 8) | (g_slave_buff[4]);
	}
	else if (branch == 6)
	{
		adc_count = (g_slave_buff[7] << 8) | (g_slave_buff[6]);
	}
	else if (branch == 8)
	{
		adc_count = (g_slave_buff[9] << 8) | (g_slave_buff[8]);
	}
	else if (branch == 10)
	{
		adc_count = (g_slave_buff[11] << 8) | (g_slave_buff[10]);
	}
	else if (branch == 12)
	{
		adc_count = (g_slave_buff[13] << 8) | (g_slave_buff[12]);
	}
	else if (branch == 14)
	{
		adc_count = (g_slave_buff[15] << 8) | (g_slave_buff[14]);
	}
	else if (branch == 16)
	{
		adc_count = (g_slave_buff[17] << 8) | (g_slave_buff[16]);
	}
	else if (branch == 18)
	{
		adc_count = (g_slave_buff[19] << 8) | (g_slave_buff[18]);
	}
	else if (branch == 20)
	{
		adc_count = (g_slave_buff[21] << 8) | (g_slave_buff[20]);
	}
	else if (branch == 22)
	{
		adc_count = (g_slave_buff[23] << 8) | (g_slave_buff[21]);
	}
	else
	{
		adc_count = 12121212; // error
	}

	int done;
	// for calculations on which family
	if (families == 0x00)
	{
		done = telemetry_bcrs(adc_count, branch);
	}
	else if (families == 0x01)
	{
		done = telemetry_solarPanelSensors(adc_count, branch);
	}
	else if (families == 0x02)
	{
		done = telemetry_powerBuses(adc_count, branch);
	}
	else if (families == 0x03)
	{
		done = telemetry_switchablePowerBuses(adc_count, branch);
	}
	else if (families == 0x04)
	{
		done = telemetry_batteryModule(adc_count, branch);
	}
	else if (families == 0x05)
	{
		done = telemetry_systemData(adc_count, branch);
	}
	else
	{
		done = 12121212; // error
	}

	return done;
}

int telemetry_bcrs(x)
{
	int y = 0;
	if ((branch % 4 == 0) | (branch == 0))
	{
		y = x * .008;
	}
	else {
		y = x * 2;
	}
	return y;
}



int telemetry_powerBuses(x, branch)
{
	int y = 0;
	if ((branch % 4 == 0) | (branch == 0))
	{
		y = x * 0.0030945;
	}
	else {
		y = x * 0.0020676;
	}
	return y;
}

int telemetry_switchablePowerBuses(x, branch)
{
	int y = 0;
	if ((branch % 4 == 0) | (branch == 0))
	{
		y = x * 0.0030945;
	}
	else {
		y = x * 0.0008336 - 0.010;
	}
	return y;
}

int telemetry_batteryModule(x, branch)
{
	int y = 0;
	if ((branch == 0) | (branch == 2))
	{
		y = x * 4.883;
	}
	else if (branch == 4)
	{
		y = x * 0.26;
	}
	else if (branch == 6)
	{
		y = x * 0.1;
	}
	else if (branch == 8)
	{
		y = x * 0.125;
	}
	else if (branch == 10)
	{
		y = x * 1.6;
	}
	else if (branch == 12)
	{
		y = x;
	}
	else if (branch == 14)
	{
		y = x * 1.042;
	}
	else
	{
		y = 121212; // in case it is out of scope
	}
	return y;
}


// TELEMETRY ENDS HERE
//____________________________________________________________________________________________________________________________


// for bus Reset
// ALSO ASSUMPTION: when asking for byte i am assuming its talking about only one byte of parameter
// otherwise when it uses the term parameter, I am assuming there are 2 bytes of parameter
// bit 1 and 0 = 1 then 3v3
// bit 3 and 2 = 1 then 5v
// bit 5 and 4 = 1 then 12v
// bit 7 and 6 = 1 then Vbat
void i2c_eps_fixedPowerBusReset(uint8_t busReset)
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_FIXED_POWER_BUS_RESET;
	buffer[2] = 0x00 | busReset;

	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused

	return;
}

void i2c_eps_manualReset()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);
	buffer[0] = EPS_SLAVE_ADDR; // do we need to send this as part of data on i2c bus?
	buffer[1] = I2C_EPS_CMD_MANUAL_RESET;
	buffer[2] = 0xFF;

	i2c_read_write_helper(buffer, 4, 0, NO_RETURN);
	(void)adc_count; //unused

	return;
}