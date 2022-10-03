/*
 * atm.c
 *
 * Created:			2022-05-04 11:56:23
 * Modified:		2022-05-20
 * Author:			Maksim Kryukov aka Fagear (fagear@mail.ru)
 *
 */ 

#include "syncavr.h"

volatile uint8_t tasks = 0;
uint8_t disp_presence = 0;		// Flags for detected displays types
uint8_t active_region = 0;		// Flag for lines in active region.
uint16_t sync_step_cnt = 0;		// Current step in sync generator logic.
uint16_t frame_line_cnt = 0;	// Number of current line in the frame.
uint8_t video_sys = COMP_625i;	// Video system value, used in interrupt (buffered).
uint8_t usr_video = COMP_625i;	// Video system value set by user.
uint16_t dbg_index = 0;
uint8_t hd44780_page_step = 0;
uint8_t hd44780_ani_step = 0;
uint8_t kbd_state = 0;			// Buttons states from the last [keys_simple_scan()] poll.
uint8_t kbd_pressed = 0;		// Flags for buttons that have been pressed (should be cleared after processing).
uint8_t kbd_released = 0;		// Flags for buttons that have been released (should be cleared after processing).
uint8_t comp_data_idx = 0;		// Index for vertical bar groups.

volatile const uint8_t ucaf_compile_time[] PROGMEM = __TIME__;		// Time of compilation
volatile const uint8_t ucaf_info[] PROGMEM = "ATmega sync generator";							// Firmware description
volatile const uint8_t ucaf_version[] PROGMEM = "v0.04";			// Firmware version
volatile const uint8_t ucaf_compile_date[] PROGMEM = __DATE__;		// Date of compilation
volatile const uint8_t ucaf_author[] PROGMEM = "Maksim Kryukov aka Fagear (fagear@mail.ru)";	// Author

//-------------------------------------- Composite horizontal sync timing.
ISR(INT0_INT)
{
	DBG_1_ON;
	
	if(sync_step_cnt==0)
	{
		// Produce pulse on the start of the frame.
		DBG_4_ON;
		tasks |= TASK_UPDATE_ASYNC;
		// Update video system to user's choice.
		video_sys = usr_video;
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
		if(video_sys==COMP_625i)
		{
			// Apply length of the active part of the line for composite 625i.
			LACT_PULSE_DUR = LINE_625i_ACT;
		}
		else if(video_sys==COMP_525i)
		{
			// Apply length of the active part of the line for composite 525i.
			LACT_PULSE_DUR = LINE_525i_ACT;
		}
		else if(video_sys==COMP_525i)
		{
			// Apply length of the active part of the line for VGA 640x480@60.
			LACT_PULSE_DUR = LINE_VGA_ACT;
		}
#endif
		DBG_4_OFF;
	}
	/*if(frame_line_cnt==(dbg_index-1))
	{
		// Produce pulse for selected video line.
		DBG_5_ON;
		NOP;
		DBG_5_OFF;
	}*/
	
	// Check if current line is in active region.
	if(active_region!=0)
	{
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
		// Force OC pin LOW (workaround for skewed timing to prevent Hsync damage).
		LACT_OC_FORCE;
		// Switch to "Toggle OC pin on compare" to allow pin to be switched HIGH from forced LOW.
		LACT_OC_TOGGLE1; LACT_OC_TOGGLE2;
#endif
		// Wait for active line part start.
		if((video_sys==COMP_625i)||(video_sys==COMP_525i))
		{
			while(VTIM_DATA<COMP_ACT_DELAY)
			{
				NOP;
			}
		}
		else if(video_sys==VGA_60Hz)
		{
			while(VTIM_DATA<50)
			{
				NOP;
			}
		}
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
		// Force OC pin HIGH from LOW.
		LACT_OC_FORCE;
		// Switch to "Clear OC pin on compare" to ensure that OC pin will be switched OFF when timer runs out.
		LACT_OC_CLEAR1; LACT_OC_CLEAR2;
		// Start line active region timer.
		LACT_START;
#endif
#ifdef FGR_DRV_SPI_HW_FOUND
		// Set minimal frequency for first set of vertical bars.
		SPI_set_target_clock(BAR_FREQ_500Hz);
		comp_data_idx = 0;
		// Start drawing first set of vertical bars.
		SPI_DATA = SPI_DUMMY_SEND;
#endif /* FGR_DRV_SPI_HW_FOUND */
	}
	// Allow timer overflow interrupt to active after this one.
	VTIM_EN_INTR;
	DBG_1_OFF;
}

//-------------------------------------- Video horizontal sync timing.
ISR(VTIM_INT)
{
	DBG_2_ON;
	if(video_sys==COMP_625i)
	{
		// 625i
		if((sync_step_cnt>=ST_COMP625_F2_EQ_START)||(sync_step_cnt<=ST_COMP625_F1_EQ_STOP))
		{
			// First field sync.
			if(sync_step_cnt==ST_COMP625_F1_VS_STOP)
			{
				// Post-equalizing pulses.
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F1_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				//VTIM_STEP_DUR = COMP_LINE_LEN_625i;
				VTIM_PULSE_DUR = COMP_SYNC_H_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_EQ_START)
			{
				// Pre-equalizing pulses.
				VTIM_STEP_DUR = COMP_HALF_LEN_625i;
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_LOOP)
			{
				// Pre-load half-line configuration one line before the vertical sync.
				VTIM_PULSE_DUR = COMP_SYNC_V_LEN_625i;
			}
			if((sync_step_cnt%2)!=0)
			{
				// When on frame sync, divide line count by two (on even lines).
				frame_line_cnt++;
			}
		}
		else if((sync_step_cnt>ST_COMP625_F1_EQ_START)&&(sync_step_cnt<ST_COMP625_F2_EQ_STOP))
		{
			// Second field sync.
			if(sync_step_cnt==(ST_COMP625_F1_EQ_START+1))
			{
				// Pre-equalizing pulses.
				VTIM_STEP_DUR = COMP_HALF_LEN_625i;
				//VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_VS_START)
			{
				// Frame (vertical) sync.
				VTIM_PULSE_DUR = COMP_SYNC_V_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_VS_STOP)
			{
				// Post-equalizing pulses.
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
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
				VTIM_STEP_DUR = COMP_LINE_LEN_625i;
				//VTIM_PULSE_DUR = COMP_SYNC_H_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F1_EQ_START)
			{
				// Pre-equalizing pulses.
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_625i;
			}
			else if(sync_step_cnt==ST_COMP625_F2_EQ_STOP)
			{
				// Delay normal width sync pulse one line.
				VTIM_STEP_DUR = COMP_LINE_LEN_625i;
				VTIM_PULSE_DUR = COMP_SYNC_H_LEN_625i;
			}
			// When not on frame sync, count step as line.
			frame_line_cnt++;
		}
		if((frame_line_cnt==ST_COMP625_F1_ACT_START)||(frame_line_cnt==ST_COMP625_F2_ACT_START))
		{
			// Enable drawing in active region.
			active_region = 1;
		}
		else if((frame_line_cnt==ST_COMP625_F1_ACT_STOP)||(frame_line_cnt==ST_COMP625_F2_ACT_STOP))
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
		// Go through steps.
		sync_step_cnt++;
		if(sync_step_cnt>ST_COMP625_LOOP)
		{
			// Loop line counter within one frame.
			frame_line_cnt = 0;
			sync_step_cnt = 0;
		}
	}
	else if(video_sys==COMP_525i)
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
				VTIM_STEP_DUR = COMP_HALF_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_VS_START)
			{
				// Frame (vertical) sync.
				VTIM_PULSE_DUR = COMP_SYNC_V_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_VS_STOP)
			{
				// Post-equalizing pulses.
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F1_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				VTIM_PULSE_DUR = COMP_SYNC_H_LEN_525i;
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
				VTIM_STEP_DUR = COMP_HALF_LEN_525i;
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_VS_START)
			{
				// Frame (vertical) sync.
				VTIM_PULSE_DUR = COMP_SYNC_V_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_VS_STOP)
			{
				// Post-equalizing pulses.
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			//DBG_3_OFF;
		}
		else
		{
			// Normal line period.
			if(sync_step_cnt==(ST_COMP525_F1_EQ_STOP+1))
			{
				VTIM_STEP_DUR = COMP_LINE_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_F2_EQ_STOP)
			{
				// Normal line (horizontal) sync pulse period.
				VTIM_STEP_DUR = COMP_LINE_LEN_525i;
				// Delay normal width sync pulse one line.
				VTIM_PULSE_DUR = COMP_SYNC_H_LEN_525i;
			}
			else if(sync_step_cnt==ST_COMP525_LOOP)
			{
				// Pre-load equalizing pulse configuration one line before.
				VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;
			}
			// When not on frame sync, count step as line.
			frame_line_cnt++;
		}
		if((frame_line_cnt==ST_COMP525_F1_ACT_START)||(frame_line_cnt==ST_COMP525_F2_ACT_START))
		{
			// Enable drawing in active region.
			active_region = 1;
		}
		else if((frame_line_cnt==ST_COMP525_F1_ACT_STOP)||(frame_line_cnt==ST_COMP525_F2_ACT_STOP))
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
		// Go through steps.
		sync_step_cnt++;
		if(sync_step_cnt>ST_COMP525_LOOP)
		{
			// Loop line counter within one frame.
			frame_line_cnt = 0;
			sync_step_cnt = 0;
		}
	}
	else if(video_sys==VGA_60Hz)
	{
		// VGA 640x480
		if(sync_step_cnt==0)
		{
			VTIM_PULSE_DUR = VGA_LINE_LEN;
			VTIM_PULSE_DUR = VGA_SYNC_H_LEN;
		}
		if(frame_line_cnt==2)
		{
			// Start vertical sync pulse.
			VSYNC_PULL_DOWN;
		}
		else if(frame_line_cnt==4)
		{
			// End vertical sync pulse.
			VSYNC_PULL_UP;
		}
		else if(frame_line_cnt==37)
		{
			// Enable drawing in active region.
			active_region = 1;
		}
		else if(frame_line_cnt==517)
		{
			// Disable drawing in inactive region.
			active_region = 0;
		}
		// Go through steps.
		sync_step_cnt++;
		if(sync_step_cnt>LINES_VGA)
		{
			frame_line_cnt = 0;
			sync_step_cnt = 0;
		}
	}
	// Disable this interrupt to allow INT0 to activate first on the next pulse.
	VTIM_DIS_INTR;
	DBG_2_OFF;
}

#ifdef FGR_DRV_IO_T0OC_HW_FOUND
//-------------------------------------- Active line timing.
ISR(LACT_INT)
{
	DBG_3_ON;
	// Stop and reset this timer, wait for another start.
	LACT_STOP;
	LACT_DATA = 0;
	DBG_3_OFF;
}
#endif

#ifdef FGR_DRV_SPI_HW_FOUND
//-------------------------------------- Vertical bars batch.
ISR(SPI_INT)
{
	if(comp_data_idx<4)
	{
		// Step through series of vertical bars.
		comp_data_idx++;
		if(comp_data_idx==1)
		{
			// 1 MHz.
			SPI_set_target_clock(BAR_FREQ_1MHz);
		}
		else if(comp_data_idx==2)
		{
			// 2 MHz.
			SPI_set_target_clock(BAR_FREQ_2MHz);
		}
		else if(comp_data_idx==3)
		{
			// 4 MHz.
			SPI_set_target_clock(BAR_FREQ_4MHz);
		}
		else if(comp_data_idx==4)
		{
			// 8 MHz.
			SPI_set_target_clock(BAR_FREQ_8MHz);
		}
		// Send a byte to generate clock for bars.
		SPI_DATA = SPI_DUMMY_SEND;
	}
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
	
	// Enable watchdog.
	WDT_PREP_ON;
	WDT_SW_ON;
	wdt_reset();
}

//-------------------------------------- Restart composite sync generation.
void restart_composite(void)
{
	if(usr_video==COMP_625i)
	{
		// 625i
		VTIM_STEP_DUR = COMP_HALF_LEN_625i;			// Load duration of the cycle.
		VTIM_PULSE_DUR = COMP_SYNC_V_LEN_525i;		// Load duration of the pulse.
	}
	else
	{
		// 525i
		VTIM_STEP_DUR = COMP_HALF_LEN_525i;			// Load duration of the cycle.
		VTIM_PULSE_DUR = COMP_EQ_PULSE_LEN_525i;	// Load duration of the pulse.
	}
	VTIM_DATA = 0;		// Reset counter.
}

//-------------------------------------- Restart HD44780 communication.
void restart_hd44780(void)
{
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

//-------------------------------------- 
uint8_t step_hd44780_ani_rotate(void)
{
	uint8_t err_collector = 0;
	if(hd44780_ani_step==0)
	{
		// Load custom font.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_4, usr_char_rot1);
		err_collector += HD44780_upload_symbol_flash(C_CHAR_5, usr_char_rot2);
		err_collector += HD44780_upload_symbol_flash(C_CHAR_6, usr_char_rot3);
		err_collector += HD44780_upload_symbol_flash(C_CHAR_7, usr_char_rot4);
	}
	else if((hd44780_ani_step==1)||(hd44780_ani_step==17)||(hd44780_ani_step==33))
	{
		// Rotate 0/90 phase, step 1.
		err_collector += HD44780_set_xy_position(0, 0);
		err_collector += HD44780_write_byte(C_CHAR_4, HD44780_DATA);
		err_collector += HD44780_set_xy_position(7, 0);
		err_collector += HD44780_write_byte(C_CHAR_6, HD44780_DATA);
	}
	else if((hd44780_ani_step==5)||(hd44780_ani_step==21)||(hd44780_ani_step==37))
	{
		// Rotate 45/135 phase, step 2.
		err_collector += HD44780_set_xy_position(0, 0);
		err_collector += HD44780_write_byte(C_CHAR_5, HD44780_DATA);
		err_collector += HD44780_set_xy_position(7, 0);
		err_collector += HD44780_write_byte(C_CHAR_7, HD44780_DATA);
	}
	else if((hd44780_ani_step==9)||(hd44780_ani_step==25)||(hd44780_ani_step==41))
	{
		// Rotate 90/180 phase, step 3.
		err_collector += HD44780_set_xy_position(0, 0);
		err_collector += HD44780_write_byte(C_CHAR_6, HD44780_DATA);
		err_collector += HD44780_set_xy_position(7, 0);
		err_collector += HD44780_write_byte(C_CHAR_4, HD44780_DATA);
	}
	else if((hd44780_ani_step==13)||(hd44780_ani_step==29)||(hd44780_ani_step==45))
	{
		// Rotate 135/225 phase, step 4.
		err_collector += HD44780_set_xy_position(0, 0);
		err_collector += HD44780_write_byte(C_CHAR_7, HD44780_DATA);
		err_collector += HD44780_set_xy_position(7, 0);
		err_collector += HD44780_write_byte(C_CHAR_5, HD44780_DATA);
	}
	// Check if display was lost.
	if(err_collector!=0)
	{
		// Set flag for no display.
		disp_presence &= ~HW_DISP_44780;
	}
	
	hd44780_ani_step++;
	if(hd44780_ani_step>49)
	{
		hd44780_ani_step = 0;
		if(HD44780_shorttest()!=HD44780_OK)
		{
			// Set flag for no display.
			disp_presence &= ~HW_DISP_44780;
		}
		return HD44780_OK;
	}
	else
	{
		return HD44780_ERR_BUSY;
	}
}

//-------------------------------------- 
uint8_t step_hd44780_ani_levels(void)
{
	uint8_t err_collector = 0;
	uint8_t idx, xcoord;
	if(hd44780_ani_step==0)
	{
		// Load custom font.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_0, usr_char_lvl1);	// Horizontal "no level"
		err_collector += HD44780_upload_symbol_flash(C_CHAR_1, usr_char_lvl2);	// Horizontal "low level"
		err_collector += HD44780_upload_symbol_flash(C_CHAR_2, usr_char_lvl3);	// Horizontal "mid level"
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl4);	// Horizontal "high level"
	}
	else if((hd44780_ani_step>0)&&(hd44780_ani_step<=40))
	{
		// Fill next column with "no level" char.
		//for(idx=0;idx<4;idx++)
		for(idx=0;idx<2;idx++)
		{
			err_collector += HD44780_set_xy_position((hd44780_ani_step-1), idx);
			err_collector += HD44780_write_byte(C_CHAR_0, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==41)
	{
		// Load more of the font.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_4, usr_char_lvl33);	// Vertical "low level" for even rows
		err_collector += HD44780_upload_symbol_flash(C_CHAR_5, usr_char_lvl32);	// Vertical "mid level" for even rows
		err_collector += HD44780_upload_symbol_flash(C_CHAR_6, usr_char_lvl31);	// Vertical "high level" for even rows
		err_collector += HD44780_upload_symbol_flash(C_CHAR_7, usr_char_lvl8);	// Full fill for even rows.
	}
	else if((hd44780_ani_step>=42)&&(hd44780_ani_step<=161))
	{
		idx = (hd44780_ani_step-42)%3;
		xcoord = (hd44780_ani_step-42)/3;
		if(idx==0)
		{
			// Replace chars in the current column with horizontal "low level".
			for(idx=0;idx<2;idx++)
			{
				err_collector += HD44780_set_xy_position(xcoord, idx);
				err_collector += HD44780_write_byte(C_CHAR_1, HD44780_DATA);
			}
		}
		else if(idx==1)
		{
			// Replace chars in the current column with horizontal "mid level".
			for(idx=0;idx<2;idx++)
			{
				err_collector += HD44780_set_xy_position(xcoord, idx);
				err_collector += HD44780_write_byte(C_CHAR_2, HD44780_DATA);
			}
		}
		else if(idx==2)
		{
			// Replace chars in the current column with horizontal "high level".
			for(idx=0;idx<2;idx++)
			{
				err_collector += HD44780_set_xy_position(xcoord, idx);
				err_collector += HD44780_write_byte(C_CHAR_3, HD44780_DATA);
			}
		}
	}
	else if(hd44780_ani_step==162)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 1.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl5);
	}
	else if(hd44780_ani_step==164)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 2.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl6);
	}
	else if(hd44780_ani_step==166)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 3.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl7);
	}
	else if(hd44780_ani_step==168)
	{
		// Transition from horizontal "high level" to "full fill" on the whole display, step 4.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl8);
	}
	else if(hd44780_ani_step==170)
	{
		// Load more of the font.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_0, usr_char_lvl23);	// Vertical "low level" for odd rows
		err_collector += HD44780_upload_symbol_flash(C_CHAR_1, usr_char_lvl22);	// Vertical "mid level" for odd rows
		err_collector += HD44780_upload_symbol_flash(C_CHAR_2, usr_char_lvl21);	// Vertical "high level" for odd rows
		// Replace even rows with another char that looks the same.
		for(idx=1;idx<2;idx+=2)
		{
			for(xcoord=0;xcoord<40;xcoord++)
			{
				err_collector += HD44780_set_xy_position(xcoord, idx);
				err_collector += HD44780_write_byte(C_CHAR_7, HD44780_DATA);
			}
		}
	}
	else if(hd44780_ani_step==171)
	{
		// Transition from "full fill" to horizontal "max level" on the whole display, step 1.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl18);
		err_collector += HD44780_upload_symbol_flash(C_CHAR_7, usr_char_lvl28);
	}
	else if(hd44780_ani_step==173)
	{
		// Transition from "full fill" to horizontal "max level" on the whole display, step 2.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl19);
		err_collector += HD44780_upload_symbol_flash(C_CHAR_7, usr_char_lvl29);
	}
	else if(hd44780_ani_step==175)
	{
		// Transition from "full fill" to horizontal "max level" on the whole display, step 3.
		err_collector += HD44780_upload_symbol_flash(C_CHAR_3, usr_char_lvl20);
		err_collector += HD44780_upload_symbol_flash(C_CHAR_7, usr_char_lvl30);
	}
	else if(hd44780_ani_step==177)
	{
		// Replace first row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 0);
			err_collector += HD44780_write_byte(C_CHAR_2, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==179)
	{
		// Replace first row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 0);
			err_collector += HD44780_write_byte(C_CHAR_1, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==181)
	{
		// Replace first row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 0);
			err_collector += HD44780_write_byte(C_CHAR_0, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==183)
	{
		// Replace first row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 0);
			err_collector += HD44780_write_byte(ASCII_SPACE, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==185)
	{
		// Replace second row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 1);
			err_collector += HD44780_write_byte(C_CHAR_6, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==187)
	{
		// Replace second row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 1);
			err_collector += HD44780_write_byte(C_CHAR_5, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==189)
	{
		// Replace second row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 1);
			err_collector += HD44780_write_byte(C_CHAR_4, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==191)
	{
		// Replace second row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 1);
			err_collector += HD44780_write_byte(ASCII_SPACE, HD44780_DATA);
		}
	}
	/*else if(hd44780_ani_step==193)
	{
		// Replace third row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 2);
			err_collector += HD44780_write_byte(C_CHAR_2, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==195)
	{
		// Replace third row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 2);
			err_collector += HD44780_write_byte(C_CHAR_1, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==197)
	{
		// Replace third row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 2);
			err_collector += HD44780_write_byte(C_CHAR_0, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==199)
	{
		// Replace third row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 2);
			err_collector += HD44780_write_byte(ASCII_SPACE, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==201)
	{
		// Replace forth row with horizontal "high level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 3);
			err_collector += HD44780_write_byte(C_CHAR_6, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==203)
	{
		// Replace forth row with horizontal "mid level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 3);
			err_collector += HD44780_write_byte(C_CHAR_5, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==205)
	{
		// Replace forth row with horizontal "low level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 3);
			err_collector += HD44780_write_byte(C_CHAR_4, HD44780_DATA);
		}
	}
	else if(hd44780_ani_step==207)
	{
		// Replace forth row with "no level".
		for(xcoord=0;xcoord<40;xcoord++)
		{
			err_collector += HD44780_set_xy_position(xcoord, 3);
			err_collector += HD44780_write_byte(ASCII_SPACE, HD44780_DATA);
		}
	}*/
	// Check if display was lost.
	if(err_collector!=0)
	{
		// Set flag for no display.
		disp_presence &= ~HW_DISP_44780;
	}
	
	hd44780_ani_step++;
	if(hd44780_ani_step>210)
	{
		hd44780_ani_step = 0;
		if(HD44780_shorttest()!=HD44780_OK)
		{
			// Set flag for no display.
			disp_presence &= ~HW_DISP_44780;
		}
		return HD44780_OK;
	}
	else
	{
		return HD44780_ERR_BUSY;
	}
}

//-------------------------------------- Draw next frame of animation on HD44780-compatible display.
void step_hd44780_animation(void)
{
	uint8_t err_collector = 0;
	if(hd44780_page_step==ST_TEXT_0)
	{
		// Check if page was drawn.
		if(hd44780_ani_step==0)
		{
			// Not yet.
			hd44780_ani_step++;
			// Draw the page.
			err_collector += HD44780_set_xy_position(0, 0);
			err_collector += HD44780_write_flash_string(hd44780_det);
			tasks &= ~TASK_SEC_TICK;
		}
		else if((tasks&TASK_SEC_TICK)!=0)
		{
			// Page already drawn and got one second tick.
			tasks &= ~TASK_SEC_TICK;
			// Go to the next page.
			hd44780_page_step++;
			hd44780_ani_step = 0;
		}
	}
	else if(hd44780_page_step==ST_TEXT_1)
	{
		// Check if page was drawn.
		if(hd44780_ani_step==0)
		{
			// Not yet.
			hd44780_ani_step++;
			// Draw the page.
			err_collector += HD44780_set_xy_position(0, 0);
			err_collector += HD44780_write_flash_string(hd44780_dsp_1x8);
			tasks &= ~TASK_SEC_TICK;
		}
		else if((tasks&TASK_SEC_TICK)!=0)
		{
			// Page already drawn and got one second tick.
			tasks &= ~TASK_SEC_TICK;
			// Go to the next page.
			hd44780_page_step++;
			hd44780_ani_step = 0;
		}
	}
	else if(hd44780_page_step==ST_TEXT_2)
	{
		// Check if page was drawn.
		if(hd44780_ani_step==0)
		{
			// Not yet.
			hd44780_ani_step++;
			// Draw the page.
			err_collector += HD44780_set_xy_position(10, 0);
			err_collector += HD44780_write_flash_string(hd44780_dsp_x16);
			err_collector += HD44780_set_xy_position(0, 1);
			err_collector += HD44780_write_flash_string(hd44780_dsp_row2);
			tasks &= ~TASK_SEC_TICK;
		}
		else if((tasks&TASK_SEC_TICK)!=0)
		{
			// Page already drawn and got one second tick.
			tasks &= ~TASK_SEC_TICK;
			// Go to the next page.
			hd44780_page_step++;
			hd44780_ani_step = 0;
		}
	}
	else if(hd44780_page_step==ST_TEXT_3)
	{
		// Check if page was drawn.
		if(hd44780_ani_step==0)
		{
			// Not yet.
			hd44780_ani_step++;
			// Draw the page.
			err_collector += HD44780_set_xy_position(16, 0);
			err_collector += HD44780_write_flash_string(hd44780_dsp_x20);
			//err_collector += HD44780_set_xy_position(0, 2);
			//err_collector += HD44780_write_flash_string(hd44780_dsp_row3);
			tasks &= ~TASK_SEC_TICK;
		}
		else if((tasks&TASK_SEC_TICK)!=0)
		{
			// Page already drawn and got one second tick.
			tasks &= ~TASK_SEC_TICK;
			// Go to the next page.
			hd44780_page_step++;
			hd44780_ani_step = 0;
		}
	}
	else if(hd44780_page_step==ST_TEXT_4)
	{
		// Check if page was drawn.
		if(hd44780_ani_step==0)
		{
			// Not yet.
			hd44780_ani_step++;
			// Draw the page.
			err_collector += HD44780_set_xy_position(20, 0);
			err_collector += HD44780_write_flash_string(hd44780_dsp_x24);
			//err_collector += HD44780_set_xy_position(0, 3);
			//err_collector += HD44780_write_flash_string(hd44780_dsp_row4);
			tasks &= ~TASK_SEC_TICK;
		}
		else if((tasks&TASK_SEC_TICK)!=0)
		{
			// Page already drawn and got one second tick.
			tasks &= ~TASK_SEC_TICK;
			// Go to the next page.
			hd44780_page_step++;
			hd44780_ani_step = 0;
		}
	}
	else if(hd44780_page_step==ST_TEXT_5)
	{
		// Check if page was drawn.
		if(hd44780_ani_step==0)
		{
			// Not yet.
			hd44780_ani_step++;
			// Draw the page.
			err_collector += HD44780_set_xy_position(36, 0);
			err_collector += HD44780_write_flash_string(hd44780_dsp_x40);
			tasks &= ~TASK_SEC_TICK;
		}
		else if((tasks&TASK_SEC_TICK)!=0)
		{
			// Page already drawn and got one second tick.
			tasks &= ~TASK_SEC_TICK;
			// Go to the next page.
			hd44780_page_step++;
			hd44780_ani_step = 0;
		}
	}
	else if(hd44780_page_step==ST_TEXT_6)
	{
		if(step_hd44780_ani_rotate()==HD44780_OK)
		{
			hd44780_page_step++;
		}
	}
	else if(hd44780_page_step==ST_TEXT_7)
	{
		if(step_hd44780_ani_levels()==HD44780_OK)
		{
			hd44780_page_step++;
		}
	}
	else if(hd44780_page_step==ST_TEXT_8)
	{
		// Re-test connections.
		if(HD44780_selftest()!=HD44780_OK)
		{
			// Set flag for no display.
			disp_presence &= ~HW_DISP_44780;
		}
		hd44780_page_step++;
	}
	else
	{
		hd44780_page_step = ST_TEXT_1;
		hd44780_ani_step = 0;
	}
	// Check if display was lost.
	if(err_collector!=0)
	{
		// Set flag for no display.
		disp_presence &= ~HW_DISP_44780;
	}
	// Check if there is no display anymore.
	if((disp_presence&HW_DISP_44780)==0)
	{
		// Cancel animations.
		hd44780_page_step = 0;
		hd44780_ani_step = 0;
	}
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

int main()
{
	uint8_t div_sec = 0;
	uint8_t sec_top;
	
	// Start-up initialization.
	system_startup();
	
	sync_step_cnt = 0;
	frame_line_cnt = 0;
	//dbg_index = 1;
	//dbg_index = 305;
	//dbg_index = 315;
	dbg_index = 620;
	
	restart_hd44780();
// 	if(HD44780_init()==HD44780_OK)
// 	{
// 		HD44780_set_xy_position(0, 0);
// 		HD44780_write_string("Display TEST");
// 		HD44780_set_xy_position(0, 1);
// 		HD44780_write_number(123, HD44780_NUMBER);
// 	}
	restart_composite();
	
#ifdef FGR_DRV_UARTSPI_HW_FOUND
	UART_SPI_CONFIG_M0;
	UART_SPI_set_target_clock(BAR_FREQ_500Hz);
	UART_SPI_START;
#endif /* FGR_DRV_UARTSPI_HW_FOUND */
#ifdef FGR_DRV_SPI_HW_FOUND
	SPI_CONFIG_MSTR_M0;
	SPI_set_target_clock(BAR_FREQ_500Hz);
	SPI_START; SPI_INT_EN;
#endif /* FGR_DRV_SPI_HW_FOUND */
	
	// Enable interrupts globally.
	sei();
	// Main cycle.
	while(1)
	{
		if((tasks&TASK_UPDATE_ASYNC)!=0)
		{
			tasks &= ~TASK_UPDATE_ASYNC;
			keys_simple_scan();
			// Determine how many updates there is per second.
			if(usr_video==COMP_525i)
			{
				sec_top = 25;
			}
			else
			{
				sec_top = 30;
			}
			// One second tick.
			div_sec++;
			if(div_sec>=sec_top)
			{
				div_sec = 0;
				// Reset watchdog.
				wdt_reset();
				// Second tick.
				tasks |= TASK_SEC_TICK;
				// Check if HD44780-compatible display is detected.
				if((disp_presence&HW_DISP_44780)==0)
				{
					// No display found, try to re-init it.
					restart_hd44780();
				}
			}
			// Check if HD44780-compatible display is detected.
			if((disp_presence&HW_DISP_44780)!=0)
			{
				// HD44780-compatible display is connected.
				step_hd44780_animation();
			}
		}
		// Video mode selector.
		if((kbd_state&SW_VID_SYS1)!=0)
		{
			if((kbd_state&SW_VID_SYS0)!=0)
			{
				usr_video = COMP_525i;
			}
			else
			{
				usr_video = COMP_625i;
			}
		}
		else
		{
			usr_video = VGA_60Hz;
		}
	}

	return 0;
}

