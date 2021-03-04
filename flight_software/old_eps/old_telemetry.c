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

#define BYTE16CAST 0xFF
#define VERIFIED_COM_MASK 0x67D6
#define NO_RETURN 9999


#define EPS_SLAVE_ADDR 0 //change
#define I2C_DATA_LENGTH 4 //?

uint8_t buffer[I2C_DATA_LENGTH];

static uint32_t adc_count;




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

	uint32_t returnArray[12];
	i2c_read_write_helper(buffer, 4, returnArray, 5000);

	uint16_t tm = returnArray & BYTE16CAST;
	return tm * .008;

}

int i2c_eps_telemetry_bcr8w_1_inputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	uint32_t returnArray[12];
	i2c_read_write_helper(buffer, 4, returnArray, 5000);

	uint16_t tm = (returnArray >> 16) & BYTE16CAST;
	return tm * 2;
}

int i2c_eps_telemetry_bcr8w_1_outputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	uint32_t returnArray[12];
	i2c_read_write_helper(buffer, 4, returnArray, 5000);

	uint16_t tm = returnArray[1] & BYTE16CAST; // I am unsure but I think + 1 should move to the next 4 bytes of array in read data
	return tm * .008; 

}

int i2c_eps_telemetry_bcr8w_1_outputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	uint32_t returnArray[12];
	i2c_read_write_helper(buffer, 4, returnArray, 5000);

	uint16_t tm = (returnArray[1] >> 16) & BYTE16CAST;
	return tm * 2;
}

int i2c_eps_telemetry_bcr8w_2_inputVoltage()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	uint32_t returnArray[12];
	i2c_read_write_helper(buffer, 4, returnArray, 5000);

	uint16_t tm = returnArray[2] & BYTE16CAST;

	return tm * .008;
}

int i2c_eps_telemetry_bcr8w_2_inputCurrent()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BCRS;

	uint32_t returnArray[12];
	i2c_read_write_helper(buffer, 4, returnArray, 5000);

	uint16_t tm = (returnArray[2] >> 16) & BYTE16CAST;

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

	uint16_t tm = (adc_count + 3) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 3) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 4) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 4) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 5) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 5) >> 16) & BYTE16CAST;
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

	uint16_t tm = adc_count & BYTE16CAST;
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

	uint16_t tm = (adc_count >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 1) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 1) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 2) & BYTE16CAST;
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

	uint16_t tm = adc_count & BYTE16CAST;
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

	uint16_t tm = (adc_count >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 1) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 1) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 2) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 2) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 3) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 3) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 4) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 4) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 5) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 5) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count) & BYTE16CAST;
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

	uint16_t tm = (adc_count >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 1) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 1) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 2) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 2) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 3) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 3) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 4) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 4) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 5) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 5) >> 16) & BYTE16CAST;
	return tm * 0.0008336 - 0.010;
}

//END_______________________________________________________________________________________

// BATTERY MODULE TELEMETRY_________________________________________________________________

// twos compliment times 0.5 which is needed to convert the temperatures below
static int twosComp (uint16_t x)
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

int binaryToInt (uint16_t x)
{

}

int i2c_eps_telemetry_vbat_1()
{
	memset(buffer, 0, sizeof(*buffer)*I2C_DATA_LENGTH);

	/* Set up i2c master to send data to slave */
	buffer[0] = EPS_SLAVE_ADDR; // i2c slave address = EPS motherboard
	buffer[1] = I2C_EPS_CMD_GET_TELEMETRY_GROUP; // i2c command = get EPS telemetry
	buffer[2] = I2C_EPS_TELE_BATTERY_MODULE;

	i2c_read_write_helper(buffer, 4, &adc_count, 5000);

	uint16_t tm = (adc_count) & BYTE16CAST;
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

	uint16_t tm = (adc_count >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 1) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 1) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 2) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 2) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 3) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 3) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count) & BYTE16CAST;
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

	uint16_t tm = ((adc_count) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 1) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 1) >> 16) & BYTE16CAST;
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

	uint16_t tm = (adc_count + 2) & BYTE16CAST;
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

	uint16_t tm = ((adc_count + 2) >> 16) & BYTE16CAST;
	return data;
}


// END ________________________________________________________________________________
