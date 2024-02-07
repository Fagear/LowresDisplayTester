#include "syncavr.h"

volatile uint8_t tasks = 0;							// System tasks.
uint8_t usr_video = MODE_COMP_625i;					// Video system value set by user.
uint8_t usr_act_delay = COMP_ACT_DELAY_625i;		// Line active part start delay from H-sync for user-selected video system.
uint8_t usr_act_time = COMP_ACT_LEN_625i;			// Line active part duration for user-selected video system.
uint8_t disp_presence = 0;							// Flags for detected displays types
uint8_t active_timer_mode = LACT_ST_IDLE;			// Mode of the line active part timer.
uint8_t active_region = 0;							// Flags for lines in active region.
volatile uint8_t act_delay = COMP_ACT_DELAY_625i;	// Line active part start delay from H-sync (for clearing interrupt from IFs).
volatile uint8_t act_time = COMP_ACT_LEN_625i;		// Line active part duration (for clearing interrupt from IFs).
uint16_t sync_step_cnt = 0;							// Current step in sync generator logic.
volatile uint16_t sync_step_limit = 0;				// Maximum step number for selected video system.
uint16_t frame_line_cnt = 0;						// Number of current line in the frame.
uint8_t video_sys = MODE_COMP_625i;					// Video system value, used in interrupt (buffered).
uint8_t kbd_state = 0;								// Buttons states from the last [keys_simple_scan()] poll.
uint8_t kbd_pressed = 0;							// Flags for buttons that have been pressed (should be cleared after processing).
uint8_t kbd_released = 0;							// Flags for buttons that have been released (should be cleared after processing).
volatile uint8_t comp_data_idx = 0;					// Index for vertical bar groups.
uint16_t dbg_index = 0;

volatile const uint8_t ucaf_compile_time[] PROGMEM = __TIME__;		// Time of compilation
volatile const uint8_t ucaf_compile_date[] PROGMEM = __DATE__;		// Date of compilation
volatile const uint8_t ucaf_version[] PROGMEM = "v0.05";			// Firmware version
volatile const uint8_t ucaf_author[] PROGMEM = "Maksim Kryukov aka Fagear (fagear@mail.ru)";	// Author
volatile const uint8_t ucaf_info[] PROGMEM = "ATmega sync gen/display tester";					// Firmware description

//-------------------------------------- Video sync timing (horizontal/composite), active region timing management (each H-line).
ISR(INT0_INT)
{	
	/*if(frame_line_cnt==(dbg_index-1))
	{
		// Produce pulse for selected video line.
		DBG_5_ON;
		__builtin_avr_delay_cycles(1);
		DBG_5_OFF;
	}*/
	DBG_2_ON;
	LACT_STOP;
	LACT_DATA = 0;
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
	// Check if current line is in vertical active region.
	if(active_region!=0)
	{
		// Start line active region timer.
		LACT_START;
		// Preload time for next active part waiting.
		LACT_PULSE_DUR = act_delay;
		// Enable interrupt to re-tune timer for active part duration.
		LACT_EN_INTR;
		// Switch to "Toggle OC pin on compare" to allow pin to be switched HIGH on the next compare after restart.
		LACT_OC_TOGGLE1; LACT_OC_TOGGLE2;
		//active_timer_mode = LACT_ST_WAIT_ACT;
	}
#endif
	if(video_sys==MODE_VGA_60Hz)
	{
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
	}
	if(video_sys==MODE_COMP_625i)
	{
		// 625i
		if((sync_step_cnt>=ST_COMP625_F2_EQ_START)||(sync_step_cnt<=ST_COMP625_F1_EQ_STOP))
		{
			// First field vertical sync.
			if(sync_step_cnt==ST_COMP625_F1_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
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
			else if(sync_step_cnt==ST_COMP625_F2_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
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
		// Set maximum step count.
		sync_step_limit = ST_COMP625_LOOP;
	}
	else if(video_sys==MODE_COMP_525i)
	{
		// 525i
		if(sync_step_cnt<=ST_COMP525_F1_EQ_STOP)
		{
			//DBG_3_ON;
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
			else if(sync_step_cnt==ST_COMP525_F1_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				SYNC_PULSE_DUR = COMP_SYNC_H_LEN_525i;
			}
			//DBG_3_OFF;
		}
		else if((sync_step_cnt>=ST_COMP525_F2_EQ_START)&&(sync_step_cnt<ST_COMP525_F2_EQ_STOP))
		{
			//DBG_3_ON;
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
			else if(sync_step_cnt==ST_COMP525_F2_VS_STOP)
			{
				// Post-equalizing pulses.
				SYNC_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			//DBG_3_OFF;
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
		// Set maximum step count.
		sync_step_limit = ST_COMP525_LOOP;
	}
	else if(video_sys==MODE_VGA_60Hz)
	{
		// VGA 640x480
		if(sync_step_cnt==VGA_HS_START)
		{
			// Start vertical sync pulse.
			//VSYNC_PULL_DOWN;
			// Horizontal sync pulse period/length.
			SYNC_STEP_DUR = VGA_LINE_LEN;
			SYNC_PULSE_DUR = VGA_SYNC_H_LEN;
		}
		/*if(sync_step_cnt==VGA_VS_STOP)
		{
			// End vertical sync pulse.
			VSYNC_PULL_UP;
		}
		else */if(sync_step_cnt==VGA_BP_STOP)
		{
			// Enable drawing in active region.
			active_region |= (ACT_RUN|ACT_RGB_LINES);
		}
		else if(sync_step_cnt==VGA_FP_START)
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
		// Set maximum step count.
		sync_step_limit = LINES_VGA;
	}
	// Go through steps.
	sync_step_cnt++;
	if(sync_step_cnt>sync_step_limit)
	{
		// New frame starts.
		DBG_1_ON;
		// Loop line counter within one frame.
		frame_line_cnt = 0;
		sync_step_cnt = 0;
		tasks |= TASK_UPDATE_ASYNC;
		// Update video system to user's choice.
		video_sys = usr_video;
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
		// Apply delay for the active part for the H-sync.
		act_delay = usr_act_delay;
		// Apply length of the active part of the line.
		act_time = usr_act_time;
#endif
		DBG_1_OFF;
	}
	DBG_2_OFF;
}

#ifdef FGR_DRV_IO_T0OC_HW_FOUND
//-------------------------------------- Horizontal line active part timing.
ISR(LACT_COMP_INT)
{
	DBG_4_ON;
	//if(active_timer_mode==LACT_ST_WAIT_ACT)
	{
		// Active part of the line begins.
		//active_timer_mode = LACT_ST_ACT;
		// Disable interrupt for the end of the line.
		LACT_DIS_INTR;
		// Load duration of the active part.
		LACT_PULSE_DUR = act_time;
		// Switch to "Clear OC pin on compare" to ensure that OC pin will be switched OFF when timer runs out.
		LACT_OC_CLEAR1; LACT_OC_CLEAR2;
#ifdef FGR_DRV_SPI_HW_FOUND
		// Start drawing patterns.
		comp_data_idx = 0;
		if((active_region&ACT_MN_LINES)!=0)
		{
			// Set minimal frequency for first set of vertical bars.
			//SPI_set_target_clock(BAR_FREQ_500Hz);
			SPI_CONTROL &= ~(1<<SPR0);
			SPI_CONTROL |= (1<<SPR1);
			SPI_STATUS = (1<<SPI2X);
			// Start drawing first set of vertical bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}
		else if((active_region&ACT_RGB_LINES)!=0)
		{
			//SPI_set_target_clock(BAR_FREQ_4MHz);
			SPI_CONTROL &= ~((1<<SPR1)|(1<<SPR0));
			SPI_STATUS &= ~(1<<SPI2X);
			// Start drawing first set of vertical bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}
#endif
	}
	/*else if(active_timer_mode==LACT_ST_ACT)
	{
		DBG_3_ON;
		// Active part of the line ended.
		active_timer_mode = LACT_ST_IDLE;
		// Stop and reset this timer, wait for another start from HSYNC.
		LACT_STOP;
		LACT_DATA = 0;
		// Switch to "Toggle OC pin on compare" to allow pin to be switched HIGH on the next compare after restart.
		LACT_OC_TOGGLE1; LACT_OC_TOGGLE2;
		// Preload time for next active part waiting.
		LACT_PULSE_DUR = act_delay;
		// Check for Vsync condition before next Hsync.
		if(video_sys==MODE_VGA_60Hz)
		{
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
		}
		DBG_3_OFF;
	}*/
	// Allow timer overflow interrupt to activate after this one.
	//SYNC_EN_INTR;
	DBG_4_OFF;
}
#endif

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
		else if(comp_data_idx==3)
		{
			// 4 MHz.
			//SPI_set_target_clock(BAR_FREQ_4MHz);
			SPI_CONTROL &= ~((1<<SPR1)|(1<<SPR0));
			SPI_STATUS &= ~(1<<SPI2X);
			// Send a byte to generate clock for bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}
		/*else if(comp_data_idx==4)
		{
			// 8 MHz.
			//SPI_set_target_clock(BAR_FREQ_8MHz);
			SPI_CONTROL = (1<<MSTR);
			SPI_STATUS = (1<<SPI2X);
			// Send a byte to generate clock for bars.
			SPI_DATA = SPI_DUMMY_SEND;
		}*/
	}
	/*else if((active_region&ACT_RGB_LINES)!=0)
	{
		
	}*/
}
#endif /* FGR_DRV_SPI_HW_FOUND */

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
	
	// Init hardware resources.
	HW_init();
	// Preload some values to sync timer.
	SYNC_STEP_DUR = COMP_HALF_LEN_625i;			// Load duration of the H-cycle.
	SYNC_PULSE_DUR = COMP_SYNC_V_LEN_625i;		// Load duration of the H-pulse.
	
	// Enable watchdog.
	WDT_PREP_ON;
	WDT_SW_ON;
	wdt_reset();
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

//====================================== MAIN FUNCTION. 
int main()
{
	// Start-up initialization.
	system_startup();

	uint8_t div_sec = 0;
	uint8_t seconds_top = FPS_COMP625;
	
	//dbg_index = 1;
	//dbg_index = 305;
	//dbg_index = 315;
	dbg_index = 620;
	
#ifdef CONF_EN_HD44780
	uint8_t err_mask = 0;
	HD44780_setup(HD44780_RES_16X2, HD44780_CYR_NOCONV);
// 	if(HD44780_init()==HD44780_OK)
// 	{
// 		HD44780_set_xy_position(0, 0);
// 		HD44780_write_string("Display TEST");
// 		HD44780_set_xy_position(0, 1);
// 		HD44780_write_number(123, HD44780_NUMBER);
// 	}
#endif /* CONF_EN_HD44780 */
	
#ifdef FGR_DRV_UARTSPI_HW_FOUND
	UART_SPI_CONFIG_M0;
	UART_SPI_set_target_clock(BAR_FREQ_500Hz);
	UART_SPI_START;
#endif /* FGR_DRV_UARTSPI_HW_FOUND */
#ifdef FGR_DRV_SPI_HW_FOUND
	SPI_CONFIG_MSTR_M0;
	//SPI_set_target_clock(BAR_FREQ_500Hz);
	SPI_CONTROL &= ~(1<<SPR0);
	SPI_CONTROL |= (1<<SPR1);
	SPI_STATUS = (1<<SPI2X);
	SPI_START; SPI_INT_EN;
#endif /* FGR_DRV_SPI_HW_FOUND */
	// Enable interrupt from sync-gen.      
	//SYNC_EN_INTR;
	
	// Enable interrupts globally.
	sei();
	// Main cycle.
	while(1)
	{
		if((tasks&TASK_UPDATE_ASYNC)!=0)
		{
			tasks &= ~TASK_UPDATE_ASYNC;
			keys_simple_scan();
			// One second tick.
			div_sec++;
			if(div_sec>=seconds_top)
			{
				div_sec = 0;
				// Reset watchdog.
				wdt_reset();
				// Second tick.
				tasks |= TASK_SEC_TICK;
#ifdef CONF_EN_HD44780
				// Check if HD44780-compatible display is detected.
				if((disp_presence&HW_DISP_44780)==0)
				{
					// No display found, try to re-init it.
					HD44780_setup(HD44780_RES_16X2, HD44780_CYR_CONVERT);
					if(HD44780_init()==HD44780_OK)
					{
						disp_presence |= HW_DISP_44780;
					}
					else
					{
						disp_presence &= ~HW_DISP_44780;
					}
				}
#endif /* CONF_EN_HD44780 */
			}
#ifdef CONF_EN_HD44780
			// Check if HD44780-compatible display is detected.
			if((disp_presence&HW_DISP_44780)!=0)
			{
				// Setup character display test module.
				chardisp_set_device(HD44780_upload_symbol_flash,
									HD44780_set_xy_position,
									HD44780_write_byte,
									HD44780_write_flash_string);
				// HD44780-compatible display is connected.
				err_mask = chardisp_step_animation(tasks&TASK_SEC_TICK);
				if(err_mask!=HD44780_OK)
				{
					// Seems like display fell of the bus.
					disp_presence &= ~HW_DISP_44780;
				}
			}
#endif /* CONF_EN_HD44780 */
			// Clear processed tasks.
			tasks &= ~TASK_SEC_TICK;
		}
		// Video mode selector.
		if((kbd_state&SW_VID_SYS0)!=0)
		{
			if((kbd_state&SW_VID_SYS1)!=0)
			{
				// Select composite 525i.
				usr_video = MODE_COMP_525i;
				usr_act_delay = COMP_ACT_DELAY_525i;
				usr_act_time = COMP_ACT_LEN_525i;
				seconds_top = FPS_COMP525;
			}
			else
			{
				// Select composite 625i.
				usr_video = MODE_COMP_625i;
				usr_act_delay = COMP_ACT_DELAY_625i;
				usr_act_time = COMP_ACT_LEN_625i;
				seconds_top = FPS_COMP625;
			}
		}
		else
		{
			// Select VGA 640x480@60.
			usr_video = MODE_VGA_60Hz;
			usr_act_delay = VGA_ACT_DELAY;
			usr_act_time = VGA_ACT_LEN;
			seconds_top = FPS_VGA;
		}
	}

	return 0;
}

