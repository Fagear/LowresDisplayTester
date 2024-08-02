#include "drv_hd44780[gen].h"

#ifdef HD44780_RU_REENCODE
// Char set conversion table for displays with Cyrillic letters but without CP1251 support (like МЭЛТ displays has).
const uint8_t hd44780_cp1251toRU_lut[128] PROGMEM =
{
//	0x_0, 0x_1, 0x_2, 0x_3, 0x_4, 0x_5, 0x_6, 0x_7, 0x_8, 0x_9, 0x_A, 0x_B, 0x_C, 0x_D, 0x_E, 0x_F
	0xAD, 0xA1, 0x2C, 0xB4, 0xCA, 0x2E, 0xFB, 0xFB, 0xFF, 0xD4, 0xA7, 0x3C, 0x48, 0x4B, 0xAD, 0x56,		// 0x8_
	0x68, 0x60, 0xE7, 0xCB, 0xCB, 0xDF, 0x2D, 0x2D, 0x20, 0xEE, 0xBB, 0x3E, 0xBD, 0xBA, 0x68, 0x76,		// 0x9_
	0x20, 0xA9, 0x79, 0x4A, 0xEF, 0xA1, 0x3A, 0xFD, 0xA2, 0xEE, 0xAF, 0xC8, 0xE9, 0x2D, 0xEE, 0x49,		// 0xA_
	0xEF, 0xD2, 0x49, 0x69, 0xB4, 0x75, 0xFE, 0x2D, 0xB5, 0xCC, 0xC5, 0xC9, 0x6A, 0x53, 0x73, 0x69,		// 0xB_
	0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 	// 0xC_
	0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1,		// 0xD_
	0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, 	// 0xE_
	0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7		// 0xF_
};

const uint8_t hd44780_cp1251toEN_lut[128] PROGMEM =
{
//	0x_0, 0x_1, 0x_2, 0x_3, 0x_4, 0x_5, 0x_6, 0x_7, 0x_8, 0x_9, 0x_A, 0x_B, 0x_C, 0x_D, 0x_E, 0x_F
	0x23, 0x72, 0x2C, 0x72, 0x22, 0x2E, 0x74, 0x74, 0x23, 0x25, 0x23, 0x3C, 0x23, 0x4B, 0x68, 0x56,		// 0x8_
	0x68, 0x60, 0x60, 0x22, 0x22, 0x2A, 0x2D, 0x2D, 0x20, 0x22, 0x22, 0x3E, 0x48, 0x6B, 0x68, 0x76,		// 0x9_
	0x20, 0x79, 0x79, 0x4A, 0x6F, 0x72, 0x3A, 0x24, 0x45, 0x63, 0x45, 0x3C, 0x2D, 0x2D, 0x72, 0x49,		// 0xA_
	0x6F, 0x5E, 0x49, 0x69, 0x72, 0x75, 0x20, 0x2D, 0x65, 0x23, 0x65, 0x3E, 0x6A, 0x53, 0x73, 0x69,		// 0xB_
	0x41, 0x36, 0x42, 0x72, 0x44, 0x45, 0x4A, 0x33, 0x4E, 0x4E, 0x4B, 0x4C, 0x4D, 0x48, 0x4F, 0x6E, 	// 0xC_
	0x50, 0x43, 0x54, 0x79, 0x46, 0x58, 0x55, 0x34, 0x57, 0x57, 0x62, 0x47, 0x62, 0x45, 0x55, 0x5A,		// 0xD_
	0x61, 0x36, 0x76, 0x72, 0x64, 0x65, 0x6A, 0x33, 0x75, 0x75, 0x6B, 0x6E, 0x6D, 0x48, 0x6F, 0x6E, 	// 0xE_
	0x70, 0x63, 0x54, 0x79, 0x66, 0x78, 0x75, 0x34, 0x77, 0x77, 0x62, 0x67, 0x62, 0x33, 0x75, 0x7A		// 0xF_
};
#endif /* HD44780_RU_REENCODE */


//-------------------------------------- Functions' pointers ("API").
static uint8_t (*hd44780_wr_cmd)(const uint8_t send_data);
static uint8_t (*hd44780_wr_data)(const uint8_t send_data);
static uint8_t (*hd44780_rd_data)(uint8_t *read_result);

//-------------------------------------- Required function assignment before anything can be done.
void hd44780_set_device(void *f_wr_cmd, void *f_wr_data, void *f_rd_data)
{
	hd44780_wr_cmd = (uint8_t (*)(const uint8_t))f_wr_cmd;
	hd44780_wr_data = (uint8_t (*)(const uint8_t))f_wr_data;
	hd44780_rd_data = (uint8_t (*)(uint8_t *))f_rd_data;
}

//-------------------------------------- Upload user-defined symbol.
uint8_t hd44780_upload_symbol_flash(uint8_t symbol_number, const int8_t *symbol_array)
{
	uint8_t idx, err_count = 0;
	// Mask out character selector above 7 (normally 0...7 are configurable).
	symbol_number &= 0x07;
	// Move DDRAM bits 3 bits left for CGRAM bits 5...3.
	symbol_number = (symbol_number<<3);
	// Add "Set CGRAM address" command bit.
	symbol_number |= HD44780_CMD_SCGR;
	// Send CGRAM address command.
	err_count |= hd44780_wr_cmd(symbol_number);
	if(err_count!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Cycle through rows of the CG matrix.
	for(idx=0;idx<8;idx++)
	{
		// Write byte.
		err_count |= hd44780_wr_data(pgm_read_byte_near(symbol_array+idx));
	}
	if(err_count==0)
	{
		return HD44780_OK;
	}
	else
	{
		return HD44780_ERR_BUSY;
	}
}

//-------------------------------------- Set 2+ lines display cursor position.
uint8_t hd44780_set_xy_position(uint8_t x_pos, uint8_t y_pos)
{
	// Limit address to max 40 chars per line.
	if(x_pos>HD44780_MAX_COL)
	{
		x_pos = HD44780_MAX_COL;
	}
	// Calculate address.
	if(y_pos==1)
	{
		// Second line starts at AC 0x40.
		x_pos += 0x40;
	}
	else if(y_pos==2)
	{
		// Further limit address to max 40 chars per line.
		if(x_pos>HD44780_MAX_COL)
		{
			x_pos = HD44780_MAX_COL;
		}
		// Third line is a part of the logical 1st line in the display,
		// starting at 0x14.
		x_pos += 0x14;
	}
	else if(y_pos==3)
	{
		// Further limit address to max 20 chars per line.
		if(x_pos>0x13)
		{
			x_pos = 0x13;
		}
		// Fourth line is a part of the logical 2nd line in the display,
		// starting at 0x54.
		x_pos += 0x54;
	}
	// Set DDRAM address command.
	x_pos |= HD44780_CMD_SDDR;
	// Set position.
	return hd44780_wr_cmd(x_pos);
}

//-------------------------------------- Set 1 line display cursor position.
uint8_t hd44780_set_x_position(uint8_t x_pos)
{
	// Limit address to max 80 chars in line.
	if(x_pos>HD44780_MAX_DDRAM)
	{
		x_pos = HD44780_MAX_DDRAM;
	}
	// Set DDRAM address command.
	x_pos |= HD44780_CMD_SDDR;
	// Set position.
	return hd44780_wr_cmd(x_pos);
}

//-------------------------------------- Fill selected line with spaces.
uint8_t hd44780_clear_line(const uint8_t line_idx)
{
	uint8_t idx, disp_status = HD44780_OK;
	if(line_idx>HD44780_MAX_ROWS)
	{
		return HD44780_ERR_DATA;
	}
	hd44780_set_xy_position(0, line_idx);
	for(idx=0; idx<=HD44780_MAX_COL; idx++)
	{
		disp_status = hd44780_wr_data(ASCII_SPACE);
		if(disp_status!=HD44780_OK)
		{
			break;
		}
	}
	return disp_status;
}

//-------------------------------------- Convert digit symbol to raw data for display.
uint8_t hd44780_digit_to_data(const uint8_t send_number, const uint8_t send_mode)
{
	uint8_t disp_data = ASCII_SPACE;
	if(send_mode==HD44780_NONZERO_DIGIT)
	{
		// Draw a digit, but if it's zero - draw a space.
		if(send_number==0)
		{
			// Draw ASCII space char in place of zero.
			disp_data = ASCII_SPACE;
		}
		else
		{
			// Non-zero symbols.
			if(send_number>9)
			{
				// It's not a single digit! Draw a ASCII overflow filler.
				disp_data = ASCII_OVF;
			}
			else
			{
				// Convert from digit binary to ASCII code for the digit.
				disp_data = send_number + ASCII_ZERO;
			}
		}
	}
	else if(send_mode==HD44780_DIGIT)
	{
		// Draw a digit (0...9).
		if(send_number>9)
		{
			// It's not a single digit! Draw a hash.
			disp_data = ASCII_OVF;
		}
		else
		{
			// Convert from digit binary to ASCII code for the digit.
			disp_data = send_number + ASCII_ZERO;
		}
	}
	else if(send_mode==HD44780_HEX_DIGIT)
	{
		if(send_number>=0x10)
		{
			// It's not a single digit! Draw a hash.
			disp_data = ASCII_OVF;
		}
		else if(send_number>9)
		{
			// Shift value to ASCII 'A'...'F' symbol.
			disp_data = send_number + ASCII_A;
		}
		else
		{
			// Convert from digit binary to ASCII code for the digit.
			disp_data = send_number + ASCII_ZERO;
		}
	}
	return disp_data;
}

//-------------------------------------- Write [uint8_t] as number on display.
uint8_t hd44780_write_8bit_number(const uint8_t send_number, const uint8_t send_mode)
{
	uint8_t err_count = 0;
	// Convert and output.
	if(send_mode==HD44780_NUMBER_SPACES)
	{
		// Space-padded right-aligned 3-char number.
		err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number/100, HD44780_NONZERO_DIGIT));
		if(send_number<100)
		{
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%100/10, HD44780_NONZERO_DIGIT));
		}
		else
		{
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%100/10, HD44780_DIGIT));
		}
		err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%10, HD44780_DIGIT));
	}
	else if(send_mode==HD44780_NUMBER)
	{
		// Non-padded left-aligned number.
		if(send_number>=100)
		{
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number/100, HD44780_DIGIT));
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%100/10, HD44780_DIGIT));
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%10, HD44780_DIGIT));
		}
		else if(send_number>=10)
		{
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%100/10, HD44780_DIGIT));
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%10, HD44780_DIGIT));
		}
		else
		{
			err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%10, HD44780_DIGIT));
		}
	}
	else if(send_mode==HD44780_NUMBER_HEX)
	{
		// Two-char HEX number.
		err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number/0x10, HD44780_HEX_DIGIT));
		err_count |= hd44780_wr_data(hd44780_digit_to_data(send_number%0x10, HD44780_HEX_DIGIT));
	}
	// Check for errors.
	if(err_count==0)
		return HD44780_OK;
	else
		return HD44780_ERR_BUSY;
}

#ifdef HD44780_RU_REENCODE
//-------------------------------------- Conversion from CP1251 codepage to codes for display with Cyrillic support.
uint8_t hd44780_cp1251toRU(uint8_t in_char)
{
	uint8_t out_char;
	if(in_char<=127)
	{
		out_char = in_char;
	}
	else
	{
		in_char = in_char - 128;
		out_char = pgm_read_byte_near(hd44780_cp1251toRU_lut+in_char);
	}
	return out_char;
}

//-------------------------------------- Conversion (approximation) from CP1251 codepage to codes for display with NO Cyrillic support.
uint8_t hd44780_cp1251toEN(uint8_t in_char)
{
	uint8_t out_char;
	if(in_char<=127)
	{
		out_char = in_char;
	}
	else
	{
		in_char = in_char - 128;
		out_char = pgm_read_byte_near(hd44780_cp1251toEN_lut+in_char);
	}
	return out_char;
}
#endif /* HD44780_RU_REENCODE */

uint8_t (*cp_conv_func)(uint8_t in_char);

//-------------------------------------- Write ASCII string from FLASH to display.
uint8_t hd44780_write_flash_string(const int8_t *str_output, void *f_conv)
{
	uint8_t send_char, char_cnt, disp_status = HD44780_OK;
	// Keep first char.
	send_char = pgm_read_byte_near(str_output);
	char_cnt = 0;
	// Cycle while symbols left.
	while(send_char!=0x00)
	{
		// If previous sending failed - skip data transfer.
		if(disp_status==HD44780_OK)
		{
			// Convert codepage if required.
			if(f_conv != NULL)
			{
				cp_conv_func = (uint8_t (*)(uint8_t))f_conv;
				send_char = cp_conv_func(send_char);
			}
			// Send char to display.
			disp_status = hd44780_wr_data(send_char);
		}
		else
		{
			break;
		}
		// Next char.
		char_cnt++;
		if(char_cnt>HD44780_MAX_COL)
		{
			return HD44780_ERR_DATA;
		}
		// Go to the next byte in the memory.
		send_char = pgm_read_byte_near(str_output+char_cnt);
	}
	if(disp_status==HD44780_OK)
		return HD44780_OK;
	else
		return HD44780_ERR_BUSY;
}

//-------------------------------------- Write ASCII string from RAM to display.
uint8_t hd44780_write_string(uint8_t *str_output, void *f_conv)
{
	uint8_t send_char, char_cnt, disp_status = HD44780_OK;
	// Keep first char.
	send_char = *(str_output);
	char_cnt = 0;
	// Cycle while symbols left.
	while(send_char!=0x00)
	{
		// If previous sending failed - skip data transfer.
		if(disp_status==HD44780_OK)
		{
			// Convert codepage if required.
			if(f_conv != NULL)
			{
				cp_conv_func = (uint8_t (*)(uint8_t))f_conv;
				send_char = cp_conv_func(send_char);
			}
			// Send char to display.
			disp_status = hd44780_wr_data(send_char);
		}
		else
		{
			break;
		}
		// Next char.
		char_cnt++;
		if(char_cnt>HD44780_MAX_COL)
		{
			return HD44780_ERR_DATA;
		}
		// Go to the next byte in the memory.
		send_char = *(str_output+char_cnt);
	}
	if(disp_status==HD44780_OK)
		return HD44780_OK;
	else
		return HD44780_ERR_BUSY;
}

//-------------------------------------- Read from DDRAM until stable results.
// Some displays are fast (instant ready), but lag in updating DRAM anyway.
// (US2066 OLEDs, PT6314 VFDs, ahem)
uint8_t hd44780_ddram_read(uint8_t x_pos, uint8_t *read_result)
{
	uint8_t error_collector = 0, read_tries, arr_idx;
	uint8_t read_data[3];
	read_tries = 8;
	arr_idx = 0;
	while(read_tries>0)
	{
		// Read data at position in DDRAM.
		error_collector |= hd44780_set_x_position(x_pos);
		error_collector |= hd44780_rd_data(&read_data[arr_idx]);
		// Sliding window for last 3.
		if(arr_idx<2)
		{
			arr_idx++;
		}
		else
		{
			// Check if 3 of 3 are equal.
			if((error_collector==0)&&
				(read_data[0]==read_data[1])&&
				(read_data[1]==read_data[2]))
			{
				// Data read successfully.
				(*read_result) = read_data[0];
				return HD44780_OK;
			}
			// Slide data back.
			read_data[0] = read_data[1];
			read_data[1] = read_data[2];
		}
		read_tries--;
	}
	if(error_collector==0)
	{
		return HD44780_ERR_BUS;
	}
	else
	{
		return HD44780_ERR_BUSY;
	}
}

//-------------------------------------- Read from CGRAM until stable results.
// Some displays are fast (instant ready), but lag in updating DRAM anyway.
// (US2066 OLEDs, PT6314 VFDs, ahem)
uint8_t hd44780_cgram_read(uint8_t char_idx, uint8_t *read_result)
{
	uint8_t error_collector = 0, read_tries, arr_idx;
	uint8_t read_data[3];
	read_tries = 8;
	arr_idx = 0;
	while(read_tries>0)
	{
		// Read data at position in CGRAM.
		error_collector |= hd44780_wr_cmd(HD44780_CMD_SCGR|char_idx);
		error_collector |= hd44780_rd_data(&read_data[arr_idx]);
		// Sliding window for last 3.
		if(arr_idx<2)
		{
			arr_idx++;
		}
		else
		{
			// Check if 3 of 3 are equal.
			if((error_collector==0)&&
				(read_data[0]==read_data[1])&&
				(read_data[1]==read_data[2]))
			{
				// Data read successfully.
				(*read_result) = read_data[0];
				return HD44780_OK;
			}
			// Slide data back.
			read_data[0] = read_data[1];
			read_data[1] = read_data[2];
		}
		read_tries--;
	}
	if(error_collector==0)
	{
		return HD44780_ERR_BUS;
	}
	else
	{
		return error_collector;
	}
}

//-------------------------------------- Perform fast display control and data connections test.
uint8_t hd44780_selftest(void)
{
	uint8_t error_collector = 0, saved_char = ASCII_SPACE, saved_line, check_byte;
	// Save symbol to variable.
	error_collector |= hd44780_ddram_read(HD44780_TEST_ADDR, &saved_char);
	// Send test symbol 1.
	error_collector |= hd44780_set_x_position(HD44780_TEST_ADDR);
	error_collector |= hd44780_wr_data(HD44780_TEST_CHAR1);
	// Read symbol back.
	error_collector = hd44780_ddram_read(HD44780_TEST_ADDR, &check_byte);
	if(check_byte!=HD44780_TEST_CHAR1)
	{
		return HD44780_ERR_BUS;
	}
	if(error_collector!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Send test symbol 2.
	error_collector |= hd44780_set_x_position(HD44780_TEST_ADDR);
	error_collector |= hd44780_wr_data(HD44780_TEST_CHAR2);
	// Read symbol back.
	error_collector = hd44780_ddram_read(HD44780_TEST_ADDR, &check_byte);
	if(check_byte!=HD44780_TEST_CHAR2)
	{
		return HD44780_ERR_BUS;
	}
	if(error_collector!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Send symbol from custom-CG area.
	error_collector |= hd44780_set_x_position(HD44780_TEST_ADDR);
	error_collector |= hd44780_wr_data(0x00);
	// Cycle through all lines of one CG character.
	for(uint8_t idx=0;idx<8;idx+=2)
	{
		// Save CG line.
		error_collector |= hd44780_cgram_read(idx, &saved_line);
		// Re-set CGRAM address.
		error_collector |= hd44780_wr_cmd(HD44780_CMD_SCGR|idx);
		// Write test pattern.
		// (PT6314 always returns "010x" as MSBs of CG RAM reads)
		error_collector |= hd44780_wr_data(HD44780_TEST_CHAR1&HD44780_TEST_CG_MASK);
		// Read pattern back.
		error_collector |= hd44780_cgram_read(idx, &check_byte);
		if((check_byte&HD44780_TEST_CG_MASK)!=(HD44780_TEST_CHAR1&HD44780_TEST_CG_MASK))
		{
			return HD44780_ERR_BUS;
		}
		if(error_collector!=0)
		{
			return HD44780_ERR_BUSY;
		}
		// Repeat with 2nd test pattern.
		error_collector |= hd44780_wr_cmd(HD44780_CMD_SCGR|idx);
		error_collector |= hd44780_wr_data(HD44780_TEST_CHAR2&HD44780_TEST_CG_MASK);
		error_collector |= hd44780_cgram_read(idx, &check_byte);
		if((check_byte&HD44780_TEST_CG_MASK)!=(HD44780_TEST_CHAR2&HD44780_TEST_CG_MASK))
		{
			return HD44780_ERR_BUS;
		}
		if(error_collector!=0)
		{
			return HD44780_ERR_BUSY;
		}
		// Return CG line contents.
		hd44780_wr_cmd(HD44780_CMD_SCGR|idx);
		hd44780_wr_data(saved_line);
	}
	// Restore symbol into RAM.
	hd44780_set_x_position(HD44780_TEST_ADDR);
	hd44780_wr_data(saved_char);
	return HD44780_OK;
}
