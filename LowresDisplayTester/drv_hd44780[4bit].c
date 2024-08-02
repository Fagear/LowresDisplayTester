#include "config.h"
#include "drv_cpu.h"						// Contains [F_CPU].
#include "drv_hd44780[4bit].h"

#ifdef CONF_EN_HD44780P

uint8_t disp_resolution = HD44780_RES_8X1,
	disp_cyr = HD44780_CYR_NOCONV;

extern void __builtin_avr_delay_cycles(unsigned long);

//-------------------------------------- Setup options for HD44780-compatible display.
uint8_t HD44780_setup(uint8_t disp_res, uint8_t cyr_conv)
{
	if((disp_res<HD44780_RES_LIM)&&(cyr_conv<HD44780_CYR_LIM))
	{
		disp_resolution = disp_res;
		disp_cyr = cyr_conv;
		return HD44780_OK;
	}
	return HD44780_ERR_DATA;
}

//-------------------------------------- Setup control bus as output.
inline static void HD44780_ctrl_setup(void)
{
	HD44780CTRL_PORT &= ~(HD44780_A0|HD44780_RW|HD44780_E);
	HD44780CTRL_DIR |= HD44780_A0|HD44780_RW|HD44780_E;
}

//-------------------------------------- Setup data bus as output.
inline static void HD44780_data_output(void)
{
	HD44780DATA_DIR |= HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7;
};

//-------------------------------------- Setup data bus as input.
inline static void HD44780_data_input(void)
{
	// Enable pull-up for BUSY flag reading.
	HD44780DATA_PORT |= HD44780_D7;
	// Set pins as inputs.
	HD44780DATA_DIR &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
};

//-------------------------------------- Set 4-bit data to the data bus.
inline static void HD44780_set_data(uint8_t data4b)
{
	uint8_t port_data;
	// Read old port state.
	port_data = HD44780DATA_PORT;
	// Clear LCD data bits.
	port_data &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
	// Set "1"s.
	if((data4b&(1<<4))!=0)
	{
		port_data |= HD44780_D4;
	}
	if((data4b&(1<<5))!=0)
	{
		port_data |= HD44780_D5;
	}
	if((data4b&(1<<6))!=0)
	{
		port_data |= HD44780_D6;
	}
	if((data4b&(1<<7))!=0)
	{
		port_data |= HD44780_D7;
	}
	// Update port.
	HD44780DATA_PORT = port_data;
}

//-------------------------------------- Start data exchange.
inline static void HD44780_start_ex(void)
{
	// Pin "E" set high.
	HD44780CTRL_PORT |= HD44780_E;
}

//-------------------------------------- Stop data exchange.
inline static void HD44780_stop_ex(void)
{
	// Pin "E" set low.
	HD44780CTRL_PORT &= ~HD44780_E;
}

//-------------------------------------- Init HD44780-compatible display.
uint8_t HD44780_init(void)
{
	uint8_t error_collector = 0, idx;
	// Set bits of display as outputs and send "0".
	HD44780_ctrl_setup();
	HD44780_data_output();
	HD44780_set_data(0x00);
	// >20ms power-on delay.
	_delay_ms(15);
	_delay_ms(15);
	// Set 8-bit display transfer mode.
	HD44780_set_data(HD44780_CMD_FUNC|HD44780_FUNC_8BIT);
	// Repeat 3 times.
	for(idx=0;idx<3;idx++)
	{
		HD44780_start_ex();
		// >230ns delay to hold E output.
		_delay_us(1);
		HD44780_stop_ex();
		// >4.1ms delay.
		_delay_ms(5);
	}
	// Set 4-bit transfer mode.
	HD44780_set_data(HD44780_CMD_FUNC);
	HD44780_start_ex();
	// >230ns delay.
	_delay_us(1);
	HD44780_stop_ex();
	// >40us delay.
	_delay_us(1);
	HD44780_set_data(0x0);
	_delay_us(40);
	// Set 4-bit transfer mode (4-bit) and codepage (page 1).
	// Set 4-bit transfer mode (4-bit), 2-line display, 5x8 font.
	error_collector |= HD44780_write_command_byte(HD44780_CMD_FUNC|HD44780_FUNC_2LINE);
	// Turn display off and set no cursor, no blinking.
	error_collector |= HD44780_write_command_byte(HD44780_CMD_DISP);
	// Clear display.
	error_collector |= HD44780_write_command_byte(HD44780_CMD_CLR);
	// Set cursor shift: "right".
	error_collector |= HD44780_write_command_byte(HD44780_CMD_ENTRY|HD44780_ENTRY_RIGHT);
	// Check error counter.
	if(error_collector!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Turn display on.
	HD44780_write_command_byte(HD44780_CMD_DISP|HD44780_DISP_SCREEN);
	// Check bus connection to the display and its RAM.
	//return HD44780_selftest();
	return HD44780_OK;
}

//-------------------------------------- Read a byte from the display.
uint8_t HD44780_raw_read(void)
{
	uint8_t bus_state, disp_data = 0;
	HD44780_start_ex();
	// Set data bus as input.
	HD44780_data_input();
	// >=360ns delay (wait for data delay, td>=360ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(8);
	// Reading first half-byte.
	bus_state = HD44780DATA_SRC&(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
	if((bus_state&HD44780_D7)!=0) disp_data |= (1<<7);
	if((bus_state&HD44780_D6)!=0) disp_data |= (1<<6);
	if((bus_state&HD44780_D5)!=0) disp_data |= (1<<5);
	if((bus_state&HD44780_D4)!=0) disp_data |= (1<<4);
	// >=100ns delay (to complete minimum E up time, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(2);
	HD44780_stop_ex();
	// >=550ns delay (to complete full E cycle, tc>=1000ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(12);
	HD44780_start_ex();
	// >=450ns delay (wait E pulse width, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	// Reading second half-byte.
	bus_state = HD44780DATA_SRC&(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
	if((bus_state&HD44780_D7)!=0) disp_data |= (1<<3);
	if((bus_state&HD44780_D6)!=0) disp_data |= (1<<2);
	if((bus_state&HD44780_D5)!=0) disp_data |= (1<<1);
	if((bus_state&HD44780_D4)!=0) disp_data |= (1<<0);
	// >=100ns delay (to complete minimum E up time, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(2);
	HD44780_stop_ex();
	// >=550ns delay (to complete full E cycle, tc>=1000ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(12);
	// Disable pull-up on data bus.
	HD44780_set_data(0x00);
	// Set data bus as output.
	HD44780_data_output();
	return disp_data;
}

//-------------------------------------- Wait for display to become ready or abort by timeout.
uint8_t HD44780_wait_ready(uint8_t *disp_addr)
{
	uint8_t disp_data, cycle_cnt = HD44780_MAX_TRIES;

	// Pin "A0" set "0" to read busy/address.
	HD44780CTRL_PORT &= ~HD44780_A0;
	// Pin "R/W" set "1" to read status.
	HD44780CTRL_PORT |= HD44780_RW;
	// >60ns delay between "R/W" and "E" and let AVR input pull up.
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(2);
	// Try in cycle.
	DBG_1_ON;
	while(cycle_cnt>0)
	{
		// Read busy/address byte from the display.
		disp_data = HD44780_raw_read();
		// Check BUSY flag.
		if((disp_data&(1<<7))==0)
		{
			break;
		}
		else
		{
			DBG_2_ON;
			DBG_2_OFF;
		}
		// Display not ready yet (or does not exist), increase counter.
		cycle_cnt--;
		// Wait for the display to become ready.
		_delay_us(10);
	}
	DBG_1_OFF;
	// Clear control bus.
	HD44780CTRL_PORT &= ~(HD44780_A0|HD44780_RW);
	// Recheck BUSY flag to determine cause of loop exit.
	if((disp_data&(1<<7))==0)
	{
		// Return internal address.
		(*disp_addr) = disp_data;
		// Display got ready.
		return HD44780_OK;
	}
	// No answer from display in time.
	return HD44780_ERR_BUSY;
}

//-------------------------------------- Read a byte from the display.
uint8_t HD44780_read_byte(uint8_t *read_result)
{
	uint8_t disp_addr;

	// Check BUSY flag.
	if(HD44780_wait_ready(&disp_addr)!=HD44780_OK)
	{
		// Return error code (BUSY wait fail).
		return HD44780_ERR_BUSY;
	}
	// Pin "R/W" set "1" to read, pin "A0" set "1" to read data from DRAM.
	HD44780CTRL_PORT |= (HD44780_RW|HD44780_A0);
	// 60ns delay (RW to E setup, tsu>=60ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(2);
	// Perform raw bit-banging to read a byte from the data bus.
	*read_result = HD44780_raw_read();
	// Clear control bus.
	HD44780CTRL_PORT &= ~(HD44780_A0|HD44780_RW);
	// Everything went fine.
	return HD44780_OK;
}

//-------------------------------------- Send one command to the display.
uint8_t HD44780_write_command_byte(const uint8_t send_data)
{
	uint8_t disp_addr;
	// Check BUSY flag.
	if(HD44780_wait_ready(&disp_addr)!=HD44780_OK)
	{
		// Return error code (BUSY wait fail).
		return HD44780_ERR_BUSY;
	}

	HD44780_stop_ex();
	// Pin "R/W" set low to write data, sending command to display controller.
	HD44780CTRL_PORT &= ~(HD44780_RW|HD44780_A0);
	HD44780_start_ex();
	// Send first half-byte to display data bus.
	HD44780_set_data(send_data);
	// >=450ns delay (wait for minimum E pulse width, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	HD44780_stop_ex();
	// >=10ns delay (th2>=10ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(1);
	// Clear data bus.
	HD44780_set_data(0x00);
	// >=500ns delay (to complete full E cycle, tc>=1000ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	HD44780_start_ex();
	// Send second half-byte to display data bus.
	HD44780_set_data((send_data<<4));
	// >=450ns delay (wait for minimum E pulse width, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	HD44780_stop_ex();
	// >=10ns delay (th2>=10ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(1);
	// Clear data bus.
	HD44780_set_data(0x00);
	// >=400ns delay (to complete full E cycle, tc>=1000ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(8);
	// Everything went fine.
	return HD44780_OK;
}

//-------------------------------------- Write a data byte to the display.
uint8_t HD44780_write_data_byte(const uint8_t send_data)
{
	uint8_t disp_addr;
	// Check BUSY flag.
	if(HD44780_wait_ready(&disp_addr)!=HD44780_OK)
	{
		// Return error code (BUSY wait fail).
		return HD44780_ERR_BUSY;
	}

	HD44780_stop_ex();
	// Pin "R/W" set low to write data.
	HD44780CTRL_PORT &= ~HD44780_RW;
	// Sending data byte to display controller.
	HD44780CTRL_PORT |= HD44780_A0;
	HD44780_start_ex();
	// Send first half-byte to display data bus.
	HD44780_set_data(send_data);
	// >=450ns delay (wait for minimum E pulse width, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	HD44780_stop_ex();
	// >=10ns delay (th2>=10ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(1);
	// Clear data bus.
	HD44780_set_data(0x00);
	// >=500ns delay (to complete full E cycle, tc>=1000ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	HD44780_start_ex();
	// Send second half-byte to display data bus.
	HD44780_set_data((send_data<<4));
	// >=450ns delay (wait for minimum E pulse width, tw>=450ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(10);
	HD44780_stop_ex();
	// >=10ns delay (th2>=10ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(1);
	// Clear data bus.
	HD44780_set_data(0x00);
	// >=400ns delay (to complete full E cycle, tc>=1000ns).
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(8);
	// Clear control bus.
	HD44780CTRL_PORT &= ~HD44780_A0;
	// Everything went fine.
	return HD44780_OK;
}

#endif /* CONF_EN_HD44780P */
