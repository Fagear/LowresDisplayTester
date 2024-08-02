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
		return HD44780_ERR_BUSY;
	}
	// Turn display on.
	HD44780s_write_command_byte(HD44780S_CMD_DISP|HD44780S_DISP_SCREEN);
	return HD44780_OK;
}

//-------------------------------------- Send one command to the display.
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
			return HD44780_OK;
		}
		_delay_ms(1);
	}
	return HD44780_ERR_BUSY;
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
			return HD44780_OK;
		}
		_delay_ms(1);
	}
	return HD44780_ERR_BUSY;
}

#endif /* CONF_EN_HD44780S */
