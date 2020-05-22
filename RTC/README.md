The EPS team developed a real-time clock circuit (RTC) to be included on the OBC, providing accurate information about time elapsed even when the battery is disconnected from the satellite. The RTC uses Maxim Integregated’s DS1307 integrated chip and a CR-2032 battery.

# DS1307

The DS1307 is a low power, serial real-time clock. It sources power both from a VCC pin, and an external coin cell battery. Time is programmed/read via I2C. Time is kept by the chip, using an external 32.768 kHz crystal.
The DS1307 was selected for several reasons:
-	It has extremely minimal features and low power consumption. Many other RTC  chips include features such as alarms and interrupts, but these are not required for SOC-i. The DS1307 keeps the time and does nothing else, leading to very small footprint and power consumption.
-	Ease of use. The chip has few features, it does not require many external components, it is available in a through-hole package, and it has been used extensively by hobbyists online.

# Usage

Using I2C, the time first must be programmed into the chip, and counting must be enabled. After this point, the chip will keep track of time indefinitely, as long as it continues to receive power.

The device uses I2C address 0x68. The time is stored in an array of onboard RAM, starting with seconds, at address 0x00. Minutes are stored at 0x01, hours are stored at 0x02, day of the week (i.e., Sunday – Saturday) is stored at 0x03, day of the month is stored at 0x04, month is stored at 0x05, year at 0x06, and 0x07 is used as a control register. Data is stored as a binary coded decimal.

For example, if setting the day of the month to be 27, a user should program 0x27 to address 0x04 of the chip’s RAM.

Reading the time from the chip is done via I2C and works the same way as writing.
