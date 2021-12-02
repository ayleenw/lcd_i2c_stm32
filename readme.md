# Library for an I2C LCD display to connect to an STM32 controller

Tested with STM32F072RB and a 20x4\* character LCD conforming to the HD44780 industrial standard with a PCF8574 adapter chip for I2C serial communication.

Based on the code from Controllerstech: https://controllerstech.com/lcd-20x4-using-i2c-with-stm32/

## Usage

Place .h in Inc folder, .c in Src folder of your STM32 project.

Initialize once with lcd_init().

Use lcd_write(const char \*txt, uint8_t line, uint8_t column) to send texts to the display. This wraps the lcd_send_cmd (to set line and column) and lcd_send_string (the actual text to send) in one function.

## Remarks

In my case, I had to shift the I2C address one bit to the left:
#define SLAVE_ADDRESS_LCD 0x27 << 1
Just change the hex value acc. to your I2C expander module.

\*) The code may also be used for a 16x2 character LCD, change the lcdPos array acc. to the DDRAM addresses shown on the above mentioned Controllerstech page.

I added support for the German umlauts äöü and the ß character (sz in German). For the umlauts, only small letters are included in the LCDs ROM. More special characters could be added, following the example in the lcd_send_string function. See HD44780 datasheet for available character in the LCDs ROM.
