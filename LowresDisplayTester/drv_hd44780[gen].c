#include "drv_hd44780[gen].h"

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

//-------------------------------------- Write ASCII string from FLASH to display.
uint8_t hd44780_write_flash_string(const int8_t *str_output)
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
uint8_t hd44780_write_string(uint8_t *str_output)
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
