/*
 * test_chardisp.h
 *
 * Created: 01.02.2024 17:18:23
 *  Author: kryukov
 */ 


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
		CHAR_SPACE = 0x20,
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
	
	// Task states.
	enum
	{
		ST_ANI_DONE,
		ST_ANI_BUSY,
	};
	
	void chardisp_set_device(void *f_upload, void *f_setpos, void *f_wr_char, void *f_wr_line);
	uint8_t chardisp_step_ani_rotate(uint8_t *err_mask);
	uint8_t chardisp_step_ani_levels(uint8_t *err_mask);
	uint8_t chardisp_step_animation(uint8_t sec_tick);
#endif /* CONF_EN_CHARDISP */

#endif /* TEST_CHARDISP_H_ */