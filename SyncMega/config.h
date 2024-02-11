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

//#define CONF_NO_DEBUG_PINS			// Disable debugging pins
#define CONF_EN_HD44780				// Enable support for character displays with HD44780/KS0066/ÊÁ1013ÂÃ6/SPLC780 parallel controllers
//#define CONF_EN_I2C					// Enable support for I2C displays and expanders

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
	I2C_US2066_ADR1		= 0x3C,
	I2C_US2066_ADR2		= 0x3D,
};

enum
{
	HW_DISP_44780 = (1<<0),			// Presence of HD44780-compatible display
};

// Horizontal clocks
// 16000000 / 508 = 8000000 / 254 = 4000000 / 127 = 31496 (31476)
// 16000000 / 1024 = 8000000 / 512 = 4000000 / 256 = 2000000 / 128 = 1000000 / 64 = 15625
// 16000000 / 1017 = 0 (15734,279)

#endif /* CONFIG_H_ */
