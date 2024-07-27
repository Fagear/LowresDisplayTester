#include "drv_cpu.h"						// Contains [F_CPU].
#include "drv_hd44780[serial].h"

#ifdef CONF_EN_HD44780S

uint8_t hd44780s_addr = 0;

static uint8_t (*serial_write_data)(uint8_t addr, uint8_t cnt, uint8_t *data);

//-------------------------------------- Set display address.
void HD44780s_set_address(uint8_t addr)
{
	hd44780s_addr = addr;
}

//-------------------------------------- Init HD44780-compatible display.
uint8_t HD44780s_init(void)
{
	uint8_t error_collector = 0;
	// >20ms power-on delay.
	_delay_ms(15);
	_delay_ms(15);
	// Repeat 3 times.
	for(uint8_t idx=0;idx<3;idx++)
	{
		// Set 8-bit display transfer mode.
		error_collector |= HD44780s_write_command_byte(HD44780S_CMD_FUNC|HD44780S_FUNC_8BIT);
		_delay_ms(5);
	}
	// Set 8-bit transfer mode, 2-line display, 5x8 font.
	error_collector |= HD44780s_write_command_byte(HD44780S_CMD_FUNC|HD44780S_FUNC_8BIT|HD44780S_FUNC_2LINE);
	// Turn display off and set no cursor, no blinking.
	error_collector |= HD44780s_write_command_byte(HD44780S_CMD_DISP);
	// Clear display.
	error_collector |= HD44780s_write_command_byte(HD44780S_CMD_CLR);
	// Set cursor shift: "right".
	error_collector |= HD44780s_write_command_byte(HD44780S_CMD_ENTRY|HD44780S_ENTRY_RIGHT);
	// Check error counter.
	if(error_collector!=0)
	{
		return HD44780S_ERR_BUSY;
	}
	// Turn display on.
	HD44780s_write_command_byte(HD44780S_CMD_DISP|HD44780S_DISP_SCREEN);
	return HD44780S_OK;
}

//-------------------------------------- Send one byte to the display.
uint8_t HD44780s_write_command_byte(const uint8_t send_data)
{
	uint8_t cmd[S_PCKT_LEN];
	// One command - one transmittion.
	cmd[S_PCKT_CTRL] = SER_LAST_BIT;
	cmd[S_PCKT_DATA] = send_data;
	for(uint8_t try_idx=0;try_idx<SER_TRY_MAX;try_idx++)
	{
		if(serial_write_data(hd44780s_addr, S_PCKT_LEN, cmd)==0)
		{
			return HD44780S_OK;
		}
		_delay_ms(1);
	}
	return HD44780S_ERR_BUSY;
}

//-------------------------------------- Write a data byte to the display.
uint8_t HD44780s_write_data_byte(const uint8_t send_data)
{
	uint8_t cmd[S_PCKT_LEN];
	// One data byte - one transmittion.
	cmd[S_PCKT_CTRL] = SER_LAST_BIT|SER_DATA_BIT;
	cmd[S_PCKT_DATA] = send_data;
	for(uint8_t try_idx=0;try_idx<SER_TRY_MAX;try_idx++)
	{
		if(serial_write_data(hd44780s_addr, S_PCKT_LEN, cmd)==0)
		{
			return HD44780S_OK;
		}
		_delay_ms(1);
	}
	return HD44780S_ERR_BUSY;
}

//-------------------------------------- Set 2+ lines display cursor position.
uint8_t HD44780s_set_xy_position(uint8_t x_pos, uint8_t y_pos)
{
	// Limit address to max 40 chars per line.
	if(x_pos>0x27)
	{
		x_pos = 0x27;
	}
	// Calculate address.
	if(y_pos==1)
	{
		// Second line starts at AC 0x40.
		x_pos += 0x40;
	}
	else if(y_pos==2)
	{
		// Further limit address to max 20 chars per line.
		if(x_pos>0x13)
		{
			x_pos = 0x13;
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
	x_pos |= HD44780S_CMD_SDDR;
	// Set position.
	return HD44780s_write_command_byte(x_pos);
}

//-------------------------------------- Upload user-defined symbol.
uint8_t HD44780s_upload_symbol_flash(uint8_t symbol_number, const int8_t *symbol_array)
{
	uint8_t idx, error_collector = 0;
	// Mask out character selector above 7 (normally 0...7 are configurable).
	symbol_number &= 0x07;
	// Move DDRAM bits 3 bits left for CGRAM bits 5...3.
	symbol_number = (symbol_number<<3);
	// Add "Set CGRAM address" command bit.
	symbol_number |= HD44780S_CMD_SCGR;
	// Send CGRAM address command.
	error_collector |= HD44780s_write_command_byte(symbol_number);
	if(error_collector!=0)
	{
		return HD44780S_ERR_BUSY;
	}
	uint8_t cmd[(S_PCKT_DATA+8)];
	// One data byte - one transmittion.
	cmd[S_PCKT_CTRL] = SER_LAST_BIT|SER_DATA_BIT;
	// Cycle through rows of the CG matrix.
	for(idx=0;idx<8;idx++)
	{
		// Assemble data packet.
		cmd[S_PCKT_DATA+idx] = pgm_read_byte_near(symbol_array+idx);
	}
	// Try to send data packet through serial.
	for(uint8_t try_idx=0;try_idx<SER_TRY_MAX;try_idx++)
	{
		if(serial_write_data(hd44780s_addr, (S_PCKT_DATA+8), cmd)==0)
		{
			return HD44780S_OK;
		}
		_delay_ms(1);
	}
	return HD44780S_ERR_BUSY;
}

//-------------------------------------- Write ASCII string from RAM on display.
uint8_t HD44780s_write_string(uint8_t *str_output)
{
	uint8_t *send_char;
	uint8_t char_cnt;
	uint8_t cmd[S_PCKT_CTRL+SER_STR_LEN_MAX];
	// One data byte - one transmittion.
	cmd[S_PCKT_CTRL] = SER_LAST_BIT|SER_DATA_BIT;
	// Set first char.
	send_char = str_output;
	char_cnt = 0;
	// Copy symbols to internal buffer.
	while(send_char!=0x00)
	{
		char_cnt++;
		// Check for transmittion size limit.
		if(char_cnt>=SER_STR_LEN_MAX)
		{
			break;
		}
		// Copy the byte.
		cmd[char_cnt] = *(send_char);
		send_char++;
	}
	// Try to send data packet through serial.
	for(uint8_t try_idx=0;try_idx<SER_TRY_MAX;try_idx++)
	{
		if(serial_write_data(hd44780s_addr, char_cnt, cmd)==0)
		{
			return HD44780S_OK;
		}
		_delay_ms(1);
	}
	return HD44780S_ERR_BUSY;
}

//-------------------------------------- Write ASCII string from FLASH on display.
uint8_t HD44780s_write_flash_string(const int8_t *str_output)
{
	uint8_t *send_char;
	uint8_t char_cnt;
	uint8_t cmd[S_PCKT_CTRL+SER_STR_LEN_MAX];
	// One data byte - one transmittion.
	cmd[S_PCKT_CTRL] = SER_LAST_BIT|SER_DATA_BIT;
	// Set first char.
	send_char = (uint8_t *)str_output;
	char_cnt = 0;
	// Copy symbols to internal buffer.
	while(send_char!=0x00)
	{
		char_cnt++;
		// Check for transmittion size limit.
		if(char_cnt>=SER_STR_LEN_MAX)
		{
			break;
		}
		// Copy the byte.
		cmd[char_cnt] = pgm_read_byte_near(send_char);
		send_char++;
	}
	// Try to send data packet through serial.
	for(uint8_t try_idx=0;try_idx<SER_TRY_MAX;try_idx++)
	{
		if(serial_write_data(hd44780s_addr, char_cnt, cmd)==0)
		{
			return HD44780S_OK;
		}
		_delay_ms(1);
	}
	return HD44780S_ERR_BUSY;
}

#endif /* CONF_EN_HD44780S */
