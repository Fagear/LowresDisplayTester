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
HD44780-compatible (HD44780/KS0066/КБ1013ВГ6/PT6314/SPLC780/US2066) character display serial driver
for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

TODO: cleaup this junk
Initialization routine [HD44780s_init()] is provided.
Single-byte operations are performed through [HD44780s_write_command_byte()] and [HD44780_read_byte()].
Driver supports Cyrillic displays on КБ1013ВГ6 controller with CP1251 codepage
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

**************************************************************************************************************************************************************/

#ifndef DRV_HD44780_SERIAL_H_
#define DRV_HD44780_SERIAL_H_

//#include "config.h"
#include "drv_io.h"
#include "drv_hd44780[gen].h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#ifdef CONF_EN_HD44780S

#define FGR_DRV_HD44780S_FOUND

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
	#define HD44780S_A0			(1<<0)
	#define HD44780S_RW			(1<<1)
	#define HD44780S_E			(1<<2)
	#define HD44780S_D4			(1<<4)
	#define HD44780S_D5			(1<<5)
	#define HD44780S_D6			(1<<6)
	#define HD44780S_D7			(1<<7)
	#define HD44780S_BL			(1<<3)
#endif /* HD44780_A0 */

void HD44780s_set_address(uint8_t addr);
uint8_t HD44780s_init(void);
uint8_t HD44780s_write_command_byte(const uint8_t send_data);
uint8_t HD44780s_write_data_byte(const uint8_t send_data);

#endif /* CONF_EN_HD44780S */

#endif /* DRV_HD44780_SERIAL_H_ */
