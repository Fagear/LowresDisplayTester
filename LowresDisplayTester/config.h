/**************************************************************************************************************************************************************
config.h

Copyright © 2024 Maksim Kryukov <fagear@mail.ru>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Created: 2024-02

Part of the [LowresDisplayTester] project.
Pre-compile configuration file.

**************************************************************************************************************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

// These setting directly affect how much ROM will be required from MCU.
// Note that device like ATmega48 or ATmega88 can not fit all listed options!
// Video sync generation available for all supported MCUs,
// other options can/should be switched off.
//#define CONF_NO_DEBUG_PINS			// Disable debugging pins
#define CONF_EN_HD44780P			// Enable support for character displays with HD44780/KS0066/КБ1013ВГ6/SPLC780 parallel controllers
//#define CONF_EN_I2C					// Enable support for I2C displays and expanders
//#define CONF_EN_UART				// Enable support for UART displays testing

#ifdef CONF_EN_HD44780P
	#define CONF_EN_CHARDISP		// Enable support for character displays testing
#elif defined CONF_EN_I2C
	#define CONF_EN_HD44780S		// Enable support for character displays with HD44780/KS0066/КБ1013ВГ6/SPLC780 controllers via serial link
	#define CONF_EN_CHARDISP		// Enable support for character displays testing
#endif /* CONF_EN_HD44780P */

#ifdef CONF_EN_UART
	#define UART_IN_LEN			8	// UART receiving buffer length
	#define UART_OUT_LEN		384U// UART transmitting buffer length
#endif /* CONF_EN_UART */

// I2C device addresses (including zeroed R/W bit).
enum
{
	I2C_GEN_CALL		= 0x00,		// General call
	I2C_PCF8574_START	= 0x40,		// Used on I2C - HD44780 "backpacks"
	I2C_PCF8574_END		= 0x4E,		// Used on I2C - HD44780 "backpacks"
	I2C_PCF8574A_START	= 0x70,		// Used on I2C - HD44780 "backpacks"
	I2C_PCF8574A_END	= 0x7E,		// Used on I2C - HD44780 "backpacks"
	I2C_SSD1306_ADR1	= 0x78,		// Used on OLED graphic ~1" displays
	I2C_SSD1306_ADR2	= 0x7A,		// Used on OLED graphic ~1" displays
	I2C_US2066_ADR1		= 0x78,		// Used on OLED HD44780-compatible displays
	I2C_US2066_ADR2		= 0x7A,		// Used on OLED HD44780-compatible displays
	I2C_ST7032I_ADR		= 0x7C,		// LCD character display driver
	I2C_HT16K33_START	= 0xE0,		// LED/button driver
	I2C_HT16K33_END		= 0xEE,		// LED/button driver
	I2C_RESERVED		= 0xF0,		// Start of the reserved address space
};

enum
{
	HW_DISP_44780		= (1<<0),	// Presence of HD44780-compatible display
	HW_DISP_I2C_40		= (1<<1),	// Presence of at least one I2C device in 0x40...0x4E range
	HW_DISP_I2C_70		= (1<<2),	// Presence of at least one I2C device in 0x70...0x7E range
	HW_DISP_I2C_78		= (1<<3),	// Presence of a I2C device at 0x78 address
	HW_DISP_I2C_7A		= (1<<4),	// Presence of a I2C device at 0x7A address
	HW_DISP_I2C_7C		= (1<<5),	// Presence of a I2C device at 0x7C address
	HW_DISP_I2C_E0		= (1<<6),	// Presence of at least one I2C device in 0xE0...0xEE range
};

#endif /* CONFIG_H_ */
