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


    //EPS telemetry
    float eps_temperature;
    float eps_bcrOutputCurrent;
    float eps_bcrOutputVoltage;
    float eps_3v3CurrentDrawOfEps;
    float eps_5VCurrentDrawOfEps;
    float eps_outputCurrentOf12VBus;
    float eps_outputVoltageOf12VBus;
    float eps_outputCurrentOfBatteryBus;
    float eps_outputVoltageOfBatteryBus;
    float eps_outputCurrentOf5VBus;
    float eps_outputVoltageOf5VBus;
    float eps_outputCurrentOf3V3Bus;
    float eps_outputVoltageOf3V3Bus;

    eps_temperature = i2c_eps_temperature();
    eps_bcrOutputCurrent = i2c_eps_bcrOutputCurrent();
    eps_bcrOutputVoltage = i2c_eps_bcrOutputVoltage();
    eps_3v3CurrentDrawOfEps = i2c_eps_3v3CurrentDrawOfEps();
    eps_5VCurrentDrawOfEps = i2c_eps_5VCurrentDrawOfEps();
    eps_outputCurrentOf12VBus = i2c_eps_outputCurrentOf12VBus();
    eps_outputVoltageOf12VBus = i2c_eps_outputVoltageOf12VBus();
    eps_outputCurrentOfBatteryBus = i2c_eps_outputCurrentOfBatteryBus();
    eps_outputVoltageOfBatteryBus = i2c_eps_outputVoltageOfBatteryBus();
    eps_outputCurrentOf5VBus = i2c_eps_outputCurrentOf5VBus();
    eps_outputVoltageOf5VBus = i2c_eps_outputVoltageOf5VBus();
    eps_outputCurrentOf3V3Bus = i2c_eps_outputCurrentOf3V3Bus();
    eps_outputVoltageOf3V3Bus = i2c_eps_outputVoltageOf3V3Bus();


    printf("EPS temperature is: %f\n", eps_temperature);
    printf("EPS BCR output current is: %f\n", eps_bcrOutputCurrent);
    printf("EPS BCR output voltage is: %f\n", eps_bcrOutputVoltage);
    printf("I3V3_DRW is: %f\n", eps_3v3CurrentDrawOfEps);
    printf("I5V_DRW is: %f\n", eps_5VCurrentDrawOfEps);
    printf("IPCM12V is: %f\n", eps_outputCurrentOf12VBus);
    printf("VPCM12V is: %f\n", eps_outputVoltageOf12VBus);
    printf("IPCMBATV is: %f\n", eps_outputCurrentOfBatteryBus);
    printf("VPCMBATV is: %f\n", eps_outputVoltageOfBatteryBus);
    printf("IPCM5V is: %f\n", eps_outputCurrentOf5VBus);
    printf("VPCM5V is: %f\n", eps_outputVoltageOf5VBus);
    printf("IPCM3V3 is: %f\n", eps_outputCurrentOf3V3Bus);
    printf("VPCM3V3 is: %f\n", eps_outputVoltageOf3V3Bus);


    // batteries telemetry
    float eps_batteryOutputVoltage;

    eps_batteryOutputVoltage = i2c_eps_batteryOutputVoltage();

    printf("EPS battery output voltage is: %f\n", eps_batteryOutputVoltage);


    



    while (1)
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

//used to reduce repetition
uint32_t i2c_read_write_helper(gmb_0, gmb_1, gmb_2, gmb_3, d) {
    printf("Master will send data :");
    print_i2c_data(g_master_buff);

    // i2c write
    I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Write, I2C_EPS_REG_ADDR, gmb_0, datasize);
    I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Write, I2C_EPS_REG_ADDR, gmb_1, datasize);
    I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Write, I2C_EPS_REG_ADDR, gmb_2, datasize);

    delay(d);

    //i2c read
    I2C_read_write_lp(&master_rtos_handle, &status, I2C_EPS_ADDR, kLPI2C_Read, I2C_EPS_REG_ADDR, g_slave_buff, datasize);

    delay(d);

    printf("Master received data from slave: ");
    print_i2c_data(g_slave_buff);

    uint32_t adc_count = 0;
    /* get the ADC count returned using bitwise operations
     * adc_count will be a 32-bit int of the form:
     * {0x00     0x00     g_slave_buff[1]     g_slave_buff[0]}
     * I don't know if this calculation is correct
     */
    adc_count = (g_slave_buff[1] << 7) | g_slave_buff[0]); ///changed 8 to 7

    return adc_count; 
}

//edited to make easier to create proper delay
void delay(int n)
{
    for (int i = 0; i < n; i++) {}
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

// EPS telemetry 
//_______________________________________________________________________________________________________________
float i2c_eps_temperature()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E3_0; // telemetry = motherboard temperature, first byte
    g_master_buff[3] = I2C_EPS_TELE_TEMPERATURE_MOTHERBOARD_1; // telemetry = motherboard temperature, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 6000);

    float temperature = (0.372434 * adc_count) - 273.15; // from eps datasheet
    return temperature;
}

float i2c_eps_bcrOutputCurrent()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_BCR_OUTPUT_CURRENT_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float bcrOutputCurrent = 14.2757 * adc_count; // from eps datasheet
    return bcrOutputCurrent;
}

float i2c_eps_bcrOutputVoltage()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_BCR_OUTPUT_VOLTAGE_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float bcrOutputVoltage = 0.008993157 * adc_count; // from eps datasheet
    return bcrOutputVoltage;
}

float i2c_eps_3v3CurrentDrawOfEps()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_3V3_CURRENT_DRAW_OF_EPS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float CurrentDrawOfEps_3V3 = 0.001327547 * adc_count; // from eps datasheet
    return CurrentDrawOfEps_3V3;
}

float i2c_eps_5VCurrentDrawOfEps()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_5V_CURRENT_DRAW_OF_EPS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float CurrentDrawOfEps_5V = 0.001327547 * adc_count; // from eps datasheet
    return CurrentDrawOfEps_5V;
}

float i2c_eps_outputCurrentOf12VBus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_CURRENT_OF_12V_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float outputCurrentOf12VBus = 0.00207 * adc_count; // from eps datasheet
    return outputCurrentOf12VBus;
}

float i2c_eps_outputVoltageOf12VBus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_12V_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.00207 * adc_count; // from eps datasheet
    return output;
}

float i2c_eps_outputCurrentOfBatteryBus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_CURRENT_OF_BATTERY_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.005237 * adc_count; // from eps datasheet
    return output;
}

float i2c_eps_outputVoltageOfBatteryBus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_BATTERY_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.008978 * adc_count; // from eps datasheet
    return output;
}

float i2c_eps_outputCurrentOf5VBus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_CURRENT_OF_5V_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.005237 * adc_count; // from eps datasheet
    return output;
}

float i2c_eps_outputVoltageOf5VBus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_5V_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.005865 * adc_count; // from eps datasheet
    return output;
}

float i2c_eps_outputCurrentOf3V3Bus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_CURRENT_OF_3V3_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.005237 * adc_count; // from eps datasheet
    return output;
}

float i2c_eps_outputVoltageOf3V3Bus()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_OUTPUT_VOLTAGE_OF_3V3_BUS_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float output = 0.004311 * adc_count; // from eps datasheet
    return output;
}




// batteries telemetry
//___________________________________________________________________________________________________________
// output voltage 
float i2c_eps_batteryOutputVoltage()
{
    /* Set up i2c master to send data to slave */
    g_master_buff[0] = I2C_EPS_ADDR; // i2c slave address = EPS motherboard
    g_master_buff[1] = I2C_EPS_CMD_TELEMETRY; // i2c command = get EPS telemetry
    g_master_buff[2] = I2C_EPS_TELE_E2_0; // telemetry = motherboard battery, first byte
    g_master_buff[3] = I2C_EPS_TELE_BATTERY_OUTPUT_VOLTAGE_1; // telemetry = motherboard battery, second byte

    i2c_read_write_helper(g_master_buff[0], g_master_buff[1], g_master_buff[2], g_master_buff[3], 5000);

    float outputVoltage = 0.008993 * adc_count; // from eps datasheet
    return ouputVoltage;
}