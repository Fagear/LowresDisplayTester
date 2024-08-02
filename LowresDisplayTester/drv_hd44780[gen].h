/**************************************************************************************************************************************************************
drv_hd44780[gen].h

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

Created: 2024-08-01

Part of the [LowresDisplayTester] project.
HD44780-compatible (HD44780/KS0066/КБ1013ВГ6/PT6314/SPLC780/US2066) character display driver
for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

TODO

**************************************************************************************************************************************************************/

#ifndef DRV_HD44780_GEN_H_
#define DRV_HD44780_GEN_H_

#include "drv_cpu.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

enum
{
	HD44780_MAX_DDRAM		= 0x67,		// Last bytes of the DDRAM in the display
	HD44780_MAX_COL			= 39,		// Last index of visible column of the display
	HD44780_MAX_ROWS		= 3,		// Last index of visible rows of the display
};

// MSBs for writing a command.
enum
{
	HD44780_CMD_CLR			= (1<<0),	// "Clear display"
	HD44780_CMD_HOME		= (1<<1),	// "Return home"
	HD44780_CMD_ENTRY		= (1<<2),	// "Entry mode set"
	HD44780_CMD_DISP		= (1<<3),	// "Display ON/OFF control"
	HD44780_CMD_SHIFT		= (1<<4),	// "Cursor or display shift"
	HD44780_CMD_FUNC		= (1<<5),	// "Function set"
	HD44780_CMD_SCGR		= (1<<6),	// "Set Character Generator RAM address"
	HD44780_CMD_SDDR		= (1<<7),	// "Set Display Data RAM address"
};

// Bits for "Entry mode set" command.
enum
{
	HD44780_ENTRY_RIGHT		= (1<<1),	// Shift right on data access (shift left if not set)
	HD44780_ENTRY_SHDISP	= (1<<0),	// Shift display on data access (shift cursor if not set)
};

// Bits for "Display ON/OFF control" command.
enum
{
	HD44780_DISP_SCREEN		= (1<<2),	// Enable display output (off if not set)
	HD44780_DISP_CURSOR		= (1<<1),	// Enable cursor drawing (no cursor if not set)
	HD44780_DISP_BLINK		= (1<<0),	// Enable blinking in position of the cursor (static if not set)
};

// Bits for "Cursor or display shift" command.
enum
{
	HD44780_SHIFT_DISP		= (1<<3),	// Force shifting display (shift cursor if not set)
	HD44780_SHIFT_RIGHT		= (1<<2),	// Force shifting to the right (to the left if not set)
};

// Bits for "Function set" command.
enum
{
	HD44780_FUNC_8BIT		= (1<<4),	// 8-bit data bus (4-bit if not set)
	HD44780_FUNC_2LINE		= (1<<3),	// 2-line display access mode (1-line if not set)
	HD44780_FUNC_5X10		= (1<<2),	// 5x10 CG font selected (5x8 CG font if not set) (only 5x8 font when 2-line mode selected)
	HD44780_FUNC_CP			= (1<<1),	// Codepage, CP1251 Cyrillic character set
};

// What to send to the display.
enum
{
	HD44780_CMD,			// Send a command to the display
	HD44780_DATA,			// Send a raw symbol to the display
	HD44780_DIGIT,			// Send a digit to the display (convert to char)
	HD44780_NONZERO_DIGIT,	// Send a digit to the display (convert to char, zeros to spaces)
	HD44780_HEX_DIGIT,		// Send a hex-digit to the display (convert to char)
	HD44780_NUMBER,			// Send a number to the display (convert to chars)
	HD44780_NUMBER_SPACES,	// Send a number to the display (convert to chars, no leading zeros)
	HD44780_NUMBER_HEX,		// Send a number to the display (convert to HEX)
};

// Special ASCII codes.
enum
{
	ASCII_SPACE				= 0x20,		// Space character
	ASCII_ZERO				= 0x30,		// 0-digit character (offset for conversion [uint8_t] to digit)
	ASCII_A					= 0x37,		// 'A' symbol for hex digit
	ASCII_OVF				= 0x23		// Filler for digit overflow (trying to print number >10 into one digit)
};

// Error codes.
enum
{
	HD44780_OK = 0,				// Everything went fine
	HD44780_ERR_BUSY = (1<<0),	// Error while working with the display (BUSY wait fail)
	HD44780_ERR_DATA = (1<<1),	// Error in input parameters of a function
	HD44780_ERR_BUS = (1<<2),	// Error while testing display data bus
};

// Bus and RAM test data.
enum
{
	HD44780_TEST_CHAR1 = 0x5A,				// Char for display RAM testing, pattern 1
	HD44780_TEST_CHAR2 = 0xA5,				// Char for display RAM testing, pattern 2
	HD44780_TEST_ADDR = 0x67,				// Display RAM address for testing data bus
	HD44780_TEST_CG_MASK = 0x1F,			// Mask for CGRAM area (PT6314 doesn't allow MSBs to change)
};

void hd44780_set_device(void *f_wr_cmd, void *f_wr_data, void *f_rd_data);
uint8_t hd44780_upload_symbol_flash(uint8_t symbol_number, const int8_t *symbol_array);
uint8_t hd44780_set_xy_position(uint8_t x_pos, uint8_t y_pos);
uint8_t hd44780_set_x_position(uint8_t x_pos);
uint8_t hd44780_clear_line(const uint8_t line_idx);
uint8_t hd44780_digit_to_data(const uint8_t send_number, const uint8_t send_mode);
uint8_t hd44780_write_8bit_number(const uint8_t send_number, const uint8_t send_mode);	
uint8_t hd44780_write_flash_string(const int8_t *str_output);
uint8_t hd44780_write_string(uint8_t *str_output);
uint8_t hd44780_ddram_read(uint8_t x_pos, uint8_t *read_result);
uint8_t hd44780_cgram_read(uint8_t char_idx, uint8_t *read_result);
uint8_t hd44780_selftest(void);

#endif /* DRV_HD44780_GEN_H_ */