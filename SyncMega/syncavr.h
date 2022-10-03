/*
 * syncavr.h
 *
 * Created:			2022-05-17 15:28:20
 * Modified:		2022-05-20
 * Author:			Maksim Kryukov aka Fagear (fagear@mail.ru)
 *
 */ 

#ifndef SYNCAVR_H_
#define SYNCAVR_H_

#include <stdio.h>
#include "drv_cpu.h"
#include "drv_io.h"
#include "flash_strings.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>

// Configuration and user input.
#define SW_USR_1			(1<<0)
#define SW_VID_SYS0			(1<<1)
#define SW_VID_SYS1			(1<<2)

#define I2C_SSD1306_ADR1	0x3C
#define I2C_SSD1306_ADR2	0x3D

#define SPI_DUMMY_SEND		0xA5	// Dummy byte to send via SPI to generate bars

// Sync generation modes.
enum
{
	COMP_625i,					// Composite 625 lines @ 25Hz interlaced (PAL/SECAM)
	COMP_525i,					// Composite 525 lines @ 30Hz interlaced (NTSC)
	VGA_60Hz,					// VGA 640x480 @ 60Hz
};

// Horizontal sync timing constants.
enum
{
	COMP_LINE_LEN_525i = 1015,	// Line duration in field mode (normal line timing) for composite 525i
	COMP_HALF_LEN_525i = 507,	// Half-line duration in frame sync mode for composite 525i
	COMP_SYNC_V_LEN_525i = 432,	// Duration of negative pulse for composite vertical sync pulses
	COMP_SYNC_H_LEN_525i = 74,	// Duration of negative pulse for composite normal horizontal sync
	COMP_EQ_PULSE_LEN_525i = 36,// Duration of negative pulse for composite equalization pulses
	COMP_LINE_LEN_625i = 1023,	// Line duration in field mode (normal line timing) for composite 625i
	COMP_HALF_LEN_625i = 511,	// Half-line duration in frame sync mode for composite 625i
	COMP_SYNC_V_LEN_625i = 435,	// Duration of negative pulse for composite vertical sync pulses
	COMP_SYNC_H_LEN_625i = 75,	// Duration of negative pulse for composite normal horizontal sync
	COMP_EQ_PULSE_LEN_625i = 37,// Duration of negative pulse for composite equalization pulses
	COMP_ACT_DELAY = 150,		// Timer value for start of active part of the line for composite
	VGA_LINE_LEN = 507,			// Line duration for VGA 640x480
	VGA_SYNC_H_LEN = 60,		// Duration of negative pulse for VGA 640x480 vertical sync pulses
};

// Active line timing constants.
enum
{
	LINE_525i_ACT = 101,
	LINE_625i_ACT = 102,
	LINE_VGA_ACT = 50,
};

// Composite video 625i (PAL/SECAM) step count constants.
enum
{
	ST_COMP625_F1_VS_START = 0,		// 625i, field 1, end of pre-EQ pulses, start of vertical sync
	ST_COMP625_F1_VS_STOP = 4,		// 625i, field 1, end of vertical sync, start of post-EQ pulses
	ST_COMP625_F1_EQ_STOP = 9,		// 625i, field 1, end of post-EQ pulses, start of normal video lines
	ST_COMP625_F1_EQ_START = 314,	// 625i, field 1, end of normal video lines, start of pre-EQ pulses
	ST_COMP625_F2_VS_START = 319,	// 625i, field 2, end of pre-EQ pulses, start of vertical sync
	ST_COMP625_F2_VS_STOP = 324,	// 625i, field 2, end of vertical sync, start of post-EQ pulses
	ST_COMP625_F2_EQ_STOP = 329,	// 625i, field 2, end of post-EQ pulses, start of normal video lines
	ST_COMP625_F2_EQ_START = 634,	// 625i, field 2, end of normal video lines, start of pre-EQ pulses
	ST_COMP625_LOOP = 639,			// Maximum number of steps for 625i loop
};

// Composite video 625i (PAL/SECAM) line count constants.
enum
{
	ST_COMP625_F1_ACT_START = 24,	// 625i, field 1, start of active region
	ST_COMP625_F1_ACT_STOP = 309,	// 625i, field 1, end of active region
	ST_COMP625_F2_ACT_START = 336,	// 625i, field 2, start of active region
	ST_COMP625_F2_ACT_STOP = 622,	// 625i, field 2, end of active region
};

// Composite video 525i (NTSC) step count constants.
enum
{
	ST_COMP525_F1_EQ_START = 0,		// 525i, field 1, start of pre-EQ pulses
	ST_COMP525_F1_VS_START = 5,		// 525i, field 1, end of pre-EQ pulses, start of vertical sync
	ST_COMP525_F1_VS_STOP = 11,		// 525i, field 1, end of vertical sync, start of post-EQ pulses
	ST_COMP525_F1_EQ_STOP = 17,		// 525i, field 1, end of post-EQ pulses, start of normal video lines
	ST_COMP525_F2_EQ_START = 271,	// 525i, field 2, end of normal video lines, start of pre-EQ pulses
	ST_COMP525_F2_VS_START = 277,	// 525i, field 2, end of pre-EQ pulses, start of vertical sync
	ST_COMP525_F2_VS_STOP = 283,	// 525i, field 2, end of vertical sync, start of post-EQ pulses
	ST_COMP525_F2_EQ_STOP = 289,	// 525i, field 2, end of post-EQ pulses, start of normal video lines
	ST_COMP525_LOOP = 542,			// Maximum number of steps for 525i loop
};

// Composite video 525i (NTSC) line count constants.
enum
{
	ST_COMP525_F1_ACT_START = 19,	// 525i, field 1, start of active region
	ST_COMP525_F1_ACT_STOP = 261,	// 525i, field 1, end of active region
	ST_COMP525_F2_ACT_START = 284,	// 525i, field 2, start of active region
	ST_COMP525_F2_ACT_STOP = 524,	// 525i, field 2, end of active region
};

// Lines per frame count constants.
enum
{
	LINES_625 = 625,				// Number of video lines for composite 625i frame
	LINES_525 = 525,				// Number of video lines for composite 525i frame
	LINES_VGA = 525,				// Number of video lines for VGA 640x480
};

// Frequencies of generated vertical bars.
enum
{
	BAR_FREQ_500Hz = 500000,
	BAR_FREQ_1MHz = 1000000,
	BAR_FREQ_2MHz = 2000000,
	BAR_FREQ_4MHz = 4000000,
	BAR_FREQ_8MHz = 8000000,
};

enum
{
	TASK_UPDATE_ASYNC = (1<<0),
	TASK_SEC_TICK = (1<<1),
};

enum
{
	HW_DISP_44780 = (1<<0),			// Presence of HD44780-compatible display
};

// Custom character indexes.
enum
{
	C_CHAR_0,
	C_CHAR_1,
	C_CHAR_2,
	C_CHAR_3,
	C_CHAR_4,
	C_CHAR_5,
	C_CHAR_6,
	C_CHAR_7,
};

enum
{
	ST_TEXT_0,
	ST_TEXT_1,
	ST_TEXT_2,
	ST_TEXT_3,
	ST_TEXT_4,
	ST_TEXT_5,
	ST_TEXT_6,
	ST_TEXT_7,
	ST_TEXT_8,
	ST_TEXT_MAX
};

void system_startup(void);
void restart_composite(void);
void restart_hd44780(void);
uint8_t step_hd44780_ani_rotate(void);
uint8_t step_hd44780_ani_levels(void);
void step_hd44780_animation(void);
void keys_simple_scan(void);
int main();

#endif /* SYNCAVR_H_ */
