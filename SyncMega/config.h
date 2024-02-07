/*
 * config.h
 *
 * Created: 01.02.2024 17:23:20
 *  Author: kryukov
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

//#define CONF_NO_DEBUG_PINS			// Disable debugging pins
//#define CONF_EN_HD44780				// Enable support for character displays with HD44780/KS0066/ÊÁ1013ÂÃ6/SPLC780 parallel controllers

#ifdef CONF_EN_HD44780
	#define CONF_EN_CHARDISP		// Enable support for character displays testing.
#endif /* CONF_EN_HD44780 */

// I2C devices.
enum
{
	US2066,
	I2C_PCF8574_START	= 0x20,
	I2C_PCF8574_END		= 0x27,
	I2C_HT16K33_START	= 0x70,
	I2C_HT16K33_END		= 0x77,
	I2C_SSD1306_ADR1	= 0x3C,
	I2C_SSD1306_ADR2	= 0x3D,
	I2C_ST7032_ADR		= 0x3E,
};

enum
{
	HW_DISP_44780 = (1<<0),			// Presence of HD44780-compatible display
};

#endif /* CONFIG_H_ */