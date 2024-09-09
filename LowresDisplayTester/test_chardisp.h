/**************************************************************************************************************************************************************
test_chardisp.h

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
Character display test module.
Performs drawing strings and animations on a character display
via provided callback functions of the display driver.

Callbacks must be provided through [chardisp_set_device()] function
before animations can be stepped with [chardisp_step_animation()] function.

**************************************************************************************************************************************************************/

#ifndef TEST_CHARDISP_H_
#define TEST_CHARDISP_H_

#include <stdio.h>
#include "drv_io.h"

#ifdef CONF_EN_CHARDISP

	#ifdef FGR_DRV_HD44780P_FOUND
		enum
		{
			CHTST_RES_OK = HD44780_OK,
		};
	#elif defined FGR_DRV_HD44780S_FOUND
		enum
		{
			CHTST_RES_OK = HD44780_OK,
		};
	#else
		#error HD44780 driver not found!
	#endif

	// Limit number of animations for low-ROM devices
	#if FLASHEND>0x2000
		#define CHARDISP_FULL
	#endif
	
	// Enable tests for Cyrillic support.
	#define CYR_TESTS
	
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
		CHAR_SPACE = 0x20,	// ASCII space code
	};

	// Stages of test.
	enum
	{
		ST_TEXT_DET,		// Print out "Detected" message only after display first found
		ST_TEXT_1x8,		// Print dimensions of the display rows and columns
		ST_TEXT_1x16,
		ST_TEXT_1x20,
		ST_TEXT_1x24,
		ST_TEXT_1x40,
#ifdef CHARDISP_FULL
		ST_ROTATE,			// Display rotating animation on 8x1 brackets
#endif /* CHARDISP_FULL */
		ST_TEXT_PAUSE,
		ST_TEXT_2x8,		// Print NUT
		ST_TEXT_2x16,
		ST_TEXT_2x20,
		ST_TEXT_2x24,
		ST_TEXT_4x20,
#ifdef CHARDISP_FULL
		ST_LEVELS,			// Fill screen with bar graphs
		ST_SPIRAL,			// Draw circle and spiral animations
		ST_FADEOUT,			// Draw top to bottom fadeout animation
#endif /* CHARDISP_FULL */
		ST_CP_FILL,			// Print all symbols from codepage
		ST_TEXT_PAUSE2,
#ifdef CYR_TESTS
		ST_CYR_EN_LC,		// Cyrillic lower-case letters on regular display
		ST_CYR_RU_LC,		// Cyrillic lower-case letters on Cyrillic-enabled display
		ST_TEXT_PAUSE3,
#endif /* CYR_TESTS */
		ST_END_CLEAR,
		ST_TEXT_MAX
	};
	
	// Task states.
	enum
	{
		ST_ANI_DONE,
		ST_ANI_BUSY,
	};
	
	void chardisp_set_device(void *f_upload, void *f_setpos, void *f_wr_char, void *f_wr_line);
	void chardisp_reset_anim(void);
	uint8_t chardisp_cycle_done(void);
	uint8_t chardisp_fill(uint8_t count, uint8_t symbol);
	uint8_t chardisp_clear(void);
#ifdef CHARDISP_FULL
	uint8_t chardisp_step_ani_rotate(uint8_t *err_mask);
	uint8_t chardisp_step_ani_levels(uint8_t *err_mask);
	uint8_t chardisp_step_ani_spiral(uint8_t *err_mask);
	uint8_t chardisp_step_ani_fade(uint8_t *err_mask);
#endif /* CHARDISP_FULL */
	uint8_t chardisp_step_ani_cp_fill(uint8_t *err_mask);
	uint8_t chardisp_step_animation();
#endif /* CONF_EN_CHARDISP */

#endif /* TEST_CHARDISP_H_ */
