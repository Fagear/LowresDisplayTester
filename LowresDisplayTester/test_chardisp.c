#include "test_chardisp.h"
#include "cp1251.h"

#ifdef CONF_EN_CHARDISP
#include "flash_strings.h"

uint8_t chardisp_page_step = 0;
uint8_t chardisp_ani_step = 0;

#ifdef CHARDISP_FULL
//-------------------------------------- Animation order.
const int8_t* const ani_vert_fadeout[] PROGMEM =
{
	usr_char_lvl4,		// "High level" to "empty", step 1.
	usr_char_lvl5,
	usr_char_lvl6,
	usr_char_lvl7,
	usr_char_lvl8,
	usr_char_lvl9,
	usr_char_lvl10,
	usr_char_lvl11		// "High level" to "empty", step 8.
};

const int8_t* const ani_spiral[] PROGMEM =
{
	usr_char_load1,		// Rotating animation, step 1.
	usr_char_load2,
	usr_char_load3,
	usr_char_load4,
	usr_char_load5,
	usr_char_load6,
	usr_char_load7,
	usr_char_load8,
	usr_char_load9,
	usr_char_load10,
	usr_char_load11,
	usr_char_load12,
	usr_char_load13,
	usr_char_load14,
	usr_char_load15,
	usr_char_lvl18,
	usr_char_load17,
	usr_char_load18,
	usr_char_load19,
	usr_char_load20,
	usr_char_load21,
	usr_char_load22,
	usr_char_load23,
	usr_char_load24,
	usr_char_load25,
	usr_char_load26,
	usr_char_load5,		// Rotating animation, step 27.
	usr_char_spiral1,	// Spiral animation, step 1.
	usr_char_spiral2,
	usr_char_spiral3,
	usr_char_spiral4,
	usr_char_spiral5,
	usr_char_spiral6,
	usr_char_spiral7,
	usr_char_spiral8,
	usr_char_spiral9,
	usr_char_spiral10,
	usr_char_spiral11,
	usr_char_spiral12,
	usr_char_spiral13,
	usr_char_spiral14,
	usr_char_spiral15,
	usr_char_spiral16,
	usr_char_spiral17,
	usr_char_spiral18,
	usr_char_spiral19,
	usr_char_spiral20,
	usr_char_spiral21,
	usr_char_spiral22,
	usr_char_spiral23,
	usr_char_spiral24,
	usr_char_spiral25,
	usr_char_spiral26,
	usr_char_spiral27,
	usr_char_spiral28,
	usr_char_spiral29,
	usr_char_spiral30,
	usr_char_spiral31,
	usr_char_spiral32,
	usr_char_spiral33,
	usr_char_spiral34,
	usr_char_fill		// Spiral animation, step 35.
};

const int8_t* const ani_fadeout[] PROGMEM =
{
	usr_char_lvl12,		// Fadeout animation, step 1.
	usr_char_lvl13,
	usr_char_lvl14,
	usr_char_lvl15,
	usr_char_lvl16,
	usr_char_lvl17,
	usr_char_lvl18		// Fadeout animation, step 7.
};
#endif	/* CHARDISP_FULL */

//-------------------------------------- Functions' pointers ("API").
static uint8_t (*chardisp_upload_symbol_flash)(uint8_t symbol_number, const int8_t *symbol_array);
static uint8_t (*chardisp_set_xy_position)(uint8_t x_pos, uint8_t y_pos);
static uint8_t (*chardisp_write_char)(const uint8_t send_char);
static uint8_t (*chardisp_write_flash_string)(const int8_t *str_output, void *);

//-------------------------------------- Required function assignment before anything can be done.
void chardisp_set_device(void *f_upload, void *f_setpos, void *f_wr_char, void *f_wr_line)
{
	chardisp_upload_symbol_flash = (uint8_t (*)(uint8_t, const int8_t *))f_upload;
	chardisp_set_xy_position = (uint8_t (*)(uint8_t, uint8_t))f_setpos;
	chardisp_write_char = (uint8_t (*)(const uint8_t))f_wr_char;
	chardisp_write_flash_string = (uint8_t (*)(const int8_t *, void *))f_wr_line;
}

//-------------------------------------- Reset animation step.
void chardisp_reset_anim(void)
{
	chardisp_page_step = ST_TEXT_DET;
}

//-------------------------------------- Check if animation cycle is done.
uint8_t chardisp_cycle_done(void)
{
	if((chardisp_page_step==ST_TEXT_1x8)&&
		(chardisp_ani_step==0))
	{
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Fill display from current position +[count] with [symbol].
uint8_t chardisp_fill(uint8_t count, uint8_t symbol)
{
	uint8_t error_mask = 0;
	for(uint8_t idx=0;idx<count;idx++)
	{
		error_mask |= chardisp_write_char(symbol);
	}
	return error_mask;
}

//-------------------------------------- Clear whole screen.
uint8_t chardisp_clear(void)
{
	uint8_t error_mask = 0;
	// Clear screen.
	error_mask |= chardisp_set_xy_position(0, 0);
	error_mask |= chardisp_fill(40, CHAR_SPACE);
	error_mask |= chardisp_set_xy_position(0, 1);
	error_mask |= chardisp_fill(40, CHAR_SPACE);
	return error_mask;
}

#ifdef CHARDISP_FULL
//-------------------------------------- Draw spinning animation.
uint8_t chardisp_step_ani_rotate(uint8_t *err_mask)
{
	if(chardisp_ani_step==0)
	{
		// Load custom font.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_4, usr_char_rot1);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_5, usr_char_rot2);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_6, usr_char_rot3);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_7, usr_char_rot4);
	}
	else if(chardisp_ani_step<=100)
	{
		uint8_t frac, step;
		step = frac = (chardisp_ani_step-1);
		// Calculate rotation speed.
		frac = frac%4;
		// Calculate step of the animation.
		step = step/4;
		step = step%4;
		if(frac==0)
		{
			if(step==0)
			{
				// Rotate 0/90 phase, step 1.
				(*err_mask) |= chardisp_set_xy_position(0, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_4);
				(*err_mask) |= chardisp_set_xy_position(7, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_6);
			}
			else if(step==1)
			{
				// Rotate 45/135 phase, step 2.
				(*err_mask) |= chardisp_set_xy_position(0, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_5);
				(*err_mask) |= chardisp_set_xy_position(7, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_7);
			}
			else if(step==2)
			{
				// Rotate 90/180 phase, step 3.
				(*err_mask) |= chardisp_set_xy_position(0, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_6);
				(*err_mask) |= chardisp_set_xy_position(7, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_4);
			}
			else if(step==3)
			{
				// Rotate 135/225 phase, step 4.
				(*err_mask) |= chardisp_set_xy_position(0, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_7);
				(*err_mask) |= chardisp_set_xy_position(7, 0);
				(*err_mask) |= chardisp_write_char(C_CHAR_5);
			}
		}
	}
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step>150)
	{
		chardisp_ani_step = 0;
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Draw bar graph animation.
uint8_t chardisp_step_ani_levels(uint8_t *err_mask)
{
	uint8_t idx, xcoord;
	if(chardisp_ani_step==0)
	{
		// Clear screen.
		(*err_mask) |= chardisp_clear();
		// Load custom font.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl1);	// Horizontal "no level"
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl2);	// Horizontal "low level"
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_2, usr_char_lvl3);	// Horizontal "mid level"
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl4);	// Horizontal "high level"
	}
	else if((chardisp_ani_step>0)&&(chardisp_ani_step<=40))
	{
		// Fill lines with "no level" char.
		for(idx=0;idx<2;idx++)
		{
			(*err_mask) |= chardisp_set_xy_position((chardisp_ani_step-1), idx);
			(*err_mask) |= chardisp_write_char(C_CHAR_0);
		}
	}
	else if((chardisp_ani_step>=42)&&(chardisp_ani_step<=161))
	{
		// Gradually fill lines with "full level" chars.
		idx = (chardisp_ani_step-42)%3;
		xcoord = (chardisp_ani_step-42)/3;
		if(idx==0)
		{
			// Replace chars in the current column with horizontal "low level".
			for(idx=0;idx<2;idx++)
			{
				(*err_mask) |= chardisp_set_xy_position(xcoord, idx);
				(*err_mask) |= chardisp_write_char(C_CHAR_1);
			}
		}
		else if(idx==1)
		{
			// Replace chars in the current column with horizontal "mid level".
			for(idx=0;idx<2;idx++)
			{
				(*err_mask) |= chardisp_set_xy_position(xcoord, idx);
				(*err_mask) |= chardisp_write_char(C_CHAR_2);
			}
		}
		else if(idx==2)
		{
			// Replace chars in the current column with horizontal "high level".
			for(idx=0;idx<2;idx++)
			{
				(*err_mask) |= chardisp_set_xy_position(xcoord, idx);
				(*err_mask) |= chardisp_write_char(C_CHAR_3);
			}
		}
	}
	else if((chardisp_ani_step>=170)&&(chardisp_ani_step<=200)&&((chardisp_ani_step%5)==0))
	{
		uint8_t ani_idx;
		int8_t *ani_data;
		// Calculate index for animation.
		ani_idx = (chardisp_ani_step/5)-33;
		// Retrieve custom symbol from ROM.
		ani_data = (int8_t *)pgm_read_word(&ani_vert_fadeout[ani_idx]);
		// Perform whole screen animation, loading custom symbol by LUT.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_3, ani_data);
	}
	else if(chardisp_ani_step==205)
	{
		// Clear screen.
		(*err_mask) |= chardisp_clear();
	}
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step>240)
	{
		chardisp_ani_step = 0;
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Draw spiral animation.
uint8_t chardisp_step_ani_spiral(uint8_t *err_mask)
{
	if(((chardisp_ani_step%3)==0)&&(chardisp_ani_step<=183))
	{
		uint8_t ani_idx;
		int8_t *ani_data;
		// Calculate index for animation.
		ani_idx = chardisp_ani_step/3;
		// Retrieve custom symbol from ROM.
		ani_data = (int8_t *)pgm_read_word(&ani_spiral[ani_idx]);
		// Perform whole screen animation, loading custom symbol by LUT.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_0, ani_data);
	}
	if(chardisp_ani_step==0)
	{
		// Fill the screen.
		(*err_mask) |= chardisp_set_xy_position(0, 0);
		(*err_mask) |= chardisp_fill(40, C_CHAR_0);
		(*err_mask) |= chardisp_set_xy_position(0, 1);
		(*err_mask) |= chardisp_fill(40, C_CHAR_0);
	}
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step>250)
	{
		chardisp_ani_step = 0;
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Draw from top to bottom fade.
uint8_t chardisp_step_ani_fade(uint8_t *err_mask)
{
	if(chardisp_ani_step==0)
	{
		// Load new fill symbol.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_1, usr_char_fill);
		// Fill 2nd line with another custom symbol (with no visual difference, yet).
		(*err_mask) |= chardisp_set_xy_position(0, 1);
		(*err_mask) |= chardisp_fill(40, C_CHAR_1);
	}
	if((chardisp_ani_step<=30)&&((chardisp_ani_step%5)==0))
	{
		uint8_t ani_idx;
		int8_t *ani_data;
		// Calculate index for animation.
		ani_idx = chardisp_ani_step/5;
		// Retrieve custom symbol from ROM.
		ani_data = (int8_t *)pgm_read_word(&ani_fadeout[ani_idx]);
		// Perform first line animation, loading custom symbol by LUT.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_0, ani_data);
	}
	else if(chardisp_ani_step==35)
	{
		// Fadeout animation.
		// Fill first line with empty spaces.
		(*err_mask) |= chardisp_set_xy_position(0, 0);
		(*err_mask) |= chardisp_fill(40, CHAR_SPACE);
	}
	else if((chardisp_ani_step>=40)&&(chardisp_ani_step<=70)&&((chardisp_ani_step%5)==0))
	{
		uint8_t ani_idx;
		int8_t *ani_data;
		// Calculate index for animation.
		ani_idx = (chardisp_ani_step/5)-8;
		// Retrieve custom symbol from ROM.
		ani_data = (int8_t *)pgm_read_word(&ani_fadeout[ani_idx]);
		// Perform second line animation, loading custom symbol by LUT.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_1, ani_data);
	}
	else if(chardisp_ani_step==75)
	{
		// Fadeout animation.
		// Fill second line with empty spaces.
		(*err_mask) |= chardisp_set_xy_position(0, 1);
		(*err_mask) |= chardisp_fill(40, CHAR_SPACE);
	}
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step>110)
	{
		chardisp_ani_step = 0;
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}
#endif /* CHARDISP_FULL */

//-------------------------------------- Print out all symbols from codepage.
uint8_t chardisp_step_ani_cp_fill(uint8_t *err_mask)
{
	if(chardisp_ani_step==0)
	{
		// Load custom letters to print out.
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_0, usr_char_fgr1);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_1, usr_char_fgr2);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_2, usr_char_fgr3);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_3, usr_char_test1);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_4, usr_char_test2);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_5, usr_char_test3);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_6, usr_char_test1);
		(*err_mask) |= chardisp_upload_symbol_flash(C_CHAR_7, usr_char_lvl1);
	}
	if((chardisp_ani_step%16)==0)
	{
		if((chardisp_ani_step%32)==0)
		{
			// Set home position.
			(*err_mask) |= chardisp_set_xy_position(0, 0);
		}
		else
		{
			// Set home position.
			(*err_mask) |= chardisp_set_xy_position(0, 1);
		}
	}
	// Print next symbol.
	(*err_mask) |= chardisp_write_char(chardisp_ani_step);
	
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step==0)
	{
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Draw next frame of animation on HD44780-compatible display.
uint8_t chardisp_step_animation()
{
	uint8_t err_collector = 0;
	if(chardisp_page_step==ST_TEXT_DET)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(chardisp_det, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=25)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_1x8)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Clear screen.
			err_collector |= chardisp_clear();
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_1x8, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=50)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_1x16)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(12, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x16, NULL);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row2, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=50)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_1x20)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(16, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=50)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_1x24)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(20, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x24, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=50)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_1x40)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(36, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x40, NULL);
			err_collector |= chardisp_set_xy_position(6, 3);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row4, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=50)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if((chardisp_page_step==ST_TEXT_PAUSE)||
			(chardisp_page_step==ST_TEXT_PAUSE2))
	{
		chardisp_ani_step++;
		if(chardisp_ani_step>=100)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
		else if(chardisp_ani_step==50)
		{
			chardisp_clear();
		}
	}
	else if(chardisp_page_step==ST_TEXT_2x8)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Clear screen.
			err_collector |= chardisp_clear();
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_1x8, NULL);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row2_2, NULL);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=100)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_2x16)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Load custom symbols.
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_0, usr_char_triag1);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_1, usr_char_triag2);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_2, usr_char_triag3);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_3, usr_char_triag4);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_4, usr_char_fill);
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row1_2, NULL);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row2_2, NULL);
			err_collector |= chardisp_set_xy_position(12, 0);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x16, NULL);
			err_collector |= chardisp_set_xy_position(12, 1);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x16, NULL);
			err_collector |= chardisp_set_xy_position(7, 0);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(7, 1);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_2);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=100)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_2x20)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(6, 0);
			err_collector |= chardisp_fill(10, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
			err_collector |= chardisp_set_xy_position(6, 1);
			err_collector |= chardisp_fill(10, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
			err_collector |= chardisp_set_xy_position(9, 0);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(9, 1);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_2);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=100)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_2x24)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(9, 0);
			err_collector |= chardisp_fill(11, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x24, NULL);
			err_collector |= chardisp_set_xy_position(9, 1);
			err_collector |= chardisp_fill(11, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x24, NULL);
			err_collector |= chardisp_set_xy_position(11, 0);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(11, 1);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_2);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=100)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_4x20)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(7, 0);
			err_collector |= chardisp_fill(9, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
			err_collector |= chardisp_set_xy_position(7, 1);
			err_collector |= chardisp_fill(9, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
			err_collector |= chardisp_set_xy_position(0, 2);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row3, NULL);
			err_collector |= chardisp_fill(10, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
			err_collector |= chardisp_set_xy_position(0, 3);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_row4_2, NULL);
			err_collector |= chardisp_fill(10, CHAR_SPACE);
			err_collector |= chardisp_write_flash_string(chardisp_dsp_x20, NULL);
			err_collector |= chardisp_set_xy_position(8, 0);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(7, 1);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_2);
			err_collector |= chardisp_write_char(CHAR_SPACE);
			err_collector |= chardisp_write_char(CHAR_SPACE);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(7, 2);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_write_char(CHAR_SPACE);
			err_collector |= chardisp_write_char(CHAR_SPACE);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_2);
			err_collector |= chardisp_set_xy_position(8, 3);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_write_char(C_CHAR_2);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=250)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
#ifdef CHARDISP_FULL
	else if(chardisp_page_step==ST_ROTATE)
	{
		if(chardisp_step_ani_rotate(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
	else if(chardisp_page_step==ST_LEVELS)
	{
		if(chardisp_step_ani_levels(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
	else if(chardisp_page_step==ST_SPIRAL)
	{
		if(chardisp_step_ani_spiral(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
	else if(chardisp_page_step==ST_FADEOUT)
	{
		if(chardisp_step_ani_fade(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
#endif /* CHARDISP_FULL */
	else if(chardisp_page_step==ST_CP_FILL)
	{
		if(chardisp_step_ani_cp_fill(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
#ifdef CYR_TESTS
	else if(chardisp_page_step==ST_CYR_EN_LC)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Load custom font.
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_0, usr_char_v_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_1, usr_char_j_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_2, usr_char_l_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_3, usr_char_f_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_4, usr_char_yi_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_5, usr_char_e_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_6, usr_char_ju_lc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_7, usr_char_ja_lc);
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(cp1251_lc1, hd44780_cp1251toEN);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(cp1251_lc2, hd44780_cp1251toEN);
			// Substitute custom symbols.
			err_collector |= chardisp_set_xy_position(2, 0);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_set_xy_position(7, 0);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(12, 0);
			err_collector |= chardisp_write_char(C_CHAR_2);
			err_collector |= chardisp_set_xy_position(5, 1);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_set_xy_position(12, 1);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_set_xy_position(14, 1);
			err_collector |= chardisp_write_char(C_CHAR_5);
			err_collector |= chardisp_write_char(C_CHAR_6);
			err_collector |= chardisp_write_char(C_CHAR_7);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=150)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_CYR_EN_UC)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Load custom font.
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_0, usr_char_b_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_1, usr_char_g_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_2, usr_char_j_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_3, usr_char_f_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_4, usr_char_yi_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_5, usr_char_e_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_6, usr_char_ju_uc);
			err_collector |= chardisp_upload_symbol_flash(C_CHAR_7, usr_char_ja_uc);
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(cp1251_uc1, hd44780_cp1251toEN);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(cp1251_uc2, hd44780_cp1251toEN);
			// Substitute custom symbols.
			err_collector |= chardisp_set_xy_position(1, 0);
			err_collector |= chardisp_write_char(C_CHAR_0);
			err_collector |= chardisp_set_xy_position(3, 0);
			err_collector |= chardisp_write_char(C_CHAR_1);
			err_collector |= chardisp_set_xy_position(7, 0);
			err_collector |= chardisp_write_char(C_CHAR_2);
			err_collector |= chardisp_set_xy_position(5, 1);
			err_collector |= chardisp_write_char(C_CHAR_3);
			err_collector |= chardisp_set_xy_position(12, 1);
			err_collector |= chardisp_write_char(C_CHAR_4);
			err_collector |= chardisp_set_xy_position(14, 1);
			err_collector |= chardisp_write_char(C_CHAR_5);
			err_collector |= chardisp_write_char(C_CHAR_6);
			err_collector |= chardisp_write_char(C_CHAR_7);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=150)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_CYR_RU_LC)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(cp1251_lc1, hd44780_cp1251toRU);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(cp1251_lc2, hd44780_cp1251toRU);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=150)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_CYR_RU_UC)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(cp1251_uc1, hd44780_cp1251toRU);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(cp1251_uc2, hd44780_cp1251toRU);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=150)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_CYR_RU_SYM)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			// Draw the page.
			err_collector |= chardisp_set_xy_position(0, 0);
			err_collector |= chardisp_write_flash_string(cp1251_symbols1, hd44780_cp1251toRU);
			err_collector |= chardisp_set_xy_position(0, 1);
			err_collector |= chardisp_write_flash_string(cp1251_symbols2, hd44780_cp1251toRU);
		}
		chardisp_ani_step++;
		if(chardisp_ani_step>=150)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_PAUSE3)
	{
		chardisp_ani_step++;
		if(chardisp_ani_step>=100)
		{
			// Page already drawn and timer run out.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
#endif /* CYR_TESTS */
	else if(chardisp_page_step==ST_END_CLEAR)
	{
		// Clear display before restarting.
		chardisp_clear();
		// Go to the next page.
		chardisp_page_step++;
	}
	else
	{
		// Reset animation cycle.
		chardisp_page_step = ST_TEXT_1x8;
		chardisp_ani_step = 0;
	}
	// Check if display was lost.
	if(err_collector!=0)
	{
		// Cancel animations.
		chardisp_page_step = 0;
		chardisp_ani_step = 0;
	}
	return err_collector;
}
#endif /* CONF_EN_CHARDISP */
