#include "i2clcd.h"

extern I2C_HandleTypeDef hi2c1; // change handler accordingly

#define SLAVE_ADDRESS_LCD 0x27 << 1 // change the address accordingly
#define LINES 4
#define COLUMS 20

const uint8_t lcdPos[LINES][COLUMS] = {
	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13},
	{0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53},
	{0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27},
	{0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67}};

void lcd_send_cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C; //en=1, rs=0
	data_t[1] = data_u | 0x08; //en=0, rs=0
	data_t[2] = data_l | 0x0C; //en=1, rs=0
	data_t[3] = data_l | 0x08; //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D; //en=1, rs=1
	data_t[1] = data_u | 0x09; //en=0, rs=1
	data_t[2] = data_l | 0x0D; //en=1, rs=1
	data_t[3] = data_l | 0x09; //en=0, rs=1
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_clear(void)
{
	lcd_send_cmd(0x00);
	for (int i = 0; i < 100; i++)
	{
		lcd_send_data(' ');
	}
}

void lcd_init(void)
{
	// 4 bit initialisation
	HAL_Delay(50); // wait for >40ms
	lcd_send_cmd(0x30);
	HAL_Delay(5); // wait for >4.1ms
	lcd_send_cmd(0x30);
	HAL_Delay(1); // wait for >100us
	lcd_send_cmd(0x30);
	HAL_Delay(10);
	lcd_send_cmd(0x20); // 4bit mode
	HAL_Delay(10);

	// display initialisation
	lcd_send_cmd(0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd(0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd(0x01); // clear display
	HAL_Delay(2);
	lcd_send_cmd(0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd(0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string(const char *str)
{
	uint8_t rep = 195;
	const uint8_t rep_auml = 164;
	const uint8_t rep_ouml = 182;
	const uint8_t rep_uuml = 188;
	const uint8_t rep_sz = 159;
	char *auml = "\xE1";
	char *ouml = "\xEF";
	char *uuml = "\xF5";
	char *sz = "\xE2";
	bool is_cmd = false;

	while (*str)
		if (*str != '\0')
		{
			if (is_cmd)
			{
				switch (*str)
				{
				case rep_auml:
					lcd_send_data(*auml);
					break;
				case rep_ouml:
					lcd_send_data(*ouml);
					break;
				case rep_uuml:
					lcd_send_data(*uuml);
					break;
				case rep_sz:
					lcd_send_data(*sz);
					break;
				default:
					break;
				}
				*str++;
				is_cmd = false;
				continue;
			}
			if (*str == rep)
			{
				*str++;
				is_cmd = true;
				continue;
			}
			lcd_send_data(*str++);
		}
}

void lcd_write(const char *txt, uint8_t line, uint8_t column)
{
	lcd_clear_line(line, column);
	lcd_send_cmd(0x80 | lcdPos[line][column]);
	lcd_send_string(txt);
}

void lcd_clear_line(uint8_t line, uint8_t column)
{
	lcd_send_cmd(0x80 | lcdPos[line][column]);
	for (int i = 0; i < (COLUMS - column); i++)
	{
		lcd_send_data(' ');
	}
}
