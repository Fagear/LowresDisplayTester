#include "test_chardisp.h"

#ifdef CONF_EN_CHARDISP
#include "flash_strings.h"

uint8_t chardisp_page_step = 0;
uint8_t chardisp_ani_step = 0;

// API requests:
// HD44780_upload_symbol_flash
// HD44780_set_xy_position
// HD44780_write_byte
// HD44780_write_flash_string

//-------------------------------------- Functions' pointers.
static uint8_t (*chardisp_upload_symbol_flash)(uint8_t symbol_number, const int8_t *symbol_array);
static uint8_t (*chardisp_set_xy_position)(uint8_t x_pos, uint8_t y_pos);
static uint8_t (*chardisp_write_char)(const uint8_t send_char);
static uint8_t (*chardisp_write_flash_string)(const int8_t *str_output);

//-------------------------------------- Required function assignment before anything can be done.
void chardisp_set_device(void *f_upload, void *f_setpos, void *f_wr_char, void *f_wr_line)
{
	chardisp_upload_symbol_flash = (uint8_t (*)(uint8_t, const int8_t *))f_upload;
	chardisp_set_xy_position = (uint8_t (*)(uint8_t, uint8_t))f_setpos;
	chardisp_write_char = (uint8_t (*)(const uint8_t))f_wr_char;
	chardisp_write_flash_string = (uint8_t (*)(const int8_t *))f_wr_line;
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
		error_mask += chardisp_write_char(symbol);
	}
	return error_mask;
}

//-------------------------------------- Draw spinning animation.
uint8_t chardisp_step_ani_rotate(uint8_t *err_mask)
{
	if(chardisp_ani_step==0)
	{
		// Load custom font.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_4, usr_char_rot1);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_5, usr_char_rot2);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_6, usr_char_rot3);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_7, usr_char_rot4);
	}
	else if((chardisp_ani_step==1)||(chardisp_ani_step==17)||(chardisp_ani_step==33))
	{
		// Rotate 0/90 phase, step 1.
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_4);
		(*err_mask) += chardisp_set_xy_position(7, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_6);
	}
	else if((chardisp_ani_step==5)||(chardisp_ani_step==21)||(chardisp_ani_step==37))
	{
		// Rotate 45/135 phase, step 2.
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_5);
		(*err_mask) += chardisp_set_xy_position(7, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_7);
	}
	else if((chardisp_ani_step==9)||(chardisp_ani_step==25)||(chardisp_ani_step==41))
	{
		// Rotate 90/180 phase, step 3.
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_6);
		(*err_mask) += chardisp_set_xy_position(7, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_4);
	}
	else if((chardisp_ani_step==13)||(chardisp_ani_step==29)||(chardisp_ani_step==45))
	{
		// Rotate 135/225 phase, step 4.
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_7);

		(*err_mask) += chardisp_set_xy_position(7, 0);
		(*err_mask) += chardisp_write_char(C_CHAR_5);
	}
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step>49)
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
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_fill(40, CHAR_SPACE);
		(*err_mask) += chardisp_set_xy_position(0, 1);
		(*err_mask) += chardisp_fill(40, CHAR_SPACE);
		// Load custom font.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl1);	// Horizontal "no level"
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl2);	// Horizontal "low level"
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_2, usr_char_lvl3);	// Horizontal "mid level"
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl4);	// Horizontal "high level"
	}
	else if((chardisp_ani_step>0)&&(chardisp_ani_step<=40))
	{
		// Fill next column with "no level" char.
		//for(idx=0;idx<4;idx++)
		for(idx=0;idx<2;idx++)
		{
			(*err_mask) += chardisp_set_xy_position((chardisp_ani_step-1), idx);
			(*err_mask) += chardisp_write_char(C_CHAR_0);
		}
	}
	else if(chardisp_ani_step==41)
	{
		// Load more of the font.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_4, usr_char_lvl33);	// Vertical "low level" for even rows
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_5, usr_char_lvl32);	// Vertical "mid level" for even rows
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_6, usr_char_lvl31);	// Vertical "high level" for even rows
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_7, usr_char_fill);	// Full fill for even rows.
	}
	else if((chardisp_ani_step>=42)&&(chardisp_ani_step<=161))
	{
		idx = (chardisp_ani_step-42)%3;
		xcoord = (chardisp_ani_step-42)/3;
		if(idx==0)
		{
			// Replace chars in the current column with horizontal "low level".
			for(idx=0;idx<2;idx++)
			{
				(*err_mask) += chardisp_set_xy_position(xcoord, idx);
				(*err_mask) += chardisp_write_char(C_CHAR_1);
			}
		}
		else if(idx==1)
		{
			// Replace chars in the current column with horizontal "mid level".
			for(idx=0;idx<2;idx++)
			{
				(*err_mask) += chardisp_set_xy_position(xcoord, idx);
				(*err_mask) += chardisp_write_char(C_CHAR_2);
			}
		}
		else if(idx==2)
		{
			// Replace chars in the current column with horizontal "high level".
			for(idx=0;idx<2;idx++)
			{
				(*err_mask) += chardisp_set_xy_position(xcoord, idx);
				(*err_mask) += chardisp_write_char(C_CHAR_3);
			}
		}
	}
	else if(chardisp_ani_step==162)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 1.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl5);
	}
	else if(chardisp_ani_step==164)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 2.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl6);
	}
	else if(chardisp_ani_step==166)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 3.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl7);
	}
	else if(chardisp_ani_step==168)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 4.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_fill);
	}
	else if(chardisp_ani_step==170)
	{
		// Load more of the font.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl23);	// Vertical "low level" for odd rows
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl22);	// Vertical "mid level" for odd rows
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_2, usr_char_lvl21);	// Vertical "high level" for odd rows
		// Replace even rows with another char that looks the same.
		for(idx=1;idx<2;idx+=2)
		{
			for(xcoord=0;xcoord<40;xcoord++)
			{
				(*err_mask) += chardisp_set_xy_position(xcoord, idx);
				(*err_mask) += chardisp_write_char(C_CHAR_7);
			}
		}
	}
	else if(chardisp_ani_step==171)
	{
		// Transition from "full fill" to horizontal "max level" on the whole display, step 1.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl18);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_7, usr_char_lvl28);
	}
	else if(chardisp_ani_step==173)
	{
		// Transition from "full fill" to horizontal "max level" on the whole display, step 2.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl19);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_7, usr_char_lvl29);
	}
	else if(chardisp_ani_step==175)
	{
		// Transition from "full fill" to horizontal "max level" on the whole display, step 3.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl20);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_7, usr_char_lvl30);
	}
	else if(chardisp_ani_step==177)
	{
		// Replace first row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 0);
			(*err_mask) += chardisp_write_char(C_CHAR_2);
		}
	}
	else if(chardisp_ani_step==179)
	{
		// Replace first row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 0);
			(*err_mask) += chardisp_write_char(C_CHAR_1);
		}
	}
	else if(chardisp_ani_step==181)
	{
		// Replace first row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 0);
			(*err_mask) += chardisp_write_char(C_CHAR_0);
		}
	}
	else if(chardisp_ani_step==183)
	{
		// Replace first row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 0);
			(*err_mask) += chardisp_write_char(CHAR_SPACE);
		}
	}
	else if(chardisp_ani_step==185)
	{
		// Replace second row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 1);
			(*err_mask) += chardisp_write_char(C_CHAR_6);
		}
	}
	else if(chardisp_ani_step==187)
	{
		// Replace second row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 1);
			(*err_mask) += chardisp_write_char(C_CHAR_5);
		}
	}
	else if(chardisp_ani_step==189)
	{
		// Replace second row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 1);
			(*err_mask) += chardisp_write_char(C_CHAR_4);
		}
	}
	else if(chardisp_ani_step==191)
	{
		// Replace second row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 1);
			(*err_mask) += chardisp_write_char(CHAR_SPACE);
		}
	}
	/*else if(chardisp_ani_step==193)
	{
		// Replace third row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 2);
			(*err_mask) += chardisp_write_char(C_CHAR_2);
		}
	}
	else if(chardisp_ani_step==195)
	{
		// Replace third row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 2);
			(*err_mask) += chardisp_write_char(C_CHAR_1);
		}
	}
	else if(chardisp_ani_step==197)
	{
		// Replace third row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 2);
			(*err_mask) += chardisp_write_char(C_CHAR_0);
		}
	}
	else if(chardisp_ani_step==199)
	{
		// Replace third row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 2);
			(*err_mask) += chardisp_write_char(CHAR_SPACE);
		}
	}
	else if(chardisp_ani_step==201)
	{
		// Replace forth row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 3);
			(*err_mask) += chardisp_write_char(C_CHAR_6);
		}
	}
	else if(chardisp_ani_step==203)
	{
		// Replace forth row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 3);
			(*err_mask) += chardisp_write_char(C_CHAR_5);
		}
	}
	else if(chardisp_ani_step==205)
	{
		// Replace forth row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 3);
			(*err_mask) += chardisp_write_char(C_CHAR_4);
		}
	}
	else if(chardisp_ani_step==207)
	{
		// Replace forth row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			(*err_mask) += chardisp_set_xy_position(xcoord, 3);
			(*err_mask) += chardisp_write_char(CHAR_SPACE);
		}
	}*/
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step>210)
	{
		chardisp_ani_step = 0;
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Draw next frame of animation on HD44780-compatible display.
uint8_t chardisp_step_animation(uint8_t sec_tick)
{
	uint8_t err_collector = 0;
	if(chardisp_page_step==ST_TEXT_DET)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(0, 0);
			err_collector += chardisp_write_flash_string(chardisp_det);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(0, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_1x8);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(12, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x16);
			err_collector += chardisp_set_xy_position(0, 1);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row2);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(16, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(20, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x24);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(36, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x40);
			err_collector += chardisp_set_xy_position(6, 3);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row4);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_TEXT_PAUSE)
	{
		if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
			// Go to the next page.
			chardisp_page_step++;
		}
	}
	else if(chardisp_page_step==ST_TEXT_2x8)
	{
		// Check if page was drawn.
		if(chardisp_ani_step==0)
		{
			// Not yet.
			chardisp_ani_step++;
			// Clear screen.
			err_collector += chardisp_set_xy_position(0, 0);
			err_collector += chardisp_fill(40, CHAR_SPACE);
			err_collector += chardisp_set_xy_position(0, 1);
			err_collector += chardisp_fill(40, CHAR_SPACE);
			// Draw the page.
			err_collector += chardisp_set_xy_position(0, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_1x8);
			err_collector += chardisp_set_xy_position(0, 1);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row2_2);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Load custom symbols.
			err_collector += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_triag1);
			err_collector += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_triag2);
			err_collector += chardisp_upload_symbol_flash(C_CHAR_2, usr_char_triag3);
			err_collector += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_triag4);
			err_collector += chardisp_upload_symbol_flash(C_CHAR_4, usr_char_fill);
			err_collector += chardisp_upload_symbol_flash(C_CHAR_5, usr_char_dot);
			// Draw the page.
			err_collector += chardisp_set_xy_position(0, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row1_2);
			err_collector += chardisp_set_xy_position(0, 1);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row2_2);
			err_collector += chardisp_set_xy_position(12, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x16);
			err_collector += chardisp_set_xy_position(12, 1);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x16);
			err_collector += chardisp_set_xy_position(7, 0);
			err_collector += chardisp_write_char(C_CHAR_0);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_1);
			err_collector += chardisp_set_xy_position(7, 1);
			err_collector += chardisp_write_char(C_CHAR_3);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_2);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(6, 0);
			err_collector += chardisp_fill(10, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
			err_collector += chardisp_set_xy_position(6, 1);
			err_collector += chardisp_fill(10, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
			err_collector += chardisp_set_xy_position(9, 0);
			err_collector += chardisp_write_char(C_CHAR_0);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_1);
			err_collector += chardisp_set_xy_position(9, 1);
			err_collector += chardisp_write_char(C_CHAR_3);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_2);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(9, 0);
			err_collector += chardisp_fill(11, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x24);
			err_collector += chardisp_set_xy_position(9, 1);
			err_collector += chardisp_fill(11, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x24);
			err_collector += chardisp_set_xy_position(11, 0);
			err_collector += chardisp_write_char(C_CHAR_0);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_1);
			err_collector += chardisp_set_xy_position(11, 1);
			err_collector += chardisp_write_char(C_CHAR_3);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_2);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
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
			chardisp_ani_step++;
			// Draw the page.
			err_collector += chardisp_set_xy_position(7, 0);
			err_collector += chardisp_fill(9, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
			err_collector += chardisp_set_xy_position(7, 1);
			err_collector += chardisp_fill(9, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
			err_collector += chardisp_set_xy_position(0, 2);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row3);
			err_collector += chardisp_fill(10, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
			err_collector += chardisp_set_xy_position(0, 3);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row4_2);
			err_collector += chardisp_fill(10, CHAR_SPACE);
			err_collector += chardisp_write_flash_string(chardisp_dsp_x20);
			err_collector += chardisp_set_xy_position(8, 0);
			err_collector += chardisp_write_char(C_CHAR_0);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_1);
			err_collector += chardisp_set_xy_position(7, 1);
			err_collector += chardisp_write_char(C_CHAR_0);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_2);
			err_collector += chardisp_write_char(CHAR_SPACE);
			err_collector += chardisp_write_char(CHAR_SPACE);
			err_collector += chardisp_write_char(C_CHAR_3);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_1);
			err_collector += chardisp_set_xy_position(7, 2);
			err_collector += chardisp_write_char(C_CHAR_3);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_1);
			err_collector += chardisp_write_char(CHAR_SPACE);
			err_collector += chardisp_write_char(CHAR_SPACE);
			err_collector += chardisp_write_char(C_CHAR_0);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_2);
			err_collector += chardisp_set_xy_position(8, 3);
			err_collector += chardisp_write_char(C_CHAR_3);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_4);
			err_collector += chardisp_write_char(C_CHAR_2);
		}
		else if(sec_tick!=0)
		{
			// Page already drawn and got one second tick.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if(chardisp_page_step==ST_ROTATE)
	{
		if(chardisp_step_ani_rotate(&err_collector)==ST_ANI_DONE)
		{
			if(sec_tick!=0)
			{
				chardisp_page_step++;
			}
		}
	}
	else if(chardisp_page_step==ST_LEVELS)
	{
		if(chardisp_step_ani_levels(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
	else
	{
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
