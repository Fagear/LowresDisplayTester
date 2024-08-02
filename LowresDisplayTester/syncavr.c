#include "syncavr.h"

volatile uint8_t tasks = 0;							// System tasks.
uint8_t usr_video = MODE_COMP_525i;					// Video system value set by user.
uint8_t disp_presence = 0;							// Flags for detected displays types.
uint8_t i2c_40_range = 0;							// Flags for I2C devices in 0x40...0x4E range.
uint8_t i2c_70_range = 0;							// Flags for I2C devices in 0x70...0x7E range.
uint8_t i2c_E0_range = 0;							// Flags for I2C devices in 0xE0...0xEE range.
uint8_t active_region = 0;							// Flags for lines in active region.
uint8_t act_delay = 0;								// Line active part start delay from H-sync (for clearing interrupt from IFs).
uint16_t sync_step_cnt = 0;							// Current step in sync generator logic.
uint16_t sync_step_limit = 0;						// Maximum step number for selected video system.
uint16_t frame_line_cnt = 0;						// Number of current line in the frame.
volatile uint8_t video_sys = MODE_COMP_625i;					// Video system value, used in interrupt (buffered).
uint8_t kbd_state = 0;								// Buttons states from the last [keys_simple_scan()] poll.
uint8_t kbd_pressed = 0;							// Flags for buttons that have been pressed (should be cleared after processing).
uint8_t kbd_released = 0;							// Flags for buttons that have been released (should be cleared after processing).
volatile uint8_t comp_data_idx = 0;					// Index for vertical bar groups.

volatile const uint8_t ucaf_compile_date[] PROGMEM = __DATE__;		// Date of compilation
volatile const uint8_t ucaf_compile_time[] PROGMEM = __TIME__;		// Time of compilation
volatile const uint8_t ucaf_version[] PROGMEM = "v0.11";			// Firmware version
volatile const uint8_t ucaf_author[] PROGMEM = "Maksim Kryukov aka Fagear (fagear@mail.ru)";	// Author
volatile const uint8_t ucaf_info[] PROGMEM = "ATmega sync gen/display tester";					// Firmware description

//-------------------------------------- Video sync timing (horizontal/composite), active region timing management (each H-line).
ISR(SYNC_INT)
{
	/*if(frame_line_cnt==(dbg_index-1))
	{
		// Produce pulse for selected video line.
		DBG_4_ON;
		__builtin_avr_delay_cycles(1);
		DBG_4_OFF;
	}*/
	// Stabilize active region.
	//DBG_2_ON;
	while(SYNC_DATA_8B<H_STBL_DELAY) {};
	//DBG_2_OFF;
	// Start line active part end timer.
	//LACT_START; LACT_DATA = act_delay;

	// Check if current line is in vertical active region.
	if((active_region&ACT_RUN)!=0)
	{
		// Enable active part of the line highlighting.
		HACT_ON;
	}
	else
	{
		// Disable active part of the line highlighting.
		HACT_OFF;
		// Disable line active part interrupt inside VBI.
		LACT_DIS_INTR;
	}

	// Line-by-line sync and timing management.
	if(video_sys==MODE_VGA_60Hz)
	{
		// VGA 640x480@60
		// First, process vertical sync.
		if(sync_step_cnt==VGA_VS_START)
		{
			// Start vertical sync pulse.
			VSYNC_PULL_DOWN;
		}
		else if(sync_step_cnt==VGA_VS_STOP)
		{
			// End vertical sync pulse.
			VSYNC_PULL_UP;
		}
		// Continue with H-stepping.
		else if(sync_step_cnt==VGA_BP_STOP)
		{
			// Enable drawing in active region.
			active_region |= (ACT_RUN|ACT_RGB_LINES);
		}
		else if(sync_step_cnt==VGA_FP_START)
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
	}
	else if(video_sys==MODE_EGA)
	{
		// CGA/EGA 60Hz
		// First, process vertical sync.
		if(sync_step_cnt==EGA_VS_START)
		{
			// Start vertical sync pulse.
			VSYNC_PULL_UP;
		}
		else if(sync_step_cnt==EGA_VS_STOP)
		{
			// End vertical sync pulse.
			VSYNC_PULL_DOWN;
		}
		// Continue with H-stepping.
		else if(sync_step_cnt==EGA_BP_STOP)
		{
			// Enable drawing in active region.
			active_region |= (ACT_RUN|ACT_MN_LINES);
		}
		else if(sync_step_cnt==EGA_FP_START)
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
	}
	else if(video_sys==MODE_COMP_525i)
	{
		// 525i
		if(sync_step_cnt<=ST_COMP525_F1_EQ_STOP)
		{
			// First field sync.
			if((sync_step_cnt%2)!=0)
			{
				// When on frame sync, divide line count by two (on even lines).
				frame_line_cnt++;
			}
			if(sync_step_cnt==ST_COMP525_F1_EQ_START)
			{
				// Pre-load equalizing pulse configuration one line before.
				SYNC_STEP_DUR = COMP_HALF_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_VS_START)
			{
				// Frame (vertical) sync.
				SYNC_PULSE_DUR = COMP_SYNC_V_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_VS_PLS1)
			{
				// Optional vertical sync at dedicated pin.
				VSYNC_PULL_DOWN;
			}
			else if(sync_step_cnt==ST_COMP525_F1_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_VS_PLS2)
			{
				// Optional vertical sync (end) at dedicated pin.
				VSYNC_PULL_UP;
			}
			else if(sync_step_cnt==ST_COMP525_F1_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				SYNC_PULSE_DUR = COMP_SYNC_H_LEN_525i;
			}
		}
		else if((sync_step_cnt>=ST_COMP525_F2_EQ_START)&&(sync_step_cnt<ST_COMP525_F2_EQ_STOP))
		{
			// Second field sync.
			if((sync_step_cnt%2)==0)
			{
				// When on frame sync, divide line count by two (on odd lines).
				frame_line_cnt++;
			}
			if(sync_step_cnt==ST_COMP525_F2_EQ_START)
			{
				// Pre-equalizing pulses.
				SYNC_STEP_DUR = COMP_HALF_LEN_525i;
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_VS_START)
			{
				// Frame (vertical) sync.
				SYNC_PULSE_DUR = COMP_SYNC_V_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_VS_PLS1)
			{
				// Optional vertical sync at dedicated pin.
				VSYNC_PULL_DOWN;
			}
			else if(sync_step_cnt==ST_COMP525_F2_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_VS_PLS2)
			{
				// Optional vertical sync (end) at dedicated pin.
				VSYNC_PULL_UP;
			}
		}
		else
		{
			// Normal line period.
			if(sync_step_cnt==(ST_COMP525_F1_EQ_STOP+1))
			{
				// Normal line (horizontal) sync pulse period.
				SYNC_STEP_DUR = COMP_LINE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				SYNC_STEP_DUR = COMP_LINE_LEN_525i;
				// Delay normal width sync pulse one line.
				SYNC_PULSE_DUR = COMP_SYNC_H_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_LOOP)
			{
				// Pre-load equalizing pulse configuration one line before.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			// When not on frame sync, count step as line.
			frame_line_cnt++;
		}
		// Manage active region via line number.
		if((frame_line_cnt==ST_COMP525_F1_ACT_START)||(frame_line_cnt==ST_COMP525_F2_ACT_START))
		{
			// Enable drawing in active region.
			active_region |= (ACT_RUN|ACT_MN_LINES);
		}
		else if((frame_line_cnt==ST_COMP525_F1_ACT_STOP)||(frame_line_cnt==ST_COMP525_F2_ACT_STOP))
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
	}
	else if(video_sys==MODE_COMP_625i)
	{
		// 625i
		if((sync_step_cnt>=ST_COMP625_F2_EQ_START)||(sync_step_cnt<=ST_COMP625_F1_EQ_STOP))
		{
			// First field vertical sync.
			if(sync_step_cnt==ST_COMP625_F1_VS_PLS1)
			{
				// Optional vertical sync at dedicated pin.
				VSYNC_PULL_DOWN;
			}
			else if(sync_step_cnt==ST_COMP625_F1_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F1_VS_PLS2)
			{
				// Optional vertical sync (end) at dedicated pin.
				VSYNC_PULL_UP;
			}
			else if(sync_step_cnt==ST_COMP625_F1_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				SYNC_PULSE_DUR = COMP_SYNC_H_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_EQ_START)
			{
				// Pre-equalizing pulses.
				SYNC_STEP_DUR = COMP_HALF_LEN_625i;
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_LOOP)
			{
				// Pre-load half-line configuration one line before the vertical sync.
				SYNC_PULSE_DUR = COMP_SYNC_V_LEN_625i;
			}
			if((sync_step_cnt%2)!=0)
			{
				// When on frame sync, divide line count by two (on even lines).
				frame_line_cnt++;
			}
		}
		else if((sync_step_cnt>ST_COMP625_F1_EQ_START)&&(sync_step_cnt<ST_COMP625_F2_EQ_STOP))
		{
			// Second field vertical sync.
			if(sync_step_cnt==(ST_COMP625_F1_EQ_START+1))
			{
				// Pre-equalizing pulses.
				SYNC_STEP_DUR = COMP_HALF_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_VS_START)
			{
				// Frame (vertical) sync.
				SYNC_PULSE_DUR = COMP_SYNC_V_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_VS_PLS1)
			{
				// Optional vertical sync at dedicated pin.
				VSYNC_PULL_DOWN;
			}
			else if(sync_step_cnt==ST_COMP625_F2_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_VS_PLS2)
			{
				// Optional vertical sync (end) at dedicated pin.
				VSYNC_PULL_UP;
			}
			if((sync_step_cnt%2)==0)
			{
				// When on frame sync, divide line count by two (on odd lines).
				frame_line_cnt++;
			}
		}
		else
		{
			// Normal line period.
			if(sync_step_cnt==(ST_COMP625_F1_EQ_STOP+1))
			{
				// Normal line (horizontal) sync pulse period.
				SYNC_STEP_DUR = COMP_LINE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F1_EQ_START)
			{
				// Pre-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_EQ_STOP)
			{
				// Delay normal width sync pulse one line.
				SYNC_STEP_DUR = COMP_LINE_LEN_625i;
				SYNC_PULSE_DUR = COMP_SYNC_H_LEN_625i;
			}
			// When not on frame sync, count step as line.
			frame_line_cnt++;
		}
		// Manage active region via line number.
		if((frame_line_cnt==ST_COMP625_F1_ACT_START)||(frame_line_cnt==ST_COMP625_F2_ACT_START))
		{
			// Enable drawing in active region.
			active_region |= (ACT_RUN|ACT_MN_LINES);
		}
		else if((frame_line_cnt==ST_COMP625_F1_ACT_STOP)||(frame_line_cnt==ST_COMP625_F2_ACT_STOP))
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
	}

	// Go through steps.
	tasks &= ~TASK_UPDATE_ASYNC;
	sync_step_cnt++;
	if(sync_step_cnt>sync_step_limit)
	{
		// New frame starts.
		//DBG_1_ON;
		// Update video system to user's choice.
		video_sys = usr_video;
		// Set new maximum step count and pulse polarity for selected video mode.
		if(video_sys==MODE_VGA_60Hz)
		{
			// Negative pulse horizontal sync for VGA.
			SYNC_CONFIG_NEG;
			sync_step_limit = LINES_VGA;
			// Horizontal sync pulse period/length.
			SYNC_STEP_DUR = VGA_LINE_LEN;
			SYNC_PULSE_DUR = VGA_SYNC_H_LEN;
			// Active part length.
			HACT_PULSE_DUR = VGA_ACT_DELAY;
		}
		else if(video_sys==MODE_EGA)
		{
			// Positive pulse horizontal sync for CGA/EGA.
			SYNC_CONFIG_POS;
			sync_step_limit = LINES_EGA;
			// Horizontal sync pulse period/length.
			SYNC_STEP_DUR = EGA_LINE_LEN;
			SYNC_PULSE_DUR = EGA_SYNC_H_LEN;
			// Active part length.
			HACT_PULSE_DUR = EGA_ACT_DELAY;
			//LACT_PULSE_DUR = 254; act_delay = 148;
		}
		else if(video_sys==MODE_COMP_525i)
		{
			// Negative pulse composite sync for 525i30.
			SYNC_CONFIG_NEG;
			sync_step_limit = ST_COMP525_LOOP;
			// Active part length.
			HACT_PULSE_DUR = COMP_ACT_DELAY_525i;
			//LACT_PULSE_DUR = 254; act_delay = 148;
		}
		else if(video_sys==MODE_COMP_625i)
		{
			// Negative pulse composite sync for 625i25.
			SYNC_CONFIG_NEG;
			sync_step_limit = ST_COMP625_LOOP;
			// Active part length.
			HACT_PULSE_DUR = COMP_ACT_DELAY_625i;
			//LACT_PULSE_DUR = 254; act_delay = 149;
		}
		// Loop line counter within one frame.
		frame_line_cnt = 0;
		sync_step_cnt = 0;
		active_region = 0;
		// Set flag for slow events in the main loop.
		tasks |= TASK_UPDATE_ASYNC;
		//DBG_1_OFF;
	}
	// Reset bar index.
	comp_data_idx = 0;
	// Start drawing patterns.
	if((active_region&ACT_MN_LINES)!=0)
	{
		// Stabilize bar drawing.
		while(SYNC_DATA_8B<A_MONO_STBL_DELAY) {};
		//DBG_3_ON;
		// Set minimal frequency for first set of vertical bars.
		//SPI_set_target_clock(BAR_FREQ_500Hz);
		SPI_CONTROL &= ~(1<<SPR0);
		SPI_CONTROL |= (1<<SPR1);
		SPI_STATUS = (1<<SPI2X);
		// Start drawing first set of vertical bars.
		SPI_DATA = SPI_DUMMY_SEND;
		//DBG_3_OFF;
	}
	else if((active_region&ACT_RGB_LINES)!=0)
	{
		// Stabilize bar drawing.
		while(SYNC_DATA_8B<A_RGB_STBL_DELAY) {};
		//DBG_3_ON;
		//SPI_set_target_clock(BAR_FREQ_1MHz);
		SPI_CONTROL |= (1<<SPR0);
		SPI_CONTROL &= ~(1<<SPR1);
		SPI_STATUS &= ~(1<<SPI2X);
		// Start drawing first set of vertical bars.
		SPI_DATA = SPI_DUMMY_SEND;
		//DBG_3_OFF;
	}
}

//-------------------------------------- Horizontal line active part timing (turn off "active" signal).
ISR(LACT_COMP_INT)
{
	//DBG_3_ON;

	// Turn off active part output.
	HACT_OFF;

	//DBG_3_OFF;
}

//-------------------------------------- Horizontal line active part timing (re-arm "active" signal).
ISR(LACT_OVF_INT)
{
	//DBG_4_ON;

	// Timer self-shutdown.
	LACT_STOP;

	if(active_region!=0)
	{
		// Re-arm active part output.
		HACT_ON;
	}

	//DBG_4_OFF;
}

#ifdef FGR_DRV_SPI_HW_FOUND
//-------------------------------------- Vertical bars batch.
ISR(SPI_INT)
{
	// Step through series of vertical bars.
	comp_data_idx++;
	if((active_region&ACT_MN_LINES)!=0)
	{
		if(comp_data_idx==1)
		{
			// 1 MHz.
			//SPI_set_target_clock(BAR_FREQ_1MHz);
			SPI_CONTROL |= (1<<SPR0);
			SPI_CONTROL &= ~(1<<SPR1);
			SPI_STATUS &= ~(1<<SPI2X);
			// Send a byte to generate clock for bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}
		else if(comp_data_idx==2)
		{
			// 2 MHz.
			//SPI_set_target_clock(BAR_FREQ_2MHz);
			SPI_CONTROL |= (1<<SPR0);
			SPI_CONTROL &= ~(1<<SPR1);
			SPI_STATUS |= (1<<SPI2X);
			// Send a byte to generate clock for bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}
	}
	/*else if((active_region&ACT_RGB_LINES)!=0)
	{
		if(comp_data_idx==1)
		{
			// 2 MHz.
			//SPI_set_target_clock(BAR_FREQ_2MHz);
			SPI_CONTROL |= (1<<SPR0);
			SPI_CONTROL &= ~(1<<SPR1);
			SPI_STATUS |= (1<<SPI2X);
			// Send a byte to generate clock for bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}
	}*/
}
#endif /* FGR_DRV_SPI_HW_FOUND */

#ifdef FGR_DRV_I2C_HW_FOUND
ISR(I2C_INT, ISR_NAKED)
{
	INTR_IN;
	I2C_INT_DIS;
	tasks |= TASK_I2C;
	//HD44780_write_string((uint8_t *)"INT|");
	INTR_OUT;
}
#endif /* FGR_DRV_I2C_HW_FOUND */

#ifdef FGR_DRV_UART_HW_FOUND
ISR(UART_RX_INT, ISR_NAKED)
{
	//INTR_UART_IN;
	INTR_IN;
	tasks |= TASK_UART_RX;
	INTR_OUT;
	//INTR_UART_OUT;
}

ISR(UART_TX_INT, ISR_NAKED)
{
	//INTR_UART_IN;
	INTR_IN;
	tasks |= TASK_UART_TX;
	INTR_OUT;
	//INTR_UART_OUT;
}
#endif /* FGR_DRV_UART_HW_FOUND */

//-------------------------------------- Startup init.
inline void system_startup(void)
{
	// Shut down watchdog.
	// Prevent mis-configured watchdog loop reset.
	wdt_reset();
	cli();
	WDT_RESET_DIS;
	WDT_PREP_OFF;
	WDT_SW_OFF;
	WDT_FLUSH_REASON;
	// Enable watchdog.
	WDT_PREP_ON;
	WDT_SW_ON;
	wdt_reset();

	// Init hardware resources.
	HW_init();
	// Preload some values to sync timer.
	SYNC_STEP_DUR = COMP_HALF_LEN_525i;			// Load duration of the H-cycle.
	SYNC_PULSE_DUR = COMP_SYNC_V_LEN_525i;		// Load duration of the H-pulse.
	SYNC_DATA = 0;
	LACT_DATA = 0;
#ifdef FGR_DRV_UART_HW_FOUND
	UART_enable();
	UART_set_speed(UART_BAUD_9600);
	UART_TX_INT_EN; UART_RX_INT_EN;
#endif /* FGR_DRV_UART_HW_FOUND */
#ifdef FGR_DRV_UARTSPI_HW_FOUND
	//UART_SPI_CONFIG_M0;
	//UART_SPI_set_target_clock(BAR_FREQ_500Hz);
	//UART_SPI_START;
#endif /* FGR_DRV_UARTSPI_HW_FOUND */
#ifdef FGR_DRV_SPI_HW_FOUND
	SPI_CONFIG_MSTR_M0;
	//SPI_set_target_clock(BAR_FREQ_500Hz);
	SPI_CONTROL &= ~(1<<SPR0);
	SPI_CONTROL |= (1<<SPR1);
	SPI_STATUS = (1<<SPI2X);
	SPI_START; SPI_INT_EN;
#endif /* FGR_DRV_SPI_HW_FOUND */
#ifdef FGR_DRV_I2C_HW_FOUND
	I2C_set_speed_100kHz();
	I2C_INIT_MASTER;
#endif /* FGR_DRV_I2C_HW_FOUND */
}

//-------------------------------------- Keyboard scan routine.
void keys_simple_scan(void)
{
	// Check user input.
	if(BTN_1_STATE==0)
	{
		// Button is pressed.
		// Compare with previous state.
		if((kbd_state&SW_VID_SYS0)==0)
		{
			// Button was released before, now it is pressed.
			kbd_state|=SW_VID_SYS0;
			kbd_pressed|=SW_VID_SYS0;
		}
	}
	else
	{
		// Button is released.
		// Compare with previous state.
		if((kbd_state&SW_VID_SYS0)!=0)
		{
			// Button was pressed before, now it is released.
			kbd_state&=~SW_VID_SYS0;
			kbd_released|=SW_VID_SYS0;
		}
	}
	if(BTN_2_STATE==0)
	{
		// Button is pressed.
		// Compare with previous state.
		if((kbd_state&SW_VID_SYS1)==0)
		{
			// Button was released before, now it is pressed.
			kbd_state|=SW_VID_SYS1;
			kbd_pressed|=SW_VID_SYS1;
		}
	}
	else
	{
		// Button is released.
		// Compare with previous state.
		if((kbd_state&SW_VID_SYS1)!=0)
		{
			// Button was pressed before, now it is released.
			kbd_state&=~SW_VID_SYS1;
			kbd_released|=SW_VID_SYS1;
		}
	}
}

//-------------------------------------- Select video standard by switches.
uint8_t select_video_std(void)
{
	uint8_t fps;
	if((kbd_state&SW_VID_SYS0)!=0)
	{
		if((kbd_state&SW_VID_SYS1)!=0)
		{
			// Select CGA/EGA 60Hz.
			usr_video = MODE_EGA;
			fps = FPS_EGA;
		}
		else
		{
			// Select VGA 640x480@60.
			usr_video = MODE_VGA_60Hz;
			fps = FPS_VGA;
		}
	}
	else
	{
		if((kbd_state&SW_VID_SYS1)==0)
		{
			// Select composite 525i.
			usr_video = MODE_COMP_525i;
			fps = FPS_COMP525;
		}
		else
		{
			// Select composite 625i.
			usr_video = MODE_COMP_625i;
			fps = FPS_COMP625;
		}
	}
	return fps;
}

//====================================== MAIN FUNCTION.
int main(void)
{
	// Start-up initialization.
	system_startup();

	uint8_t div_sec = 0;
	uint8_t tasks_buf = 0;
	uint8_t seconds_top = FPS_COMP525;
#ifdef FGR_DRV_I2C_HW_FOUND
	uint8_t i2c_addr = 0x02;
#endif
	//dbg_index = 1;
	//dbg_index = 305;
	//dbg_index = 315;
	//dbg_index = 620;

#ifdef CONF_EN_HD44780P
	// Parallel HD44780 display initialization.
	HD44780_setup(HD44780_RES_16X2, HD44780_CYR_NOCONV);
	if(HD44780_init()==HD44780_OK)
	{
		disp_presence |= HW_DISP_44780;
		hd44780_set_device(HD44780_write_command_byte, HD44780_write_data_byte, HD44780_read_byte);
		hd44780_set_xy_position(0, 0);
		hd44780_write_string((uint8_t *)"Display TEST|", NULL);
	}
#endif /* CONF_EN_HD44780P */

	// Enable interrupts globally.
	sei();
	// Later enabling of sync interrupt makes more straight bars.
	SYNC_INT_EN;
	// Main cycle.
	while(1)
	{
		tasks_buf |= tasks;
		//cli();
		//tasks = 0;
		//sei();
		if((tasks_buf&TASK_UPDATE_ASYNC)!=0)
		{
			tasks_buf &= ~TASK_UPDATE_ASYNC;
			//DBG_4_ON;
			// Scan switches with debounce.
			keys_simple_scan();
			// Update video standard.
			seconds_top = select_video_std();
			// One second tick.
			div_sec++;
			if(div_sec>=seconds_top)
			{
				div_sec = 0;
				// Reset watchdog.
				wdt_reset();
				// Second tick.
				tasks_buf |= TASK_SEC_TICK;
#ifdef CONF_EN_HD44780P
				// Check if HD44780-compatible display is detected.
				if((disp_presence&HW_DISP_44780)==0)
				{
					// No display found, try to re-init it.
					if(HD44780_init()==HD44780_OK)
					{
						// HD44780-compatible display found on parallel bus.
						disp_presence |= HW_DISP_44780;
						// Restart test sequence.
						chardisp_reset_anim();
					}
					else
					{
						disp_presence &= ~HW_DISP_44780;
					}
				}
#endif /* CONF_EN_HD44780P */
#ifdef CONF_EN_I2C
				if((kbd_state&SW_VID_SYS0)!=0)
				{
					if((tasks_buf&TASK_I2C_SCAN)==0)
					{
						tasks_buf |= TASK_I2C_SCAN;
						// Reset I2C address.
						i2c_addr = 0x02;
						// Reset presence flags.
						i2c_40_range = i2c_70_range = i2c_E0_range = 0;
						disp_presence &= ~(HW_DISP_I2C_40|HW_DISP_I2C_70|HW_DISP_I2C_78|HW_DISP_I2C_7A|HW_DISP_I2C_7C|HW_DISP_I2C_E0);
					}
				}
#endif /* CONF_EN_I2C */
			}
#ifdef CONF_EN_HD44780P
			// Check if HD44780-compatible display is detected.
			if((disp_presence&HW_DISP_44780)!=0)
			{
				uint8_t err_mask = 0;
				// Setup general HD44780 operations.
				hd44780_set_device(HD44780_write_command_byte,
									HD44780_write_data_byte,
									HD44780_read_byte);
				// Setup character display test module.
				chardisp_set_device(hd44780_upload_symbol_flash,
									hd44780_set_xy_position,
									HD44780_write_data_byte,
									hd44780_write_flash_string);
				// Step animation forward.
				err_mask = chardisp_step_animation();
				if(err_mask!=CHTST_RES_OK)
				{
					// Seems like display fell of the bus.
					disp_presence &= ~HW_DISP_44780;
				}
				// Check if animation cycle has finished.
				if(chardisp_cycle_done()==ST_ANI_DONE)
				{
					// Check display hardware.
					if(hd44780_selftest()!=HD44780_OK)
					{
						// Seems like display fell of the bus.
						disp_presence &= ~HW_DISP_44780;
					}
				}
			}
#endif /* CONF_EN_HD44780P */
#ifdef CONF_EN_HD44780S
			if((disp_presence&HW_DISP_I2C_78)!=0)
			{
				uint8_t err_mask = 0;
				HD44780s_set_address(I2C_US2066_ADR1);
				// Setup general HD44780 operations.
				hd44780_set_device(HD44780s_write_command_byte,
									HD44780s_write_data_byte,
									NULL);
				// Setup character display test module.
				chardisp_set_device(hd44780_upload_symbol_flash,
									hd44780_set_xy_position,
									HD44780s_write_data_byte,
									hd44780_write_flash_string);
				// Step animation forward.
				err_mask = chardisp_step_animation(tasks_buf&TASK_SEC_TICK);
				if(err_mask!=CHTST_RES_OK)
				{
					// Seems like display fell of the bus.
					disp_presence &= ~HW_DISP_I2C_78;
				}
				// Check if animation cycle has finished.
				if(chardisp_cycle_done()==ST_ANI_DONE)
				{
					// Check display hardware.
					tasks_buf |= TASK_I2C_SCAN;
				}
			}
#endif /* CONF_EN_HD44780S */
			// Clear processed tasks.
			tasks_buf &= ~TASK_SEC_TICK;
			//DBG_4_OFF;
		}
#ifdef CONF_EN_I2C
		// Check if device scan is in progress.
		if((tasks_buf&TASK_I2C_SCAN)!=0)
		{
			// Check if I2C is busy transmitting something.
			if(I2C_write_data(i2c_addr, 0, NULL)==I2C_ACCEPT)
			{
				// Initiate write to a new address.
				tasks_buf |= TASK_I2C;
				i2c_addr+=2;
				if(i2c_addr>=I2C_RESERVED)
				{
					// Stop device scan.
					tasks_buf &= ~TASK_I2C_SCAN;
				}
			}
		}
		if((tasks_buf&TASK_I2C)!=0)
		{
			tasks_buf &= ~TASK_I2C;
			I2C_master_processor();
			if(I2C_is_busy()==I2C_MODE_IDLE)
			{
				// No transmittion in progress.
				if((tasks_buf&TASK_I2C_SCAN)!=0)
				{
					// Device scan in progress.
					uint8_t i2c_err, i2c_addr;
					i2c_err = I2C_get_error();
					i2c_addr = I2C_get_write_address();
					if(i2c_err==I2C_ERR_NO_DONE)
					{
						// Print address of detected slave device.
#ifdef CONF_EN_HD44780P
						hd44780_write_8bit_number(i2c_addr, HD44780_NUMBER_HEX); hd44780_write_string((uint8_t *)"~", NULL);
#endif
						if((i2c_addr>=I2C_PCF8574_START)&&(i2c_addr<=I2C_PCF8574_END))
						{
							disp_presence |= HW_DISP_I2C_40;
							i2c_40_range |= (1<<((i2c_addr>>1)&0x07));
						}
						else if((i2c_addr>=I2C_PCF8574A_START)&&(i2c_addr<=I2C_PCF8574A_END))
						{
							disp_presence |= HW_DISP_I2C_70;
							i2c_70_range |= (1<<((i2c_addr>>1)&0x07));
						}
						else if((i2c_addr>=I2C_HT16K33_START)&&(i2c_addr<=I2C_HT16K33_END))
						{
							disp_presence |= HW_DISP_I2C_E0;
							i2c_E0_range |= (1<<((i2c_addr>>1)&0x07));
						}
						else if(i2c_addr==I2C_US2066_ADR1)
						{
							disp_presence |= HW_DISP_I2C_78;
						}
						else if(i2c_addr==I2C_US2066_ADR2)
						{
							disp_presence |= HW_DISP_I2C_7A;
						}
						else if(i2c_addr==I2C_ST7032I_ADR)
						{
							disp_presence |= HW_DISP_I2C_7C;
						}
					}
					else if(i2c_err!=I2C_ERR_M_ADR_NACK)
					{
						// Stop device scan.
						tasks_buf &= ~TASK_I2C_SCAN;
					}
				}
			}
		}
#endif /* CONF_EN_I2C */
	}

	return 0;
}
