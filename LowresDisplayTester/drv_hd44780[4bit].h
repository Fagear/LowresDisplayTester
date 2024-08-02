/**************************************************************************************************************************************************************
drv_hd44780[4bit].h

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

Created: 2009-03

Part of the [LowresDisplayTester] project.
HD44780-compatible (HD44780/KS0066/КБ1013ВГ6/PT6314/SPLC780/US2066) character display driver
for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

TODO: cleaup this junk
This driver utilizes 4-bit data bus connection and BUSY-flag reading based operations.
Driver requires display connections to be not more than on two ports of the MCU,
3 pins for command lines and 4 pins for data lines. Single port connection is also possible.
See [HD44780CTRL_PORT], [HD44780DATA_PORT] defines.

Initialization routine [HD44780_init()] and self-test routine [HD44780_selftest()] are provided.
Single-byte operations are performed through [HD44780_write_byte()] and [HD44780_read_byte()].
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

Service functions for buffered access start from "HD44780_buf_" instead of "HD44780_".

**************************************************************************************************************************************************************/

#ifndef FR_DRV_HD44780_4BIT
#define FR_DRV_HD44780_4BIT		1

#include "config.h"
#include "drv_io.h"
#include "drv_hd44780[gen].h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#ifdef CONF_EN_HD44780P

#define FGR_DRV_HD44780P_FOUND

// HD44780 display interface hardware.
#ifndef HD44780_A0
	#pragma message("HD44780 driver is using internal defines for IO pins")
	#define HD44780CTRL_DIR		DDRD
	#define HD44780CTRL_PORT	PORTD
	#define HD44780DATA_DIR		DDRC
	#define HD44780DATA_PORT	PORTC
	#define HD44780DATA_SRC		PINC
	#define HD44780_A0			(1<<3)
	#define HD44780_RW			(1<<5)
	#define HD44780_E			(1<<7)
	#define HD44780_D4			(1<<0)
	#define HD44780_D5			(1<<1)
	#define HD44780_D6			(1<<2)
	#define HD44780_D7			(1<<3)
#endif /* HD44780_A0 */

#define HD44780_MAX_TRIES	80				// Maximum number of BUSY flag reads before aborting (~2 ms)

// Display resolutions.
enum
{
	HD44780_RES_8X1,
	HD44780_RES_16X1,
	HD44780_RES_20X1,
	HD44780_RES_24X1,
	HD44780_RES_40X1,
	HD44780_RES_8X2,
	HD44780_RES_16X2,
	HD44780_RES_20X2,
	HD44780_RES_24X2,
	HD44780_RES_40X2,
	HD44780_RES_20X4,
	HD44780_RES_LIM
};

// Flag for Cyrillic charset conversion.
enum
{
	HD44780_CYR_NOCONV,		// Display supports Cyrillic CP1251 codepage or non-Cyrillic codepage (no need for converting)
	HD44780_CYR_CONVERT,	// Default Cyrillic codepage, needs conversion from CP1251 while outputting
	HD44780_CYR_LIM
};

uint8_t HD44780_setup(uint8_t disp_res, uint8_t cyr_conv);
uint8_t HD44780_init(void);
uint8_t HD44780_raw_read(void);
uint8_t HD44780_wait_ready(uint8_t *disp_addr);
uint8_t HD44780_read_byte(uint8_t *read_result);
uint8_t HD44780_write_command_byte(const uint8_t send_data);
uint8_t HD44780_write_data_byte(const uint8_t send_data);

#endif /* CONF_EN_HD44780P */

#endif // FR_DRV_HD44780_4BIT
