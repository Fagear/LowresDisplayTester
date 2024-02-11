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

//-------------------------------------- Clear whole screen.
uint8_t chardisp_clear(void)
{
	uint8_t error_mask = 0;
	// Clear screen.
	error_mask += chardisp_set_xy_position(0, 0);
	error_mask += chardisp_fill(40, CHAR_SPACE);
	error_mask += chardisp_set_xy_position(0, 1);
	error_mask += chardisp_fill(40, CHAR_SPACE);
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
		(*err_mask) += chardisp_clear();
		// Load custom font.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl1);	// Horizontal "no level"
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl2);	// Horizontal "low level"
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_2, usr_char_lvl3);	// Horizontal "mid level"
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl4);	// Horizontal "high level"
	}
	else if((chardisp_ani_step>0)&&(chardisp_ani_step<=40))
	{
		// Fill lines with "no level" char.
		for(idx=0;idx<2;idx++)
		{
			(*err_mask) += chardisp_set_xy_position((chardisp_ani_step-1), idx);
			(*err_mask) += chardisp_write_char(C_CHAR_0);
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
	else if(chardisp_ani_step==170)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 1.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl5);
	}
	else if(chardisp_ani_step==175)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 2.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl6);
	}
	else if(chardisp_ani_step==180)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 3.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl7);
	}
	else if(chardisp_ani_step==185)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 4.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl8);
	}
	else if(chardisp_ani_step==190)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 5.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl9);
	}
	else if(chardisp_ani_step==195)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 6.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl10);
	}
	else if(chardisp_ani_step==200)
	{
		// Transition from horizontal "high level" to "empty" on the whole display, step 7.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_lvl11);
	}
	else if(chardisp_ani_step==205)
	{
		// Clear screen.
		(*err_mask) += chardisp_clear();
	}
	/*else if(chardisp_ani_step==170)
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
	}*/
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
	if(chardisp_ani_step==0)
	{
		// Load custom font.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load1);
		// Fill screen with it.
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_fill(40, C_CHAR_0);
		(*err_mask) += chardisp_set_xy_position(0, 1);
		(*err_mask) += chardisp_fill(40, C_CHAR_0);
	}
	else if(chardisp_ani_step==3)
	{
		// Rotating animation, step 2.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load2);
	}
	else if(chardisp_ani_step==6)
	{
		// Rotating animation, step 3.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load3);
	}
	else if(chardisp_ani_step==9)
	{
		// Rotating animation, step 4.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load4);
	}
	else if(chardisp_ani_step==12)
	{
		// Rotating animation, step 5.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load5);
	}
	else if(chardisp_ani_step==15)
	{
		// Rotating animation, step 6.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load6);
	}
	else if(chardisp_ani_step==18)
	{
		// Rotating animation, step 7.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load7);
	}
	else if(chardisp_ani_step==21)
	{
		// Rotating animation, step 8.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load8);
	}
	else if(chardisp_ani_step==24)
	{
		// Rotating animation, step 9.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load9);
	}
	else if(chardisp_ani_step==27)
	{
		// Rotating animation, step 10.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load10);
	}
	else if(chardisp_ani_step==30)
	{
		// Rotating animation, step 11.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load11);
	}
	else if(chardisp_ani_step==33)
	{
		// Rotating animation, step 12.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load12);
	}
	else if(chardisp_ani_step==36)
	{
		// Rotating animation, step 13.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load13);
	}
	else if(chardisp_ani_step==39)
	{
		// Rotating animation, step 14.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load14);
	}
	else if(chardisp_ani_step==42)
	{
		// Rotating animation, step 15.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load15);
	}
	else if(chardisp_ani_step==45)
	{
		// Rotating animation, step 16.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load16);
	}
	else if(chardisp_ani_step==48)
	{
		// Rotating animation, step 17.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load17);
	}
	else if(chardisp_ani_step==51)
	{
		// Rotating animation, step 18.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load18);
	}
	else if(chardisp_ani_step==54)
	{
		// Rotating animation, step 19.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load19);
	}
	else if(chardisp_ani_step==57)
	{
		// Rotating animation, step 20.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load20);
	}
	else if(chardisp_ani_step==60)
	{
		// Rotating animation, step 21.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load21);
	}
	else if(chardisp_ani_step==63)
	{
		// Rotating animation, step 22.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load22);
	}
	else if(chardisp_ani_step==66)
	{
		// Rotating animation, step 23.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load23);
	}
	else if(chardisp_ani_step==69)
	{
		// Rotating animation, step 24.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load24);
	}
	else if(chardisp_ani_step==72)
	{
		// Rotating animation, step 25.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load25);
	}
	else if(chardisp_ani_step==75)
	{
		// Rotating animation, step 26.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load26);
	}
	else if(chardisp_ani_step==78)
	{
		// Rotating animation, step 27.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_load5);
	}
	else if(chardisp_ani_step==90)
	{
		// Spiral animation, step 1.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral1);
	}
	else if(chardisp_ani_step==93)
	{
		// Spiral animation, step 2.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral2);
	}
	else if(chardisp_ani_step==96)
	{
		// Spiral animation, step 3.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral3);
	}
	else if(chardisp_ani_step==99)
	{
		// Spiral animation, step 4.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral4);
	}
	else if(chardisp_ani_step==102)
	{
		// Spiral animation, step 5.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral5);
	}
	else if(chardisp_ani_step==105)
	{
		// Spiral animation, step 6.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral6);
	}
	else if(chardisp_ani_step==108)
	{
		// Spiral animation, step 7.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral7);
	}
	else if(chardisp_ani_step==111)
	{
		// Spiral animation, step 8.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral8);
	}
	else if(chardisp_ani_step==114)
	{
		// Spiral animation, step 9.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral9);
	}
	else if(chardisp_ani_step==117)
	{
		// Spiral animation, step 10.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral10);
	}
	else if(chardisp_ani_step==120)
	{
		// Spiral animation, step 11.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral11);
	}
	else if(chardisp_ani_step==123)
	{
		// Spiral animation, step 12.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral12);
	}
	else if(chardisp_ani_step==126)
	{
		// Spiral animation, step 13.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral13);
	}
	else if(chardisp_ani_step==129)
	{
		// Spiral animation, step 14.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral14);
	}
	else if(chardisp_ani_step==132)
	{
		// Spiral animation, step 15.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral15);
	}
	else if(chardisp_ani_step==135)
	{
		// Spiral animation, step 16.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral16);
	}
	else if(chardisp_ani_step==138)
	{
		// Spiral animation, step 17.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral17);
	}
	else if(chardisp_ani_step==141)
	{
		// Spiral animation, step 18.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral18);
	}
	else if(chardisp_ani_step==144)
	{
		// Spiral animation, step 19.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral19);
	}
	else if(chardisp_ani_step==147)
	{
		// Spiral animation, step 20.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral20);
	}
	else if(chardisp_ani_step==150)
	{
		// Spiral animation, step 21.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral21);
	}
	else if(chardisp_ani_step==153)
	{
		// Spiral animation, step 22.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral22);
	}
	else if(chardisp_ani_step==156)
	{
		// Spiral animation, step 23.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral23);
	}
	else if(chardisp_ani_step==159)
	{
		// Spiral animation, step 24.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral24);
	}
	else if(chardisp_ani_step==162)
	{
		// Spiral animation, step 25.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral25);
	}
	else if(chardisp_ani_step==165)
	{
		// Spiral animation, step 26.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral26);
	}
	else if(chardisp_ani_step==168)
	{
		// Spiral animation, step 27.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral27);
	}
	else if(chardisp_ani_step==171)
	{
		// Spiral animation, step 28.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral28);
	}
	else if(chardisp_ani_step==174)
	{
		// Spiral animation, step 29.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral29);
	}
	else if(chardisp_ani_step==177)
	{
		// Spiral animation, step 30.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral30);
	}
	else if(chardisp_ani_step==180)
	{
		// Spiral animation, step 31.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral31);
	}
	else if(chardisp_ani_step==183)
	{
		// Spiral animation, step 32.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral32);
	}
	else if(chardisp_ani_step==186)
	{
		// Spiral animation, step 33.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral33);
	}
	else if(chardisp_ani_step==189)
	{
		// Spiral animation, step 34.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_spiral34);
	}
	else if(chardisp_ani_step==192)
	{
		// Spiral animation, step 35.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_fill);
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
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_fill);
		// Fill 2nd line with another custom symbol (with no visual difference, yet).
		(*err_mask) += chardisp_set_xy_position(0, 1);
		(*err_mask) += chardisp_fill(40, C_CHAR_1);
		// Fadeout animation, step 1.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl12);
	}
	else if(chardisp_ani_step==5)
	{
		// Fadeout animation, step 2.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl13);
	}
	else if(chardisp_ani_step==10)
	{
		// Fadeout animation, step 3.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl14);
	}
	else if(chardisp_ani_step==15)
	{
		// Fadeout animation, step 4.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl15);
	}
	else if(chardisp_ani_step==20)
	{
		// Fadeout animation, step 5.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl16);
	}
	else if(chardisp_ani_step==25)
	{
		// Fadeout animation, step 6.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl17);
	}
	else if(chardisp_ani_step==30)
	{
		// Fadeout animation, step 7.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_lvl18);
	}
	else if(chardisp_ani_step==35)
	{
		// Fadeout animation, step 8.
		// Fill first line with empty spaces.
		(*err_mask) += chardisp_set_xy_position(0, 0);
		(*err_mask) += chardisp_fill(40, CHAR_SPACE);
	}
	else if(chardisp_ani_step==40)
	{
		// Fadeout animation, step 9.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl12);
	}
	else if(chardisp_ani_step==45)
	{
		// Fadeout animation, step 10.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl13);
	}
	else if(chardisp_ani_step==50)
	{
		// Fadeout animation, step 11.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl14);
	}
	else if(chardisp_ani_step==55)
	{
		// Fadeout animation, step 12.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl15);
	}
	else if(chardisp_ani_step==60)
	{
		// Fadeout animation, step 13.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl16);
	}
	else if(chardisp_ani_step==65)
	{
		// Fadeout animation, step 14.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl17);
	}
	else if(chardisp_ani_step==70)
	{
		// Fadeout animation, step 15.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_lvl18);
	}
	else if(chardisp_ani_step==75)
	{
		// Fadeout animation, step 16.
		// Fill second line with empty spaces.
		(*err_mask) += chardisp_set_xy_position(0, 1);
		(*err_mask) += chardisp_fill(40, CHAR_SPACE);
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

//-------------------------------------- Print out all symbols from codepage.
uint8_t chardisp_step_ani_cp_fill(uint8_t *err_mask)
{
	if(chardisp_ani_step==0)
	{
		// Load custom letters to print out.
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_0, usr_char_fgr1);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_1, usr_char_fgr2);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_2, usr_char_fgr3);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_3, usr_char_test1);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_4, usr_char_test2);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_5, usr_char_test3);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_6, usr_char_test1);
		(*err_mask) += chardisp_upload_symbol_flash(C_CHAR_7, usr_char_lvl1);
	}
	if((chardisp_ani_step%16)==0)
	{
		if((chardisp_ani_step%32)==0)
		{
			// Set home position.
			(*err_mask) += chardisp_set_xy_position(0, 0);
		}
		else
		{
			// Set home position.
			(*err_mask) += chardisp_set_xy_position(0, 1);
		}
	}
	// Print next symbol.
	(*err_mask) += chardisp_write_char(chardisp_ani_step);
	
	// Next step.
	chardisp_ani_step++;
	if(chardisp_ani_step==0)
	{
		return ST_ANI_DONE;
	}
	return ST_ANI_BUSY;
}

//-------------------------------------- Draw next frame of animation on HD44780-compatible display.
uint8_t chardisp_step_animation(uint8_t switch_tick)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
		{
			// Page already drawn and got one second tick.
			// Go to the next page.
			chardisp_page_step++;
			chardisp_ani_step = 0;
		}
	}
	else if((chardisp_page_step==ST_TEXT_PAUSE)||
			(chardisp_page_step==ST_TEXT_PAUSE2))
	{
		if(switch_tick!=0)
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
			err_collector += chardisp_clear();
			// Draw the page.
			err_collector += chardisp_set_xy_position(0, 0);
			err_collector += chardisp_write_flash_string(chardisp_dsp_1x8);
			err_collector += chardisp_set_xy_position(0, 1);
			err_collector += chardisp_write_flash_string(chardisp_dsp_row2_2);
		}
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
		else if(switch_tick!=0)
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
			if(switch_tick!=0)
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
	else if(chardisp_page_step==ST_CP_FILL)
	{
		if(chardisp_step_ani_cp_fill(&err_collector)==ST_ANI_DONE)
		{
			chardisp_page_step++;
		}
	}
	else if(chardisp_page_step==ST_END_CLEAR)
	{
		// Clear display before restarting.
		chardisp_clear();
		if(switch_tick!=0)
		{
			// Page already drawn and got one second tick.
			// Go to the next page.
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
