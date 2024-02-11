/**************************************************************************************************************************************************************
test_chardisp.h

Copyright � 2024 Maksim Kryukov <fagear@mail.ru>

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
#include "config.h"
#ifdef CONF_EN_CHARDISP
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

	enum
	{
		ST_TEXT_DET,
		ST_TEXT_1x8,
		ST_TEXT_1x16,
		ST_TEXT_1x20,
		ST_TEXT_1x24,
		ST_TEXT_1x40,
		ST_ROTATE,
		ST_TEXT_PAUSE,
		ST_TEXT_2x8,
		ST_TEXT_2x16,
		ST_TEXT_2x20,
		ST_TEXT_2x24,
		ST_TEXT_4x20,
		ST_LEVELS,
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
	uint8_t chardisp_step_ani_rotate(uint8_t *err_mask);
	uint8_t chardisp_step_ani_levels(uint8_t *err_mask);
	uint8_t chardisp_step_animation(uint8_t sec_tick);
#endif /* CONF_EN_CHARDISP */

#endif /* TEST_CHARDISP_H_ */