#ifndef FR_DRV_HD44780_4BIT
#define FR_DRV_HD44780_4BIT		1

#include "drv_cpu.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// HD44780 display interface hardware.
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

#define HD44780_MAX_TRIES	250				// Maximum number of BUSY flag reads before aborting (~2 ms)

#define HD44780_MAX_COL		40				// Number of visible columns of the connected display
#define HD44780_MAX_ROWS	4				// Number of visible rows of the connected display
#define HD44780_TEST_CHAR1	0xA5			// Char for display RAM testing, pattern 1
#define HD44780_TEST_CHAR2	0x5A			// Char for display RAM testing, pattern 2

#define HD44780_USE_DIRECT_STRINGS			// Turn on direct strings writing to the display
//#define HD44780_USE_SCREEN_BUFFERS			// Turn on paged buffer

// MSBs for writing a command.
enum
{
	HD44780_CMD_CLR = (1<<0),		// "Clear display"
	HD44780_CMD_HOME = (1<<1),		// "Return home"
	HD44780_CMD_ENTRY = (1<<2),		// "Entry mode set"
	HD44780_CMD_DISP = (1<<3),		// "Display ON/OFF control"
	HD44780_CMD_SHIFT = (1<<4),		// "Cursor or display shift"
	HD44780_CMD_FUNC = (1<<5),		// "Function set"
	HD44780_CMD_SCGR = (1<<6),		// "Set Character Generator RAM address"
	HD44780_CMD_SDDR = (1<<7),		// "Set Display Data RAM address"
};

// Bits for "Entry mode set" command.
enum
{
	HD44780_ENTRY_RIGHT = (1<<1),	// Shift right on data access (shift left if not set)
	HD44780_ENTRY_SHDISP = (1<<0),	// Shift display on data access (shift cursor if not set)
};

// Bits for "Display ON/OFF control" command.
enum
{
	HD44780_DISP_SCREEN = (1<<2),	// Enable display output (off if not set)
	HD44780_DISP_CURSOR = (1<<1),	// Enable cursor drawing (no cursor if not set)
	HD44780_DISP_BLINK = (1<<0),	// Enable blinking in position of the cursor (static if not set)
};

// Bits for "Cursor or display shift" command.
enum
{
	HD44780_SHIFT_DISP = (1<<3),	// Force shifting display (shift cursor if not set)
	HD44780_SHIFT_RIGHT = (1<<2),	// Force shifting to the right (to the left if not set)
};

// Bits for "Function set" command.
enum
{
	HD44780_FUNC_8BIT = (1<<4),		// 8-bit data bus (4-bit if not set)
	HD44780_FUNC_2LINE = (1<<3),	// 2-line display access mode (1-line if not set)
	HD44780_FUNC_5X10 = (1<<2),		// 5x10 CG font selected (5x8 CG font if not set) (only 5x8 font when 2-line mode selected)
	HD44780_FUNC_CP = (1<<1),		// CP1251 Cyrillic character set
};

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

// Error codes.
enum
{
	HD44780_OK = 0,			// Everything went fine
	HD44780_ERR_BUSY,		// Error while working with the display (BUSY wait fail)
	HD44780_ERR_DATA,		// Error in input parameters of a function
	HD44780_ERR_BUS,		// Error while testing display data bus
};

// What to send to the display.
enum
{
	HD44780_CMD,			// Send a command to the display
	HD44780_DATA,			// Send a raw symbol to the display
	HD44780_DIGIT,			// Send a digit to the display (convert to char)
	HD44780_NONZERO_DIGIT,	// Send a digit to the display (convert to char, zeros to spaces)
	HD44780_NUMBER,			// Send a number to the display (convert to chars)
	HD44780_NUMBER_SPACES,	// Send a number to the display (convert to chars, no leading zeros)
};

// Special ASCII codes.
enum
{
	ASCII_SPACE = 0x20,		// Space character
	ASCII_ZERO = 0x30,		// 0-digit character (offset for conversion [uint8_t] to digit)
	ASCII_OVF = 0x23		// Filler for digit overflow (trying to print number >10 into one digit)
};

// Display pages in memory.
enum
{
	HD44780_PAGE0,
	HD44780_PAGE1
};

// Direction of animation.
enum
{
	FADE_RIGHT2LEFT = 1,	// Direction of pages changing: right to left
	FADE_LEFT2RIGHT = 2		// Direction of pages changing: left to right
};

enum
{
	HD44780_NO_UPDATE,		// Display doesn't need to be updated
	HD44780_NEEDS_UPDATE	// Screen need to be updated: buffer changed
};

uint8_t HD44780_setup(uint8_t disp_res, uint8_t cyr_conv);
uint8_t HD44780_init(void);
uint8_t HD44780_columns(void);
uint8_t HD44780_rows(void);
uint8_t HD44780_wait_ready(void);
uint8_t HD44780_write_byte(const uint8_t send_data, const uint8_t send_mode);
uint8_t HD44780_read_byte(uint8_t *read_result);
uint8_t HD44780_set_x_position(uint8_t x_pos);
uint8_t HD44780_set_xy_position(uint8_t x_pos, uint8_t y_pos);
uint8_t HD44780_selftest(void);
uint8_t HD44780_shorttest(void);
uint8_t HD44780_upload_symbol_flash(uint8_t symbol_number, const int8_t *symbol_array);
#ifdef HD44780_USE_DIRECT_STRINGS
	uint8_t HD44780_write_8bit_number(const uint8_t send_number, const uint8_t send_mode);
	uint8_t HD44780_write_string(const int8_t *str_output);
	uint8_t HD44780_write_flash_string(const int8_t *str_output);
	uint8_t HD44780_clear_line(const uint8_t line_idx);
#endif	// HD44780_USE_DIRECT_STRINGS
#ifdef HD44780_USE_SCREEN_BUFFERS
	void HD44780_swap_display_pages(uint8_t move_direction);
	void HD44780_select_display_page(uint8_t move_direction, uint8_t selector);
	uint8_t HD44780_select_fill_page(const uint8_t page_num);
	void HD44780_select_fill_shown(void);
	void HD44780_select_fill_hidden(void);
	void HD44780_buffer_update(void);
	uint8_t HD44780_buf_set_position(const uint8_t x_pos, const uint8_t y_pos);
	uint8_t HD44780_buf_write_byte(const uint8_t, const uint8_t);
	uint8_t HD44780_buf_write_8bit_number(const uint8_t, const uint8_t);
	uint8_t HD44780_buf_write_string(const int8_t *str_output);
	uint8_t HD44780_buf_write_flash_string(const int8_t *str_output);
	uint8_t HD44780_buf_clear_line(const uint8_t line_idx);
#endif	// HD44780_USE_SCREEN_BUFFERS


#endif // FR_DRV_HD44780_4BIT
