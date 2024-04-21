/**************************************************************************************************************************************************************
drv_hd44780[serial].h

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
HD44780-compatible (HD44780/KS0066/ÊÁ1013ÂÃ6/PT6314/SPLC780/US2066) character display serial driver
for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

Initialization routine [HD44780s_init()] is provided.
Single-byte operations are performed through [HD44780s_write_command_byte()] and [HD44780_read_byte()].
Driver supports cyrillic displays on ÊÁ1013ÂÃ6 controller with CP1251 codepage
and re-encoding ASCII values for other displays if [HD44780_RU_REENCODE] is set.

There are several service functions, for example:
- [HD44780_upload_symbol_flash()] for uploading custom symbols to the display's CGRAM from MCUs ROM
- [HD44780_set_xy_position()] for setting cursor location on the display
- [HD44780_clear_line()] for clearing one line of the display
- [HD44780_write_8bit_number()] for printing a value of 8-bit integer
- [HD44780_write_flash_string] for printing a string from MCUs ROM

The driver provides direct-to-display prints (if [HD44780_USE_DIRECT_STRINGS] is set)
and/or buffered output with two pages in MCUs RAM (if [HD44780_USE_SCREEN_BUFFERS] is set).
Pages can be swapped with [HD44780_swap_display_pages()].
Buffers for printing and for display output can be selected manually
with [HD44780_select_fill_page()] and [HD44780_select_display_page()] at any time.

Service functions for buffered access start from "HD44780_buf_" instead of "HD44780_".

**************************************************************************************************************************************************************/

#ifndef DRV_HD44780_SERIAL_H_
#define DRV_HD44780_SERIAL_H_

#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#ifdef CONF_EN_HD44780

// US2066/ST7032 controller transmittion structure.
// Serial packet structure.
enum
{
	S_PCKT_CTRL,					// Control byte
	S_PCKT_DATA,					// Data byte
	S_PCKT_LEN
};

// Control byte structure.
enum
{
	SER_LAST_BIT	= (1<<7),		// Flag indicating next byte will be last in transmittion
	SER_DATA_BIT	= (1<<6),		// Flag indicating next byte will contain data (not command)
};

enum
{
	SER_TRY_MAX		= 5,			// Maximum number of attempts to send data
	SER_STR_LEN_MAX	= 15,			// Maximum length of a string to send
};

// HD44780 serial display PCF8574 pinout.
#ifndef HD44780S_A0
	#pragma message("HD44780 serial driver is using internal defines for IO pins")
	#define HD44780S_A0			(1<<0)
	#define HD44780S_RW			(1<<1)
	#define HD44780S_E			(1<<2)
	#define HD44780S_D4			(1<<4)
	#define HD44780S_D5			(1<<5)
	#define HD44780S_D6			(1<<6)
	#define HD44780S_D7			(1<<7)
	#define HD44780S_BL			(1<<3)
#endif /* HD44780_A0 */

// MSBs for writing a command.
enum
{
	HD44780S_CMD_CLR	= (1<<0),	// "Clear display"
	HD44780S_CMD_HOME	= (1<<1),	// "Return home"
	HD44780S_CMD_ENTRY	= (1<<2),	// "Entry mode set"
	HD44780S_CMD_DISP	= (1<<3),	// "Display ON/OFF control"
	HD44780S_CMD_SHIFT	= (1<<4),	// "Cursor or display shift"
	HD44780S_CMD_FUNC	= (1<<5),	// "Function set"
	HD44780S_CMD_SCGR	= (1<<6),	// "Set Character Generator RAM address"
	HD44780S_CMD_SDDR	= (1<<7),	// "Set Display Data RAM address"
};

// Bits for "Entry mode set" command.
enum
{
	HD44780S_ENTRY_RIGHT	= (1<<1),	// Shift right on data access (shift left if not set)
	HD44780S_ENTRY_SHDISP	= (1<<0),	// Shift display on data access (shift cursor if not set)
};

// Bits for "Display ON/OFF control" command.
enum
{
	HD44780S_DISP_SCREEN	= (1<<2),	// Enable display output (off if not set)
	HD44780S_DISP_CURSOR	= (1<<1),	// Enable cursor drawing (no cursor if not set)
	HD44780S_DISP_BLINK		= (1<<0),	// Enable blinking in position of the cursor (static if not set)
};

// Bits for "Cursor or display shift" command.
enum
{
	HD44780S_SHIFT_DISP		= (1<<3),	// Force shifting display (shift cursor if not set)
	HD44780S_SHIFT_RIGHT	= (1<<2),	// Force shifting to the right (to the left if not set)
};

// Bits for "Function set" command.
enum
{
	HD44780S_FUNC_8BIT	= (1<<4),	// 8-bit data bus (4-bit if not set)
	HD44780S_FUNC_2LINE	= (1<<3),	// 2-line display access mode (1-line if not set)
	HD44780S_FUNC_5X10	= (1<<2),	// 5x10 CG font selected (5x8 CG font if not set) (only 5x8 font when 2-line mode selected)
};

// Error codes.
enum
{
	HD44780S_OK			= 0,		// Everything went fine
	HD44780S_ERR_BUSY	= (1<<0),	// Error while working with the display (BUSY wait fail)
	HD44780S_ERR_DATA	= (1<<1),	// Error in input parameters of a function
	HD44780S_ERR_BUS	= (1<<2),	// Error while testing display data bus
};

void HD44780s_set_address(uint8_t addr);
uint8_t HD44780s_init(void);
uint8_t HD44780s_write_command_byte(const uint8_t send_data);
uint8_t HD44780s_write_data_byte(const uint8_t send_data);
uint8_t HD44780s_set_xy_position(uint8_t x_pos, uint8_t y_pos);
uint8_t HD44780s_upload_symbol_flash(uint8_t symbol_number, const int8_t *symbol_array);
uint8_t HD44780s_write_string(uint8_t *str_output);
uint8_t HD44780s_write_flash_string(const int8_t *str_output);

#endif /* CONF_EN_HD44780 */

#endif /* DRV_HD44780_SERIAL_H_ */
