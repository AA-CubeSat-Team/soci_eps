#pragma once
#include <stdint.h>

#include "soci_cdh_drivers\cdh_i2c_driver\drivers\fsl_common.h"
#include "soci_cdh_drivers\cdh_i2c_driver\board\board.h"

/******************** I2C definitions taken from soci_cdh_drivers\cdh_i2c_driver\source\freertos_lpi2c.c ********************/

#define EXAMPLE_I2C_MASTER_BASE (LPI2C1_BASE)
#define EXAMPLE_I2C_MASTER_IRQN (LPI2C1_IRQn)
#define EXAMPLE_I2C_SLAVE_BASE (LPI2C3_BASE)
#define EXAMPLE_I2C_SLAVE_IRQN (LPI2C3_IRQn)

/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define LPI2C_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

/* clang-format off */
/*
The example support single board communication(one instance works as master, another
instance works as slave) or board to board communication. Default is single board mode.
*/
#define SINGLE_BOARD 0
#define BOARD_TO_BOARD 1
#ifndef EXAMPLE_CONNECT_I2C
#define EXAMPLE_CONNECT_I2C SINGLE_BOARD
#endif /* EXAMPLE_CONNECT_I2C */
#if (EXAMPLE_CONNECT_I2C == BOARD_TO_BOARD)
#define isMASTER 0
#define isSLAVE 1
#ifndef I2C_MASTER_SLAVE
#define I2C_MASTER_SLAVE isMASTER
#endif /* I2C_MASTER_SLAVE */
#endif /* EXAMPLE_CONNECT_I2C */

#define EXAMPLE_I2C_MASTER   ((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#define EXAMPLE_I2C_SLAVE   ((LPI2C_Type *)EXAMPLE_I2C_SLAVE_BASE)

#define I2C_MASTER_SLAVE_ADDR_7BIT (0x7EU)
#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_DATA_LENGTH (6) /* MAX is 256 */


/******************** EPS definitions ********************/
#define I2C_EPS_ADDR 0x2B
#define I2C_BAT_ADDR 0x2A
#define I2C_EPS_CMD_BOARD_STATUS 0x01

//added byte[0] 0x00
#define I2C_EPS_DATA0 0x00

// I2C eps commands 
#define I2C_EPS_CMD_TELEMETRY 0x10
//added 8/30
#define I2C_EPS_CMD_BOARD_STATUS 0x01
#define I2C_EPS_CMD_GET_LAST_ERROR 0x03
#define I2C_EPS_CMD_GET_VERSION 0x04
#define I2C_EPS_CMD_GET_CHECKSUM 0x05
#define I2C_EPS_CMD_GET_REVISION 0x06
//added 9/3
# define I2C_EPS_CMD_GET_COM_WATCHDOG_PER 0x20
# define I2C_EPS_CMD_SET_COM_WATCHDOG_PER 0x21
# define I2C_EPS_CMD_RESET_COM_WATCHDOG 0x22
# define I2C_EPS_CMD_GET_NUM_BROWN_OUT_RESET 0x31
# define I2C_EPS_CMD_GET_NUM_AUTO_SOFTWARE_RESET 0x32
// added 9/9
# define I2C_EPS_CMD_GET_NUM_MANUAL_RESET 0x33
# define I2C_EPS_CMD_GET_NUM_COMMS_WATCHDOG_RESET 0x34
# define I2C_EPS_CMD_SWITCH_ON_ALL_PDM 0x40
# define I2C_EPS_CMD_SWITCH_OFF_ALL_PDM 0x41
# define I2C_EPS_CMD_GET_ACTUAL_STATE_OF_ALL_PDM 0x42
// added 9/27
# define I2C_EPS_CMD_GET_EXPECTED_STATE_OF_ALL_PDM 0x43
# define I2C_EPS_CMD_GET_INITIAL_STATE_OF_ALL_PDM 0x44
# define I2C_EPS_CMD_SET_ALL_PDM_TO_INITIAL_STATE 0x45
# define I2C_EPS_CMD_SWITCH_PDMN_ON 0x50
# define I2C_EPS_CMD_SWITCH_PDMN_OFF 0x51
# define I2C_EPS_CMD_SET_PDMN_INITIAL_STATE_TO_ON 0x52
# define I2C_EPS_CMD_SET_PDMN_INITIAL_STATE_TO_OFF 0x53
# define I2C_EPS_CMD_GET_PDMN_ACTUAL_STATUS 0x54
# define I2C_EPS_CMD_SET_PDMN_TIMER_LIMIT 0x60
# define I2C_EPS_CMD_GET_PDMN_TIMER_LIMIT 0x61
# define I2C_EPS_CMD_GET_PDMN_CURRENT_TIMER_VALUE 0x62
# define I2C_EPS_CMD_PCM_RESET 0x70
# define I2C_EPS_CMD_MANUAL_RESET 0x80




// breaking apart 0xE308 into two bytes, 0x3E and 0x08. Might be a better way to do this.
// the first 2 bit of TLE code 
#define I2C_EPS_TELE_E1_0 0xE1
#define I2C_EPS_TELE_E3_0 0xE3
#define I2C_EPS_TELE_E2_0 0xE2
#define I2C_EPS_TELE_E4_0 0xE4


// going in order of the telemetry eps sheet
//For the E2 methods
#define I2C_EPS_TELE_BATTERY_OUTPUT_VOLTAGE_1 0x80
#define I2C_EPS_TELE_BCR_OUTPUT_CURRENT_1 0x84
#define I2C_EPS_TELE_BCR_OUTPUT_VOLTAGE_1 0x80
#define I2C_EPS_TELE_3V3_CURRENT_DRAW_OF_EPS_1 0x05
#define I2C_EPS_TELE_5V_CURRENT_DRAW_OF_EPS_1 0x15
#define I2C_EPS_TELE_OUTPUT_CURRENT_OF_12V_BUS_1 0x34
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_12V_BUS_1 0x30
#define I2C_EPS_TELE_OUTPUT_CURRENT_OF_BATTERY_BUS_1 0x24
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_BATTERY_BUS_1 0x20
#define I2C_EPS_TELE_OUTPUT_CURRENT_OF_5V_BUS_1 0x14
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_5V_BUS_1 0x10
#define I2C_EPS_TELE_OUTPUT_CURRENT_OF_3V3_BUS_1 0x04
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_3V3_BUS_1 0x00

// added 8/8
// Voltage/Current for Switches 1-10
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_1_1 0x10
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_1_1 0x14
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_2_1 0x20
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_2_1 0x24
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_3_1 0x30
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_3_1 0x34
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_4_1 0x40
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_4_1 0x44
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_5_1 0x50
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_5_1 0x54
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_6_1 0x60
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_6_1 0x64
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_7_1 0x70
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_7_1 0x74
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_8_1 0x80
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_8_1 0x84
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_9_1 0x90
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_9_1 0x94
#define I2C_EPS_TELE_OUTPUT_VOLTAGE_SWITCH_10_1 0xA0
#define I2C_EPS_TELE_OUTPUT_CURRENT_SWITCH_10_1 0xA4

//For the E3 methods
#define I2C_EPS_TELE_TEMPERATURE_MOTHERBOARD_1 0x08

// added 8/11
// Table 11-9
#define I2C_EPS_TELE_VOLTAGE_FEEDING_BCR1_1 0x10
#define I2C_EPS_TELE_CURRENT_BCR1_CONNECTOR_SA1A_1 0x14
#define I2C_EPS_TELE_CURRENT_BCR1_CONNECTOR_SA1B_1 0x15
#define I2C_EPS_TELE_ARRAY_TEMP_CONNECTOR_SA1A_1 0x18
#define I2C_EPS_TELE_ARRAY_TEMP_CONNECTOR_SA1B_1 0x19
#define I2C_EPS_TELE_SUN_DETECTOR_CONNECTOR_SA1A_1 0x1C
#define I2C_EPS_TELE_SUN_DETECTOR_CONNECTOR_SA1B_1 0x1D

#define I2C_EPS_TELE_VOLTAGE_FEEDING_BCR2_1 0x20
#define I2C_EPS_TELE_CURRENT_BCR2_CONNECTOR_SA2A_1 0x24
#define I2C_EPS_TELE_CURRENT_BCR2_CONNECTOR_SA2B_1 0x25
#define I2C_EPS_TELE_ARRAY_TEMP_CONNECTOR_SA2A_1 0x28
#define I2C_EPS_TELE_ARRAY_TEMP_CONNECTOR_SA2B_1 0x29
#define I2C_EPS_TELE_SUN_DETECTOR_CONNECTOR_SA2A_1 0x2C
#define I2C_EPS_TELE_SUN_DETECTOR_CONNECTOR_SA2B_1 0x2D

#define I2C_EPS_TELE_VOLTAGE_FEEDING_BCR3_1 0x30
#define I2C_EPS_TELE_CURRENT_BCR3_CONNECTOR_SA3A_1 0x34
#define I2C_EPS_TELE_CURRENT_BCR3_CONNECTOR_SA3B_1 0x35
#define I2C_EPS_TELE_ARRAY_TEMP_CONNECTOR_SA3A_1 0x38
#define I2C_EPS_TELE_ARRAY_TEMP_CONNECTOR_SA3B_1 0x39
#define I2C_EPS_TELE_SUN_DETECTOR_CONNECTOR_SA3A_1 0x3C
#define I2C_EPS_TELE_SUN_DETECTOR_CONNECTOR_SA3B_1 0x3D

// added 11/3/20 (new eps check later)
//__________________________________________________________
// commands pg 9
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