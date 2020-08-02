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
#define I2C_EPS_CMD_TELEMETRY 0x10

// breaking apart 0xE308 into two bytes, 0x3E and 0x08. Might be a better way to do this.
// the first 2 bit of TLE code 
#define I2C_EPS_TELE_E3_0 0xE3
#define I2C_EPS_TELE_E2_0 0xE2

//For the E3 methods
#define I2C_EPS_TELE_TEMPERATURE_MOTHERBOARD_1 0x08

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
