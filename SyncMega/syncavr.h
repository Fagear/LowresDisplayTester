/**************************************************************************************************************************************************************
syncavr.h

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

Created: 2022-05

Part of the [LowresDisplayTester] project.

**************************************************************************************************************************************************************/

#ifndef SYNCAVR_H_
#define SYNCAVR_H_

#include <stdio.h>
#include "config.h"
//#include "drv_cpu.h"
#include "drv_io.h"
#ifdef CONF_EN_HD44780
	#include "drv_hd44780[4bit].h"
	#include "drv_hd44780[serial].h"
	#include "test_chardisp.h"
#endif /* CONF_EN_HD44780 */
#include "test_video.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>

// Configuration and user input.
#define SW_USR_1			(1<<0)
#define SW_VID_SYS0			(1<<1)
#define SW_VID_SYS1			(1<<2)

#define SPI_DUMMY_SEND		0xA5	// Dummy byte to send via SPI to generate bars

// Sync generation modes.
enum
{
	MODE_COMP_625i,					// Composite 625 lines @ 25Hz interlaced (PAL/SECAM)
	MODE_COMP_525i,					// Composite 525 lines @ 30Hz interlaced (NTSC)
	MODE_EGA,						// CGA/EGA 60Hz
	MODE_VGA_60Hz,					// VGA 640x480 @ 60Hz progressive
};

// States of active line timer.
enum
{
	LACT_ST_WAIT_ACT,				// Counting until start of the active part with OC at low
	LACT_ST_ACT,					// Counting until end of the active part with OC at high
	LACT_ST_IDLE,					// Not counting / waiting for new horizontal pulse
};

// Line active part flags.
enum
{
	ACT_RUN				= (1<<0),	// Line is now in active part
	ACT_MN_LINES		= (1<<1),	// Monochrome line generation is allowed
	ACT_RGB_LINES		= (1<<2),	// RGB lines generation is allowed
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

// System tasks.
enum
{
	TASK_UPDATE_ASYNC = (1<<0),
	TASK_SEC_TICK = (1<<1),
	TASK_I2C = (1<<2),
	TASK_I2C_SCAN = (1<<3),
	TASK_UART_RX = (1<<4),
	TASK_UART_TX = (1<<5),
};

void system_startup(void);
void keys_simple_scan(void);
uint8_t select_video_std(void);
int main(void);

#endif /* SYNCAVR_H_ */
