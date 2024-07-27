#include "config.h"
#include "drv_cpu.h"						// Contains [F_CPU].
#include "drv_hd44780[4bit].h"

#ifdef CONF_EN_HD44780P

uint8_t disp_resolution = HD44780_RES_8X1,
	disp_cyr = HD44780_CYR_NOCONV;

#ifdef HD44780_USE_SCREEN_BUFFERS
uint8_t disp_shift_stage,
	disp_coord_x,
	disp_coord_y,
	disp_anim_dir,
	disp_redraw_state;
int8_t arr_page1[HD44780_MAX_COL][HD44780_MAX_ROWS],
	arr_page2[HD44780_MAX_COL][HD44780_MAX_ROWS];
int8_t (*ptr_output)[HD44780_MAX_ROWS], (*ptr_input)[HD44780_MAX_ROWS];
#endif	/* HD44780_USE_SCREEN_BUFFERS */

#ifdef HD44780_RU_REENCODE
// Char set conversion table for Cyrillic displays without CP1251 support.
const int8_t hd44780_cyr_conv_tbl[] PROGMEM =
{
//	0x_0, 0x_1, 0x_2, 0x_3, 0x_4, 0x_5, 0x_6, 0x_7, 0x_8, 0x_9, 0x_A, 0x_B, 0x_C, 0x_D, 0x_E, 0x_F
	0xFF, 0x0A, 0x0A, 0xCF, 0x0B, 0x1D, 0xEF, 0xFD, 0xA2, 0x19, 0x0E, 0xC8, 0x7E, 0x8A, 0x18, 0x0C,		// 0xA_
	0x99, 0x90, 0x92, 0x93, 0xCA, 0xCB, 0xFE, 0xCE, 0xB5, 0x09, 0x0F, 0xC9, 0x8E, 0x19, 0x1F, 0x0D,		// 0xB_
	0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 	// 0xC_
	0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1,		// 0xD_
	0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, 	// 0xE_
	0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7		// 0xF_
};
#endif /* HD44780_RU_REENCODE */

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

//-------------------------------------- Init HD44780-compatible display.
uint8_t HD44780_init(void)
{
	uint8_t error_collector = 0, idx;
	// Set bits of display as outputs and send "0".
	HD44780CTRL_PORT &= ~(HD44780_A0|HD44780_RW|HD44780_E);
	HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
	HD44780CTRL_DIR |= HD44780_A0|HD44780_RW|HD44780_E;
	HD44780DATA_DIR |= HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7;
	// >20ms power-on delay.
	_delay_ms(15);
	_delay_ms(15);
	// Set 8-bit display transfer mode.
	HD44780DATA_PORT |= HD44780_D4|HD44780_D5;
	// Repeat 3 times.
	for(idx=0;idx<3;idx++)
	{
		// Pin "E" set high.
		HD44780CTRL_PORT |= HD44780_E;
		// >230ns delay to hold E output.
		_delay_us(1);
		// Pin "E" set low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >4.1ms delay.
		_delay_ms(5);
	}
	// Set 4-bit transfer mode.
	HD44780DATA_PORT &= ~HD44780_D4;
	// Pin "E" set high.
	HD44780CTRL_PORT |= HD44780_E;
	// >230ns delay.
	_delay_us(1);
	// Pin "E" set low.
	HD44780CTRL_PORT &= ~HD44780_E;
	// >40us delay.
	_delay_us(1);
	HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
	_delay_us(40);
	// Set 4-bit transfer mode (4-bit) and codepage (page 1).
	// Set 4-bit transfer mode (4-bit), 2-line display, 5x8 font.
	error_collector |= HD44780_write_byte(HD44780_CMD_FUNC|HD44780_FUNC_2LINE, HD44780_CMD);
	// Turn display off and set no cursor, no blinking.
	error_collector |= HD44780_write_byte(HD44780_CMD_DISP, HD44780_CMD);
	// Clear display.
	error_collector |= HD44780_write_byte(HD44780_CMD_CLR, HD44780_CMD);
	// Set cursor shift: "right".
	error_collector |= HD44780_write_byte(HD44780_CMD_ENTRY|HD44780_ENTRY_RIGHT, HD44780_CMD);
#ifdef HD44780_USE_SCREEN_BUFFERS
	// Clear buffers.
	for(uint8_t rows=0;rows<HD44780_MAX_ROWS;rows++)
	{
		for(idx=0;idx<HD44780_MAX_COL;idx++)
		{
			// Fill pages with spaces.
			arr_page1[idx][rows] = ASCII_SPACE;
			arr_page2[idx][rows] = ASCII_SPACE;
			// Select first page as displayed one.
			ptr_output = arr_page1;
			disp_shift_stage = HD44780_MAX_COL;
			disp_anim_dir = FADE_RIGHT2LEFT;
			disp_redraw_state = HD44780_NO_UPDATE;
			disp_coord_x = 0;
			disp_coord_y = 0;
		}
	}
#endif	// HD44780_USE_SCREEN_BUFFERS
	// Check error counter.
	if(error_collector!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Turn display on.
	HD44780_write_byte(HD44780_CMD_DISP|HD44780_DISP_SCREEN, HD44780_CMD);
	// Check bus connection to the display and its RAM.
	//return HD44780_selftest();
	return HD44780_OK;
}

//-------------------------------------- Return number of selected columns.
uint8_t HD44780_columns(void)
{
	if((disp_resolution==HD44780_RES_8X1)||(disp_resolution==HD44780_RES_8X2))
	{
		return 8;
	}
	else if((disp_resolution==HD44780_RES_16X1)||(disp_resolution==HD44780_RES_16X2))
	{
		return 16;
	}
	else if((disp_resolution==HD44780_RES_20X1)||(disp_resolution==HD44780_RES_20X2)||(disp_resolution==HD44780_RES_20X4))
	{
		return 20;
	}
	else if((disp_resolution==HD44780_RES_24X1)||(disp_resolution==HD44780_RES_24X2))
	{
		return 24;
	}
	else if((disp_resolution==HD44780_RES_40X1)||(disp_resolution==HD44780_RES_40X2))
	{
		return 40;
	}
	else
	{
		return 0;
	}
}

//-------------------------------------- Return number of selected rows.
uint8_t HD44780_rows(void)
{
	if((disp_resolution>=HD44780_RES_8X2)||(disp_resolution<=HD44780_RES_40X2))
	{
		return 2;
	}
	else if(disp_resolution==HD44780_RES_20X4)
	{
		return 4;
	}
	else
	{
		return 1;
	}
}

//-------------------------------------- Wait for display to become ready or abort by timeout.
uint8_t HD44780_wait_ready(uint8_t *disp_addr)
{
	uint8_t bus_state, disp_data, cycle_cnt = HD44780_MAX_TRIES;

	// Pin "R/W" set "1" to read status.
	HD44780CTRL_PORT &= ~HD44780_A0;
	HD44780CTRL_PORT |= HD44780_RW;
	// >60ns delay between "R/W" and "E" and let AVR input pull up.
	// One NOP @20 MHz = 50 ns.
	__builtin_avr_delay_cycles(2);
	// Try in cycle.
	while(cycle_cnt>0)
	{
		// Set "E" pin high.
		HD44780CTRL_PORT |= HD44780_E;
		HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6);
		// Enable pull-up on data bus.
		HD44780DATA_PORT |= HD44780_D7;
		// Set data bus as input.
		HD44780DATA_DIR &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// >=360ns delay (wait for data delay, td>=360ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(8);
		// Check display readiness.
		disp_data = 0;
		bus_state = HD44780DATA_SRC&(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		if((bus_state&HD44780_D7)!=0)
			disp_data |= (1<<7);
		if((bus_state&HD44780_D6)!=0)
			disp_data |= (1<<6);
		if((bus_state&HD44780_D5)!=0)
			disp_data |= (1<<5);
		if((bus_state&HD44780_D4)!=0)
			disp_data |= (1<<4);
		if((disp_data&(1<<7))!=0)
		{
			// Display not ready yet (or does not exist), increase counter.
			cycle_cnt--;
		}
		// Finish two-cycle byte-reading procedure.
		// >=100ns delay (to complete minimum E up time, tw>=450ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(2);
		// Set "E" pin low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >=550ns delay (to complete full E cycle, tc>=1000ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(12);
		// Set "E" pin high.
		HD44780CTRL_PORT |= HD44780_E;
		// >=450ns delay (wait E pulse width, tw>=450ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(10);
		bus_state = HD44780DATA_SRC&(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		if((bus_state&HD44780_D7)!=0)
			disp_data |= (1<<3);
		if((bus_state&HD44780_D6)!=0)
			disp_data |= (1<<2);
		if((bus_state&HD44780_D5)!=0)
			disp_data |= (1<<1);
		if((bus_state&HD44780_D4)!=0)
			disp_data |= (1<<0);
		// Set "E" pin low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >=550ns delay (to complete full E cycle, tc>=1000ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(12);
		// Disable pull-up.
		HD44780DATA_PORT &= ~HD44780_D7;
		if((disp_data&(1<<7))==0)
		{
			// Display is ready, stop cycle.
			break;
		}
		// Set data bus as output.
		HD44780DATA_DIR |= (HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// Wait for the display to become ready.
		_delay_us(10);
	}
	// Disable pull-up on data bus.
	HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
	// Set data bus as output.
	HD44780DATA_DIR |= (HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);

	if((disp_data&(1<<7))==0)
	{
		// Return internal address.
		(*disp_addr) = disp_data;
		// Display got ready.
		return HD44780_OK;
	}
	HD44780CTRL_PORT &= ~HD44780_RW;
	// No answer from display in time.
	return HD44780_ERR_BUSY;
}

//-------------------------------------- Send one byte to the display.
uint8_t HD44780_write_byte(const uint8_t send_data, const uint8_t send_mode)
{
	uint8_t bus_state, disp_data, disp_addr;

	// Check BUSY flag.
	if(HD44780_wait_ready(&disp_addr)==HD44780_OK)
	{
		// Pin "R/W" set low to write data.
		HD44780CTRL_PORT &= ~HD44780_E;
		HD44780CTRL_PORT &= ~HD44780_RW;
		// Pin "A0" set by [send_mode] parameter.
		if(send_mode==HD44780_CMD)
		{
			// Sending command to display controller.
			HD44780CTRL_PORT &= ~HD44780_A0;
			disp_data = send_data;
		}
		else
		{
			// Sending data byte to display controller.
			HD44780CTRL_PORT |= HD44780_A0;
			disp_data = send_data;
			// Check if it is needed to modify byte.
			if(send_mode==HD44780_NONZERO_DIGIT)
			{
				// Draw a digit, but if it's zero - draw a space.
				if(disp_data==0)
				{
					// Draw ASCII space char in place of zero.
					disp_data = ASCII_SPACE;
				}
				else
				{
					// Non-zero symbols.
					if(disp_data>9)
					{
						// It's not a single digit! Draw a ASCII overflow filler.
						disp_data = ASCII_OVF;
					}
					else
					{
						// Convert from digit binary to ASCII code for the digit.
						disp_data = disp_data+ASCII_ZERO;
					}
				}
			}
			else if(send_mode==HD44780_DIGIT)
			{
				// Draw a digit (0...9).
				if(disp_data>9)
				{
					// It's not a single digit! Draw a hash.
					disp_data = ASCII_OVF;
				}
				else
				{
					// Convert from digit binary to ASCII code for the digit.
					disp_data = disp_data+ASCII_ZERO;
				}
			}
			else if(send_mode==HD44780_HEX_DIGIT)
			{
				if(disp_data>=0x10)
				{
					// It's not a single digit! Draw a hash.
					disp_data = ASCII_OVF;
				}
				else if(disp_data>9)
				{
					// Shift value to ASCII 'A'...'F' symbol.
					disp_data = disp_data+0x37;
				}
				else
				{
					// Convert from digit binary to ASCII code for the digit.
					disp_data = disp_data+ASCII_ZERO;
				}
			}
#ifdef HD44780_RU_REENCODE
			else if(disp_cyr!=HD44780_CYR_NOCONV)
			{
				// Need to convert CP1251 Cyrillic character for non-CP1251 Cyrillic display.
				if(disp_data>=0xA0)
				{
					disp_data = pgm_read_byte_near(hd44780_cyr_conv_tbl+disp_data);
				}
			}
#endif /* HD44780_RU_REENCODE */
		}
		bus_state = 0;
		// Select bits to pull up on the data bus.
		if((disp_data&(1<<4))!=0)
		{
			bus_state |= HD44780_D4;
		}
		if((disp_data&(1<<5))!=0)
		{
			bus_state |= HD44780_D5;
		}
		if((disp_data&(1<<6))!=0)
		{
			bus_state |= HD44780_D6;
		}
		if((disp_data&(1<<7))!=0)
		{
			bus_state |= HD44780_D7;
		}
		// Data bus is already cleared in [HD44780_wait_ready()].
		// Set "E" pin high.
		HD44780CTRL_PORT |= HD44780_E;
		// Send first half-byte to display data bus.
		HD44780DATA_PORT |= bus_state;
		// >=450ns delay (wait for minimum E pulse width, tw>=450ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(10);
		// Set "E" pin low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >=10ns delay (th2>=10ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(1);
		bus_state = 0;
		// Select bits to pull up on the data bus.
		if((disp_data&(1<<0))!=0)
		{
			bus_state |= HD44780_D4;
		}
		if((disp_data&(1<<1))!=0)
		{
			bus_state |= HD44780_D5;
		}
		if((disp_data&(1<<2))!=0)
		{
			bus_state |= HD44780_D6;
		}
		if((disp_data&(1<<3))!=0)
		{
			bus_state |= HD44780_D7;
		}
		// Clear data bus.
		HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// >=500ns delay (to complete full E cycle, tc>=1000ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(10);
		// Set "E" pin high.
		HD44780CTRL_PORT |= HD44780_E;
		// Send second half-byte to display data bus.
		HD44780DATA_PORT |= bus_state;
		// >=450ns delay (wait for minimum E pulse width, tw>=450ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(10);
		// Set "E" pin low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >=10ns delay (th2>=10ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(1);
		// Clear data bus.
		HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// >=400ns delay (to complete full E cycle, tc>=1000ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(8);
		// Clear control bus.
		HD44780CTRL_PORT &= ~(HD44780_A0|HD44780_RW|HD44780_E);
		// Everything went fine.
		return HD44780_OK;
	}
	else
	{
		// Return error code (BUSY wait fail).
		return HD44780_ERR_BUSY;
	}
}

//-------------------------------------- Write a data byte to the display.
uint8_t HD44780_write_data_byte(const uint8_t send_data)
{
	return HD44780_write_byte(send_data, HD44780_DATA);
}

//-------------------------------------- Read a byte from the display.
uint8_t HD44780_read_byte(uint8_t *read_result)
{
	uint8_t bus_state, disp_addr, disp_data = 0;

	// Check BUSY flag.
	if(HD44780_wait_ready(&disp_addr)==HD44780_OK)
	{
		// Set "A0" pin high.
		HD44780CTRL_PORT |= HD44780_A0;
		// 60ns delay (RW to E setup, tsu>=60ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(2);
		// Set "E" pin high.
		HD44780CTRL_PORT |= HD44780_E;
		// Set data bus as input.
		HD44780DATA_DIR &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// >=360ns delay (wait for data delay, td>=360ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(8);
		// Reading first half-byte.
		bus_state = HD44780DATA_SRC&(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		if((bus_state&HD44780_D7)!=0)
			disp_data |= (1<<7);
		if((bus_state&HD44780_D6)!=0)
			disp_data |= (1<<6);
		if((bus_state&HD44780_D5)!=0)
			disp_data |= (1<<5);
		if((bus_state&HD44780_D4)!=0)
			disp_data |= (1<<4);
		// >=100ns delay (to complete minimum E up time, tw>=450ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(2);
		// Set "E" pin low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >=550ns delay (to complete full E cycle, tc>=1000ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(12);
		// Set "E" pin high.
		HD44780CTRL_PORT |= HD44780_E;
		// >=360ns delay (wait for data delay, td>=360ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(8);
		// Reading second half-byte.
		bus_state = HD44780DATA_SRC&(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		if((bus_state&HD44780_D7)!=0)
			disp_data |= (1<<3);
		if((bus_state&HD44780_D6)!=0)
			disp_data |= (1<<2);
		if((bus_state&HD44780_D5)!=0)
			disp_data |= (1<<1);
		if((bus_state&HD44780_D4)!=0)
			disp_data |= (1<<0);
		// >=100ns delay (to complete minimum E up time, tw>=450ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(2);
		// Set "E" pin low.
		HD44780CTRL_PORT &= ~HD44780_E;
		// >=550ns delay (to complete full E cycle, tc>=1000ns).
		// One NOP @20 MHz = 50 ns.
		__builtin_avr_delay_cycles(12);
		// Disable pull-up on data bus.
		HD44780DATA_PORT &= ~(HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// Set data bus as output.
		HD44780DATA_DIR |= (HD44780_D4|HD44780_D5|HD44780_D6|HD44780_D7);
		// Clear control bus.
		HD44780CTRL_PORT &= ~(HD44780_A0|HD44780_RW|HD44780_E);
		// Update output.
		*read_result = disp_data;
		// Everything went fine.
		return HD44780_OK;
	}
	else
	{
		// Return error code (BUSY wait fail).
		return HD44780_ERR_BUSY;
	}
}

//-------------------------------------- Set 1 line display cursor position.
uint8_t HD44780_set_x_position(uint8_t x_pos)
{
	// Limit address to max 80 chars in line.
	if(x_pos>0x67)
	{
		x_pos = 0x67;
	}
	// Set DDRAM address command.
	x_pos |= HD44780_CMD_SDDR;
	// Set position.
	return HD44780_write_byte(x_pos, HD44780_CMD);
}

//-------------------------------------- Set 2+ lines display cursor position.
uint8_t HD44780_set_xy_position(uint8_t x_pos, uint8_t y_pos)
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
	x_pos |= HD44780_CMD_SDDR;
	// Set position.
	return HD44780_write_byte(x_pos, HD44780_CMD);
}

//-------------------------------------- Read from DDRAM until stable results.
// Some displays are fast (instant ready), but lag in updating DRAM anyway.
// (US2066 OLEDs, PT6314 VFDs, ahem)
uint8_t HD44780_ddram_read(uint8_t x_pos, uint8_t *read_result)
{
	uint8_t error_collector = 0, read_tries, arr_idx;
	uint8_t read_data[3];
	read_tries = 8;
	arr_idx = 0;
	while(read_tries>0)
	{
		// Read data at position in DDRAM.
		error_collector |= HD44780_set_x_position(x_pos);
		error_collector |= HD44780_read_byte(&read_data[arr_idx]);
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
uint8_t HD44780_cgram_read(uint8_t char_idx, uint8_t *read_result)
{
	uint8_t error_collector = 0, read_tries, arr_idx;
	uint8_t read_data[3];
	read_tries = 8;
	arr_idx = 0;
	while(read_tries>0)
	{
		// Read data at position in CGRAM.
		error_collector |= HD44780_write_byte((HD44780_CMD_SCGR|char_idx), HD44780_CMD);
		error_collector |= HD44780_read_byte(&read_data[arr_idx]);
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
uint8_t HD44780_selftest(void)
{
	uint8_t error_collector = 0, saved_char = ASCII_SPACE, saved_line, check_byte;
	// Save symbol to variable.
	error_collector |= HD44780_ddram_read(HD44780_TEST_ADDR, &saved_char);
	// Send test symbol 1.
	error_collector |= HD44780_set_x_position(HD44780_TEST_ADDR);
	error_collector |= HD44780_write_byte(HD44780_TEST_CHAR1, HD44780_DATA);
	// Read symbol back.
	error_collector = HD44780_ddram_read(HD44780_TEST_ADDR, &check_byte);
	if(check_byte!=HD44780_TEST_CHAR1)
	{
		return HD44780_ERR_BUS;
	}
	if(error_collector!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Send test symbol 2.
	error_collector |= HD44780_set_x_position(HD44780_TEST_ADDR);
	error_collector |= HD44780_write_byte(HD44780_TEST_CHAR2, HD44780_DATA);
	// Read symbol back.
	error_collector = HD44780_ddram_read(HD44780_TEST_ADDR, &check_byte);
	if(check_byte!=HD44780_TEST_CHAR2)
	{
		return HD44780_ERR_BUS;
	}
	if(error_collector!=0)
	{
		return HD44780_ERR_BUSY;
	}
	// Send symbol from custom-CG area.
	error_collector |= HD44780_set_x_position(HD44780_TEST_ADDR);
	error_collector |= HD44780_write_byte(0, HD44780_DATA);
	// Cycle through all lines of one CG character.
	for(uint8_t idx=0;idx<8;idx+=2)
	{
		// Save CG line.
		error_collector |= HD44780_cgram_read(idx, &saved_line);
		// Re-set CGRAM address.
		error_collector |= HD44780_write_byte((HD44780_CMD_SCGR|idx), HD44780_CMD);
		// Write test pattern.
		// (PT6314 always returns "010x" as MSBs of CG RAM reads)
		error_collector |= HD44780_write_byte((HD44780_TEST_CHAR1&HD44780_TEST_CG_MASK), HD44780_DATA);
		// Read pattern back.
		error_collector |= HD44780_cgram_read(idx, &check_byte);
		if((check_byte&HD44780_TEST_CG_MASK)!=(HD44780_TEST_CHAR1&HD44780_TEST_CG_MASK))
		{
			return HD44780_ERR_BUS;
		}
		if(error_collector!=0)
		{
			return HD44780_ERR_BUSY;
		}
		// Repeat with 2nd test pattern.
		error_collector |= HD44780_write_byte((HD44780_CMD_SCGR|idx), HD44780_CMD);
		error_collector |= HD44780_write_byte((HD44780_TEST_CHAR2&HD44780_TEST_CG_MASK), HD44780_DATA);
		error_collector |= HD44780_cgram_read(idx, &check_byte);
		if((check_byte&HD44780_TEST_CG_MASK)!=(HD44780_TEST_CHAR2&HD44780_TEST_CG_MASK))
		{
			return HD44780_ERR_BUS;
		}
		if(error_collector!=0)
		{
			return HD44780_ERR_BUSY;
		}
		// Return CG line contents.
		HD44780_write_byte((HD44780_CMD_SCGR|idx), HD44780_CMD);
		HD44780_write_byte(saved_line, HD44780_DATA);
	}
	// Restore symbol into RAM.
	HD44780_set_x_position(HD44780_TEST_ADDR);
	HD44780_write_byte(saved_char, HD44780_DATA);
	return HD44780_OK;
}

//-------------------------------------- Upload user-defined symbol.
uint8_t HD44780_upload_symbol_flash(uint8_t symbol_number, const int8_t *symbol_array)
{
	uint8_t idx, err_count = 0;
	// Mask out character selector above 7 (normally 0...7 are configurable).
	symbol_number &= 0x07;
	// Move DDRAM bits 3 bits left for CGRAM bits 5...3.
	//symbol_number += symbol_number*8;
	symbol_number = (symbol_number<<3);
	// Add "Set CGRAM address" command bit.
	symbol_number |= HD44780_CMD_SCGR;
	// Send CGRAM address command.
	err_count |= HD44780_write_byte(symbol_number, HD44780_CMD);
	// Cycle through rows of the CG matrix.
	for(idx=0;idx<8;idx++)
	{
		// Write byte.
		err_count |= HD44780_write_byte(pgm_read_byte_near(symbol_array+idx), HD44780_DATA);
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

#ifdef HD44780_USE_DIRECT_STRINGS
//-------------------------------------- Write [uint8_t] as number on display.
uint8_t HD44780_write_8bit_number(const uint8_t send_number, const uint8_t send_mode)
{
	uint8_t err_count = 0;
	// Convert and output.
	if(send_mode==HD44780_NUMBER_SPACES)
	{
		err_count |= HD44780_write_byte(send_number/100, HD44780_NONZERO_DIGIT);
		if(send_number<100)
			err_count |= HD44780_write_byte(send_number%100/10, HD44780_NONZERO_DIGIT);
		else
			err_count |= HD44780_write_byte(send_number%100/10, HD44780_DIGIT);
		err_count |= HD44780_write_byte(send_number%10, HD44780_DIGIT);
	}
	else if(send_mode==HD44780_NUMBER)
	{
		if(send_number>=100)
		{
			err_count |= HD44780_write_byte(send_number/100, HD44780_DIGIT);
			err_count |= HD44780_write_byte(send_number%100/10, HD44780_DIGIT);
			err_count |= HD44780_write_byte(send_number%10, HD44780_DIGIT);
		}
		else if(send_number>=10)
		{
			err_count |= HD44780_write_byte(send_number%100/10, HD44780_DIGIT);
			err_count |= HD44780_write_byte(send_number%10, HD44780_DIGIT);
		}
		else
		{
			err_count |= HD44780_write_byte(send_number%10, HD44780_DIGIT);
		}
	}
	else if(send_mode==HD44780_NUMBER_HEX)
	{
		err_count |= HD44780_write_byte(send_number/0x10, HD44780_HEX_DIGIT);
		err_count |= HD44780_write_byte(send_number%0x10, HD44780_HEX_DIGIT);
	}
	// Check for errors.
	if(err_count==0)
		return HD44780_OK;
	else
		return HD44780_ERR_BUSY;
}

//-------------------------------------- Write ASCII string from RAM on display.
uint8_t HD44780_write_string(uint8_t *str_output)
{
	uint8_t send_char, char_cnt, disp_status = HD44780_OK;
	// Keep first char.
	send_char = *(str_output);
	char_cnt = 0;
	// Cycle while symbols left.
	while(send_char!=0x00)
	{
		// Next char.
		char_cnt++;
		if(char_cnt>=HD44780_MAX_COL)
		{
			return HD44780_ERR_DATA;
		}
		// If previous sending failed - skip data transfer.
		if(disp_status==HD44780_OK)
		{
			// Send char to display.
			disp_status = HD44780_write_byte(send_char, HD44780_DATA);
		}
		else
		{
			break;
		}
		// Go to the next byte in the memory.
		send_char = *(str_output+char_cnt);
	}
	if(disp_status==HD44780_OK)
		return HD44780_OK;
	else
		return HD44780_ERR_BUSY;
}

//-------------------------------------- Write ASCII string from FLASH on display.
uint8_t HD44780_write_flash_string(const int8_t *str_output)
{
	uint8_t send_char, char_cnt, disp_status = HD44780_OK;
	// Keep first char.
	send_char = pgm_read_byte_near(str_output);
	char_cnt = 0;
	// Cycle while symbols left.
	while(send_char!=0x00)
	{
		// Next char.
		char_cnt++;
		if(char_cnt>=HD44780_MAX_COL)
		{
			return HD44780_ERR_DATA;
		}
		// If previous sending failed - skip data transfer.
		if(disp_status==HD44780_OK)
		{
			// Send char to display.
			disp_status = HD44780_write_byte(send_char, HD44780_DATA);
		}
		else
		{
			break;
		}
		// Go to the next byte in the memory.
		send_char = pgm_read_byte_near(str_output+char_cnt);
	}
	if(disp_status==HD44780_OK)
		return HD44780_OK;
	else
		return HD44780_ERR_BUSY;
}

//-------------------------------------- Fill selected line with spaces.
uint8_t HD44780_clear_line(const uint8_t line_idx)
{
	uint8_t idx, disp_status = HD44780_OK;
	if(line_idx>=HD44780_rows())
	{
		return HD44780_ERR_DATA;
	}
	HD44780_set_xy_position(0, line_idx);
	for(idx=0;idx<HD44780_columns();idx++)
	{
		disp_status = HD44780_write_byte(ASCII_SPACE, HD44780_DATA);
		if(disp_status!=HD44780_OK)
		{
			break;
		}
	}
	return disp_status;
}
#endif // HD44780_USE_DIRECT_STRINGS

#ifdef HD44780_USE_SCREEN_BUFFERS
//-------------------------------------- Swap displaying page.
void HD44780_swap_display_pages(uint8_t move_direction)
{
	if(ptr_output==arr_page1)
	{
		ptr_output = arr_page2;
	}
	else
	{
		ptr_output = arr_page1;
	}
	disp_shift_stage = 0;
	if(move_direction==FADE_RIGHT2LEFT)
		disp_anim_dir=FADE_RIGHT2LEFT;
	else if(move_direction==FADE_LEFT2RIGHT)
		disp_anim_dir=FADE_LEFT2RIGHT;
}

//-------------------------------------- Select displaying page.
void HD44780_select_display_page(uint8_t move_direction, uint8_t selector)
{
	if(selector==HD44780_PAGE0)
	{
		if(ptr_output==arr_page2)
		{
			ptr_output = arr_page1;
			disp_shift_stage = 0;
		}
	}
	else if(selector==HD44780_PAGE1)
	{
		if(ptr_output==arr_page1)
		{
			ptr_output = arr_page2;
			disp_shift_stage = 0;
		}
	}
	if(move_direction==FADE_RIGHT2LEFT)
		disp_anim_dir = FADE_RIGHT2LEFT;
	else if(move_direction==FADE_LEFT2RIGHT)
		disp_anim_dir = FADE_LEFT2RIGHT;
}

//-------------------------------------- Set buffer filling page.
uint8_t HD44780_select_fill_page(const uint8_t page_num)
{
	if(page_num>HD44780_PAGE1)
		return HD44780_ERR_DATA;
	if(page_num==HD44780_PAGE0)
	{
		ptr_input = arr_page1;
	}
	else
	{
		ptr_input = arr_page2;
	}
	return HD44780_OK;
}

//-------------------------------------- Select displaying page as filling target.
void HD44780_select_fill_shown(void)
{
	if(ptr_output==arr_page1)
	{
		ptr_input = arr_page1;
	}
	else
	{
		ptr_input = arr_page2;
	}
}

//-------------------------------------- Select hidden page as filling target.
void HD44780_select_fill_hidden(void)
{
	if(ptr_output==arr_page1)
	{
		ptr_input = arr_page2;
	}
	else
	{
		ptr_input = arr_page1;
	}
}

//-------------------------------------- Control switching displaying page.
void HD44780_buffer_update(void)
{
	uint8_t idx, row;
	// Check if page transition is in progress.
	if(disp_shift_stage<HD44780_MAX_COL)
	{
		// Proceed with page transition.
		disp_shift_stage++;
		for(idx=0;idx<disp_shift_stage;idx++)
		{
			if(disp_anim_dir==FADE_LEFT2RIGHT)
			{
				for(row=0;row<HD44780_MAX_ROWS;row++)
				{
					HD44780_set_xy_position(idx, row);
					HD44780_write_byte(ptr_output[idx][row], HD44780_DATA);
				}
			}
			else
			{
				for(row=0;row<HD44780_MAX_ROWS;row++)
				{
					HD44780_set_xy_position((HD44780_MAX_COL-1)-idx, row);
					HD44780_write_byte(ptr_output[(HD44780_MAX_COL-1)-idx][row], HD44780_DATA);
				}
			}
		}
	}
	// No page transition.
	// Check if screen should be redrawn.
	else if(disp_redraw_state==HD44780_NEEDS_UPDATE)
	{
		for(row=0;row<HD44780_MAX_ROWS;row++)
		{
			HD44780_set_xy_position(0, row);
			for(idx=0;idx<HD44780_MAX_COL;idx++)
			{
				HD44780_write_byte(ptr_output[idx][row], HD44780_DATA);
			}
		}
		disp_redraw_state = HD44780_NO_UPDATE;
	}
}

//-------------------------------------- Set display buffer cursor position.
inline uint8_t HD44780_buf_set_position(const uint8_t x_pos, const uint8_t y_pos)
{
	if((x_pos>HD44780_MAX_COL)||(y_pos>HD44780_MAX_ROWS))
	{
		return HD44780_ERR_DATA;
	}
	disp_coord_x = x_pos;
	disp_coord_y = y_pos;
	return HD44780_OK;
}

//-------------------------------------- Write one byte to display buffer.
uint8_t HD44780_buf_write_byte(const uint8_t send_data, const uint8_t send_mode)
{
	uint8_t disp_data;
	// Determining data type.
	if(send_mode==HD44780_CMD)
	{
		// Non-buffering command.
		return HD44780_write_byte(send_data, send_mode);
	}
	else if(send_mode==HD44780_NONZERO_DIGIT)
	{
		// Draw a digit, but if it's zero - draw a space.
		if(send_data==0)
		{
			// Draw ASCII space char in place of zero.
			disp_data = ASCII_SPACE;
		}
		else
		{
			// Non-zero symbols.
			if(send_data>9)
			{
				// It's not a single digit! Draw a ASCII hash.
				disp_data = ASCII_OVF;
			}
			else
			{
				// Convert from digit binary to ASCII code for the digit.
				disp_data = send_data+ASCII_ZERO;
			}
		}
	}
	else if(send_mode==HD44780_DIGIT)
	{
		// Draw a digit (0...9).
		if(send_data>9)
		{
			// It's not a single digit! Draw a ASCII hash.
			disp_data = ASCII_OVF;
		}
		else
		{
			// Convert from digit binary to ASCII code for the digit.
			disp_data = send_data+ASCII_ZERO;
		}
	}
	else if(send_mode==HD44780_HEX_DIGIT)
	{
		if(send_data>=0x10)
		{
			// It's not a single digit! Draw a hash.
			disp_data = ASCII_OVF;
		}
		else if(send_data>9)
		{
			// Shift value to ASCII 'A'...'F' symbol.
			disp_data = send_data+0x37;
		}
		else
		{
			// Convert from digit binary to ASCII code for the digit.
			disp_data = send_data+ASCII_ZERO;
		}
	}
#ifdef HD44780_RU_REENCODE
	else if(disp_cyr!=HD44780_CYR_NOCONV)
	{
		// Need to convert CP1251 Cyrillic character for non-CP1251 Cyrillic display.
		if(send_data>=0xA0)
		{
			disp_data = pgm_read_byte_near(hd44780_cyr_conv_tbl+send_data);
		}
	}
#endif /* HD44780_RU_REENCODE */
	else if(send_mode==HD44780_DATA)
	{
		// Draw a raw ASCII char.
		disp_data = send_data;
	}
	else
	{
		return HD44780_ERR_DATA;
	}
	// Write char to the buffer.
	ptr_input[disp_coord_x][disp_coord_y] = disp_data;
	// Move cursor
	disp_coord_x++;
	// Cycle cursor in display viewport.
	if(disp_coord_x>=HD44780_MAX_COL)
	{
		disp_coord_x = 0;
		disp_coord_y++;
		if(disp_coord_y>=HD44780_MAX_ROWS)
		{
			disp_coord_y = 0;
		}
	}
	disp_redraw_state = HD44780_NEEDS_UPDATE;
	return HD44780_OK;
}

//-------------------------------------- Write [uint8_t] as number to display buffer.
uint8_t HD44780_buf_write_8bit_number(const uint8_t send_number, const uint8_t send_mode)
{
	if(send_mode==HD44780_NUMBER_SPACES)
	{
		HD44780_buf_write_byte(send_number/100, HD44780_NONZERO_DIGIT);
		if(send_number<100)
			HD44780_buf_write_byte(send_number%100/10, HD44780_NONZERO_DIGIT);
		else
			HD44780_buf_write_byte(send_number%100/10, HD44780_DIGIT);
		HD44780_buf_write_byte(send_number%10, HD44780_DIGIT);
	}
	else if(send_mode==HD44780_NUMBER)
	{
		if(send_number>=100)
		{
			HD44780_buf_write_byte(send_number/100, HD44780_DIGIT);
			HD44780_buf_write_byte(send_number%100/10, HD44780_DIGIT);
			HD44780_buf_write_byte(send_number%10, HD44780_DIGIT);
		}
		else if(send_number>=10)
		{
			HD44780_buf_write_byte(send_number%100/10, HD44780_DIGIT);
			HD44780_buf_write_byte(send_number%10, HD44780_DIGIT);
		}
		else
		{
			HD44780_buf_write_byte(send_number%10, HD44780_DIGIT);
		}
	}
	else if(send_mode==HD44780_NUMBER_HEX)
	{
		HD44780_buf_write_byte(send_number/0x10, HD44780_HEX_DIGIT);
		HD44780_buf_write_byte(send_number%0x10, HD44780_HEX_DIGIT);
	}
	else
	{
		return HD44780_ERR_DATA;
	}
	return HD44780_OK;
}

//-------------------------------------- Write ASCII string from RAM to display buffer.
uint8_t HD44780_buf_write_string(uint8_t *str_output)
{
	uint8_t send_char, char_cnt;
	// Keep first char.
	send_char = *(str_output);
	char_cnt = 0;
	// Cycle while symbols left.
	while(send_char!=0x00)
	{
		// Next char.
		char_cnt++;
		if(char_cnt>=HD44780_MAX_COL)
		{
			// Counter overflow.
			return HD44780_ERR_DATA;
		}
		// Store char in buffer.
		HD44780_buf_write_byte(send_char, HD44780_DATA);
		// Go to the next byte in the memory.
		send_char = *(str_output+char_cnt);
	}
	return HD44780_OK;
}

//-------------------------------------- Write ASCII string from FLASH to display buffer.
uint8_t HD44780_buf_write_flash_string(const int8_t *str_output)
{
	uint8_t send_char, char_cnt;
	// Keep first char.
	send_char = pgm_read_byte_near(str_output);
	char_cnt = 0;
	// Cycle while symbols left.
	while(send_char!=0x00)
	{
		// Next char.
		char_cnt++;
		if(char_cnt>=HD44780_MAX_COL)
		{
			// Counter overflow.
			return HD44780_ERR_DATA;
		}
		// Store char in buffer.
		HD44780_buf_write_byte(send_char, HD44780_DATA);
		// Go to the next byte in the memory.
		send_char = pgm_read_byte_near(str_output+char_cnt);
	}
	return HD44780_OK;
}

//-------------------------------------- Fill selected line with spaces.
inline uint8_t HD44780_buf_clear_line(const uint8_t line_idx)
{
	uint8_t idx;
	if(line_idx>=HD44780_MAX_ROWS)
	{
		return HD44780_ERR_DATA;
	}
	HD44780_buf_set_position(0, line_idx);
	for(idx=0;idx<HD44780_MAX_COL;idx++)
	{
		HD44780_buf_write_byte(ASCII_SPACE, HD44780_DATA);
	}
	return HD44780_OK;
}
#endif	// HD44780_USE_SCREEN_BUFFERS

#endif /* CONF_EN_HD44780P */
