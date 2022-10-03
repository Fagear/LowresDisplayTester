#include "LCD_pages.h"
#include "LCD_strings.h"

//-------------------------------------- Display "system reset" message.
void LCD_upd_page_system_reset(void)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_write_flash_line(cch_reset);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_blank);
}

//-------------------------------------- Display setup level.
void LCD_upd_misc_setup_level(unsigned char uc_level)
{
	LCD_write_byte(uc_level, LCD_DIGIT);
	LCD_write_byte('^', LCD_DATA);
}

//-------------------------------------- Display setup current page number.
void LCD_upd_misc_setup_selection(unsigned char uc_selection)
{
	unsigned char uc_temp;
	uc_temp=uc_selection+1;
	if(uc_temp>9)
		LCD_write_byte(uc_temp%100/10, LCD_NONZERO_DIGIT);
	LCD_write_byte(uc_temp%10, LCD_DIGIT);
	LCD_write_byte(')', LCD_DATA);
}

//-------------------------------------- Display error.
void LCD_upd_page_error(void)
{
	LCD_write_flash_line(cch_error_logic);
}

//-------------------------------------- Display error on whole screen.
void LCD_upd_page_error_full(void)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_write_flash_line(cch_error_logic);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_blank);
}

//-------------------------------------- Display reset confirm.
void LCD_upd_page_confirm(void)
{
	LCD_write_flash_line(cch_misc_confirm_reset);
}

//-------------------------------------- Display copyright info.
void LCD_upd_page_copyright(void)
{
	LCD_write_byte(0xA9, LCD_DATA);
	LCD_write_flash_line(cch_author);
	LCD_write_flash_line(cch_year);
}

//-------------------------------------- Display version info.
void LCD_upd_page_version(void)
{
	LCD_write_flash_line(cch_version);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display header.
void LCD_upd_page_set0_header(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display wheel setting options.
void LCD_upd_page_set0_wheel_length(void)
{
	LCD_write_flash_line(cch_setup_size);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display magnet count.
void LCD_upd_page_set0_magnets(void)
{
	LCD_write_flash_line(cch_setup_sensors);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display time.
void LCD_upd_page_set0_time(unsigned int ui_pending, unsigned char uc_hours,
	unsigned char uc_minutes, unsigned char uc_seconds)
{
	LCD_write_flash_line(cch_setup_time);
	LCD_upd_page_clock(ui_pending, uc_hours, uc_minutes, uc_seconds, OUTPUT_FULL);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display date.
void LCD_upd_page_set0_date(unsigned char uc_date, unsigned char uc_month,
	unsigned char uc_year)
{
	LCD_write_flash_line(cch_setup_date);
	LCD_upd_page_date(uc_date, uc_month, uc_year, OUTPUT_SHORT);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display UI maximum timeout.
void LCD_upd_page_set0_ui_timeout(unsigned char uc_timeout)
{
	LCD_write_flash_line(cch_setup_timeout);
	LCD_write_byte(uc_timeout/100, LCD_NONZERO_DIGIT);
	if(uc_timeout<100)
		LCD_write_byte(uc_timeout%100/10, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte(uc_timeout%100/10, LCD_DIGIT);
	LCD_write_byte(uc_timeout%10, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_sec);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display trip reset timeout.
void LCD_upd_page_set0_trip_reset(void)
{
	LCD_write_flash_line(cch_setup_dayreset);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display front light high/low mode.
void LCD_upd_page_set0_front_light(void)
{
	LCD_write_flash_line(cch_setup_front_light);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display rear light high/low mode.
void LCD_upd_page_set0_rear_light(void)
{
	LCD_write_flash_line(cch_setup_rear_light);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display LED tester.
void LCD_upd_page_set0_ledtest(void)
{
	LCD_write_flash_line(cch_setup_ledtest);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display temperature displaying mode.
void LCD_upd_page_set0_temperature(void)
{
	LCD_write_flash_line(cch_setup_temperature);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display full reseter.
void LCD_upd_page_set0_fullreset(void)
{
	LCD_write_flash_line(cch_setup_full_reset);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display odo enter page.
void LCD_upd_page_set0_odoenter(void)
{
	LCD_write_flash_line(cch_setup_enter_odo);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display debug bit.
void LCD_upd_page_set0_debug(unsigned int ui_mode)
{
	LCD_write_flash_line(cch_setup_service);
	if((ui_mode&(1<<10))==0)
		LCD_write_flash_line(cch_setup_no);
	else
		LCD_write_flash_line(cch_setup_yes);
}

//-------------------------------------- Setup level 0.
//-------------------------------------- Display thresholds page
void LCD_upd_page_set0_thresholds(void)
{
	LCD_write_flash_line(cch_setup_threshold);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display wheel setting options.
void LCD_upd_page_set1_wheel_length(unsigned char uc_level,
	unsigned char uc_selection)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_size_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_upd_misc_setup_selection(uc_selection);
	if(uc_selection==0)
	{
		LCD_write_flash_line(cch_setup_size_diam);
	}
	else if(uc_selection==1)
	{
		LCD_write_flash_line(cch_setup_size_length);
	}
	else
	{
		LCD_write_flash_line(cch_setup_size_interact);
	}
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display magnet count setting.
void LCD_upd_page_set1_magnets(unsigned char uc_level, unsigned char uc_digit)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_sensors_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_choose);
	LCD_write_line("    ");
	LCD_write_byte(uc_digit, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_pcs);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display setting time.
void LCD_upd_page_set1_time(unsigned char uc_level, unsigned int ui_pending,
	unsigned char uc_selector, set_parameters *set_param)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_time_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_set);
	LCD_write_line("   ");
// Check if need to blink.
	if(uc_selector==3)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_1, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_1, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==2)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	LCD_write_byte(0x3A, LCD_DATA);
// Check if need to blink.
	if(uc_selector==1)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==0)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display setting date.
void LCD_upd_page_set1_date(unsigned char uc_level, unsigned int ui_pending,
	unsigned char uc_selector, set_parameters *set_param)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_date_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_line("      ");
// Check if need to blink.
	if(uc_selector==5)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_1, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_1, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==4)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	LCD_write_byte(0x2F, LCD_DATA);
// Check if need to blink.
	if(uc_selector==3)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==2)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	LCD_write_byte(0x2F, LCD_DATA);
	LCD_write_line("20");
// Check if need to blink.
	if(uc_selector==1)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_5, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_5, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==0)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_6, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_6, LCD_DIGIT);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display setting UI maximum timeout.
void LCD_upd_page_set1_ui_timeout(unsigned char uc_level, unsigned char uc_digit)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_timeout_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_choose);
	LCD_write_line("   ");
	LCD_write_byte(uc_digit/100, LCD_NONZERO_DIGIT);
	if(uc_digit<100)
		LCD_write_byte(uc_digit%100/10, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte(uc_digit%100/10, LCD_DIGIT);
	LCD_write_byte(uc_digit%10, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_sec);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display trip reset timeout
void LCD_upd_page_set1_trip_reset(unsigned char uc_level, unsigned char uc_selector)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_dayreset_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_choose);
	if(uc_selector==0)
	{
		LCD_write_flash_line(cch_setup_dayreset_off);
	}
	else if(uc_selector==1)
	{
		LCD_write_flash_line(cch_setup_dayreset_6hr);
	}
	else if(uc_selector==2)
	{
		LCD_write_flash_line(cch_setup_dayreset_12hr);
	}
	else if(uc_selector==3)
	{
		LCD_write_flash_line(cch_setup_dayreset_24hr);
	}
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display front light high/low mode.
void LCD_upd_page_set1_front_light(unsigned char uc_level, unsigned char uc_selector)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_front_light_up);
// Update second line.
	LCD_set_position(0,1);
	if(uc_selector==0)
	{
		LCD_write_flash_line(cch_setup_light_sep);
	}
	else if(uc_selector==1)
	{
		LCD_write_flash_line(cch_setup_front_light_both);
	}
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display rear light high/low mode.
void LCD_upd_page_set1_rear_light(unsigned char uc_level, unsigned char uc_selector)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_rear_light_up);
// Update second line.
	LCD_set_position(0,1);
	if(uc_selector==0)
	{
		LCD_write_flash_line(cch_setup_light_sep);
	}
	else if(uc_selector==1)
	{
		LCD_write_flash_line(cch_setup_rear_light_both);
	}
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display LED tester.
void LCD_upd_page_set1_ledtest(unsigned char uc_level, unsigned char uc_counter)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_ledtest_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_ledtest_down);
	if((uc_counter%4)==3)
		LCD_write_line("... ");
	else if((uc_counter%4)==2)
		LCD_write_line("..  ");
	else if((uc_counter%4)==1)
		LCD_write_line(".   ");
	else
		LCD_write_line("    ");
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display temperature displaying mode.
void LCD_upd_page_set1_temperature(unsigned char uc_level, unsigned char uc_selector)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_temperature_up);
// Update second line.
	LCD_set_position(0,1);
	if(uc_selector==0)
	{
		LCD_write_flash_line(cch_setup_temperature_on);
	}
	else if(uc_selector==1)
	{
		LCD_write_flash_line(cch_setup_temperature_standby);
	}
	else if(uc_selector==2)
	{
		LCD_write_flash_line(cch_setup_temperature_off);
	}
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display full reseter.
void LCD_upd_page_set1_fullreset(unsigned char uc_level)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_full_reset_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_misc_confirm_reset);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display odo enter page.
void LCD_upd_page_set1_odoenter(unsigned char uc_level, unsigned int ui_pending,
	unsigned char uc_selector, set_parameters *set_param)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_enter_odo_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_set);
	LCD_write_byte(0x20, LCD_DATA);
// Check if need to blink.
	if(uc_selector==4)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
// Check if need to blink.
	if(uc_selector==3)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			if((*set_param).uc_set_1==0)
				LCD_write_byte((*set_param).uc_set_2, LCD_NONZERO_DIGIT);
			else
				LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	else
		if((*set_param).uc_set_1==0)
			LCD_write_byte((*set_param).uc_set_2, LCD_NONZERO_DIGIT);
		else
			LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==2)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			if(((*set_param).uc_set_1==0)&&((*set_param).uc_set_2==0))
				LCD_write_byte((*set_param).uc_set_3, LCD_NONZERO_DIGIT);
			else
				LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
	else
		if(((*set_param).uc_set_1==0)&&((*set_param).uc_set_2==0))
			LCD_write_byte((*set_param).uc_set_3, LCD_NONZERO_DIGIT);
		else
			LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==1)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			if(((*set_param).uc_set_1==0)&&((*set_param).uc_set_2==0)&&((*set_param).uc_set_3==0))
				LCD_write_byte((*set_param).uc_set_4, LCD_NONZERO_DIGIT);
			else
				LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	else
		if(((*set_param).uc_set_1==0)&&((*set_param).uc_set_2==0)&&((*set_param).uc_set_3==0))
			LCD_write_byte((*set_param).uc_set_4, LCD_NONZERO_DIGIT);
		else
			LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==0)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_5, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_5, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_km);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display setting debug bit.
void LCD_upd_page_set1_debug(unsigned char uc_level, unsigned char uc_bit)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_service_up);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_choose);
	LCD_write_line("    ");
	if(uc_bit==0)
		LCD_write_flash_line(cch_setup_no);
	else
		LCD_write_flash_line(cch_setup_yes);
}

//-------------------------------------- Setup level 1.
//-------------------------------------- Display threshold selector.
void LCD_upd_page_set1_thresholds(unsigned char uc_level, unsigned char uc_selector)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_threshold_up);
// Update first line.
	LCD_set_position(0,1);
	LCD_upd_misc_setup_selection(uc_selector-10);
	if(uc_selector==10)
	{
		LCD_write_flash_line(cch_setup_thres_ADC_BAT_FULL);
	}
	else if(uc_selector==11)
	{
		LCD_write_flash_line(cch_setup_thres_ADC_BAT_HIGH);
	}
	else if(uc_selector==12)
	{
		LCD_write_flash_line(cch_setup_thres_ADC_BAT_MID);
	}
	else if(uc_selector==13)
	{
		LCD_write_flash_line(cch_setup_thres_ADC_BAT_LOW);
	}
	else if(uc_selector==14)
	{
		LCD_write_flash_line(cch_setup_thres_LIGHT_AUTO_ON);
	}
	else if(uc_selector==15)
	{
		LCD_write_flash_line(cch_setup_thres_LIGHT_AUTO_OFF);
	}
	else if(uc_selector==16)
	{
		LCD_write_flash_line(cch_setup_thres_SPEED_L_HIGH);
	}
	else if(uc_selector==17)
	{
		LCD_write_flash_line(cch_setup_thres_SPEED_L_MID_HI);
	}
	else if(uc_selector==18)
	{
		LCD_write_flash_line(cch_setup_thres_SPEED_L_MID_LO);
	}
	else if(uc_selector==19)
	{
		LCD_write_flash_line(cch_setup_thres_SPEED_L_OFF);
	}
	else
	{
		LCD_write_flash_line(cch_error_logic);
	}
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display setting wheel diameter.
void LCD_upd_page_set2_wdiam(unsigned char uc_level, unsigned int ui_pending,
	unsigned char uc_selector, set_parameters *set_param)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_size_diam_up);
// Update second line.
	LCD_set_position(0,1);
// Convert and output.
	LCD_write_flash_line(cch_setup_set);
	LCD_write_line("  ");
// Check if need to blink.
	if(uc_selector==3)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
// Check if need to blink.
	if(uc_selector==2)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
// Output divider.
	LCD_write_byte('.', LCD_DATA);
// Check if need to blink.
	if(uc_selector==1)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==0)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
// Output string.
	LCD_write_byte(0x22, LCD_DATA);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display setting wheel length.
void LCD_upd_page_set2_wlength(unsigned char uc_level, unsigned int ui_pending,
	unsigned char uc_selector, set_parameters *set_param)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_size_length_up);
// Update second line.
	LCD_set_position(0,1);
// Convert and output.
	LCD_write_flash_line(cch_setup_set);
	LCD_write_line("  ");
// Check if need to blink.
	if(uc_selector==3)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
// Check if need to blink.
	if(uc_selector==2)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==1)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==0)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
// Output string.
	LCD_write_flash_line(cch_misc_mm);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display interactive wheel length setting.
void LCD_upd_page_set2_interactive(unsigned char uc_level,
	unsigned char uc_counter_1, unsigned char uc_counter_2)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_size_interact_up);
	LCD_write_byte(uc_counter_1, LCD_DIGIT);
	LCD_write_flash_line(cch_setup_size_sensors);
// Update second line.
	LCD_set_position(0,1);
	LCD_write_flash_line(cch_setup_size_turns);
	LCD_write_byte(uc_counter_2, LCD_DIGIT);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display ADC_BAT_FULL threshold setting.
void LCD_upd_page_set2_ADC_BAT_FULL(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_ADC_BAT_FULL);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display ADC_BAT_HIGH threshold setting.
void LCD_upd_page_set2_ADC_BAT_HIGH(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_ADC_BAT_HIGH);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display ADC_BAT_MID threshold setting.
void LCD_upd_page_set2_ADC_BAT_MID(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_ADC_BAT_MID);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display ADC_BAT_LOW threshold setting.
void LCD_upd_page_set2_ADC_BAT_LOW(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_ADC_BAT_LOW);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display LIGHT_AUTO_ON threshold setting.
void LCD_upd_page_set2_LIGHT_AUTO_ON(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_LIGHT_AUTO_ON);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display LIGHT_AUTO_OFF threshold setting.
void LCD_upd_page_set2_LIGHT_AUTO_OFF(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_LIGHT_AUTO_OFF);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display SPEED_L_HIGH threshold setting.
void LCD_upd_page_set2_SPEED_L_HIGH(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_SPEED_L_HIGH);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display SPEED_L_MID_HI threshold setting.
void LCD_upd_page_set2_SPEED_L_MID_HI(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_SPEED_L_MID_HI);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display SPEED_L_MID_LO threshold setting.
void LCD_upd_page_set2_SPEED_L_MID_LO(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_SPEED_L_MID_LO);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display SPEED_L_OFF threshold setting.
void LCD_upd_page_set2_SPEED_L_OFF(unsigned char uc_level)
{
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_thres_SPEED_L_OFF);
}

//-------------------------------------- Setup level 2.
//-------------------------------------- Display threshold setting.
void LCD_upd_page_set2_thres(unsigned int ui_pending, set_parameters *set_param)
{
	unsigned char uc_space_add=0;
// Check if additional spaces are needed.
	if(((*set_param).uc_set_2)<100)
		uc_space_add++;
	if(((*set_param).uc_set_2)<10)
		uc_space_add++;
	if(((*set_param).uc_set_3)<100)
		uc_space_add++;
	if(((*set_param).uc_set_3)<10)
		uc_space_add++;
	LCD_write_byte('(', LCD_DATA);
// Output lower limit.
	LCD_write_number((*set_param).uc_set_2, LCD_NUMBER);
// Output divider.
	LCD_write_byte(0x85, LCD_DATA);
// Output higher limit.
	LCD_write_number((*set_param).uc_set_3, LCD_NUMBER);
	LCD_write_line(")    ");
// Add spaces if needed.
	for(unsigned char uc_counter=0;uc_counter<uc_space_add;uc_counter++)
	{
		LCD_write_byte(0x20, LCD_DATA);
	}
// Output value.
	LCD_write_number((*set_param).uc_set_1, LCD_NUMBER_SPACES);
}

//-------------------------------------- Setup level 3.
//-------------------------------------- Display setting wheel path.
void LCD_upd_page_set3_path(unsigned char uc_level, unsigned int ui_pending,
	unsigned char uc_selector, set_parameters *set_param)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_upd_misc_setup_level(uc_level);
	LCD_write_flash_line(cch_setup_size_interact_in_up);
// Update second line.
	LCD_set_position(0,1);
// Convert and output.
	LCD_write_flash_line(cch_setup_size_interact_in_down);
// Check if need to blink.
	if(uc_selector==4)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_1, LCD_NONZERO_DIGIT);
// Check if need to blink.
	if(uc_selector==3)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			if((*set_param).uc_set_1!=0)
				LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
			else
				LCD_write_byte((*set_param).uc_set_2, LCD_NONZERO_DIGIT);
	else
		if((*set_param).uc_set_1!=0)
			LCD_write_byte((*set_param).uc_set_2, LCD_DIGIT);
		else
			LCD_write_byte((*set_param).uc_set_2, LCD_NONZERO_DIGIT);
// Check if need to blink.
	if(uc_selector==2)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_3, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==1)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_4, LCD_DIGIT);
// Check if need to blink.
	if(uc_selector==0)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(DISP_CURSOR_SYMBOL, LCD_DATA);
		else
			LCD_write_byte((*set_param).uc_set_5, LCD_DIGIT);
	else
		LCD_write_byte((*set_param).uc_set_5, LCD_DIGIT);
// Output string.
	LCD_write_flash_line(cch_misc_mm);
}

//-------------------------------------- Auto setting frontlight.
//-------------------------------------- Display current mode.
void LCD_upd_page_auto_frontlight(unsigned char uc_mode)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_write_flash_line(cch_auto_frontlight);
// Update second line.
	LCD_set_position(0,1);
	if((uc_mode&((1<<1)|(1<<0)))==0)
	{
		LCD_write_flash_line(cch_misc_auto);
	}
	else if((uc_mode&((1<<1)|(1<<0)))==1)
	{
		LCD_write_flash_line(cch_misc_low);
	}
	else if((uc_mode&((1<<1)|(1<<0)))==2)
	{
		LCD_write_flash_line(cch_misc_high);
	}
	else
	{
		LCD_write_flash_line(cch_misc_off1);
	}
}

//-------------------------------------- Auto setting LCD BL.
//-------------------------------------- Display current mode.
void LCD_upd_page_auto_lcd(unsigned char uc_mode)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_write_flash_line(cch_auto_LCD_BL);
// Update second line.
	LCD_set_position(0,1);
	if((uc_mode&((1<<5)|(1<<4)))==0)
	{
		LCD_write_flash_line(cch_misc_auto);
	}
	else if((uc_mode&((1<<5)|(1<<4)))==16)
	{
		LCD_write_flash_line(cch_misc_on1);
	}
	else
	{
		LCD_write_flash_line(cch_misc_off1);
	}
}

//-------------------------------------- Auto setting rearlight.
//-------------------------------------- Display current mode.
void LCD_upd_page_auto_rearlight(unsigned char uc_mode)
{
// Update first line.
	LCD_set_position(0,0);
	LCD_write_flash_line(cch_auto_rearlight);
// Update second line.
	LCD_set_position(0,1);
	if((uc_mode&((1<<3)|(1<<2)))==0)
	{
		LCD_write_flash_line(cch_misc_auto);
	}
	else if((uc_mode&((1<<3)|(1<<2)))==4)
	{
		LCD_write_flash_line(cch_misc_on2);
	}
	else if((uc_mode&((1<<3)|(1<<2)))==8)
	{
		LCD_write_flash_line(cch_misc_blink);
	}
	else
	{
		LCD_write_flash_line(cch_misc_off2);
	}
}

//-------------------------------------- Display current speed.
void LCD_upd_page_speed(unsigned int ui_speed)
{
	unsigned char uc_speed, uc_speed_frac;
// Convert for displaying.
	uc_speed=ui_speed/10;
	uc_speed_frac=ui_speed%10;
// Output left separator.
	LCD_write_byte(0xAB, LCD_DATA);
// Convert and output.
	LCD_write_number(uc_speed, LCD_NUMBER_SPACES);
	LCD_write_byte('.', LCD_DATA);
	LCD_write_byte(uc_speed_frac%10, LCD_DIGIT);
// Output right separator.
	LCD_write_byte(0xBB, LCD_DATA);
}

//-------------------------------------- Display average/maximum speed.
void LCD_upd_page_avgmaxspeed(unsigned int ui_speed, unsigned char uc_mode)
{
	unsigned char uc_speed, uc_speed_frac;
// Convert for displaying.
	uc_speed=ui_speed/10;
	uc_speed_frac=ui_speed%10;
// Check displaying mode.
	if(uc_mode==DISP_SPEED_MAX)
	{
	// Send string.
		LCD_write_flash_line(cch_drive_vmax);
	}
	else if(uc_mode==DISP_SPEED_AVG)
	{
	// Send string.
		LCD_write_flash_line(cch_drive_vavr);
	}
// Convert and output.
	LCD_write_number(uc_speed, LCD_NUMBER_SPACES);
	LCD_write_byte('.', LCD_DATA);
	LCD_write_byte(uc_speed_frac%10, LCD_DIGIT);
// Send strings.
	LCD_write_flash_line(cch_misc_speed);
}

//-------------------------------------- Display clock.
void LCD_upd_page_clock(unsigned int ui_pending, unsigned char uc_hours,
	unsigned char uc_minutes, unsigned char uc_seconds, unsigned char uc_time_mode)
{
	unsigned char uc_temp;
// Output hours.
	uc_temp=((uc_hours>>4)&((1<<1)|(1<<0)));
	LCD_write_byte(uc_temp, LCD_NONZERO_DIGIT);
	uc_temp=(uc_hours&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
	LCD_write_byte(uc_temp, LCD_DIGIT);
// Blink divider if "short mode".
	if(((ui_pending&(1<<14))!=0)&&(uc_time_mode==OUTPUT_SHORT))
		LCD_write_byte(0x20, LCD_DATA);
	else
		LCD_write_byte(0x3A, LCD_DATA);
	uc_temp=(uc_minutes>>4);
	LCD_write_byte(uc_temp, LCD_DIGIT);
	uc_temp=(uc_minutes&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
	LCD_write_byte(uc_temp, LCD_DIGIT);
// Write second if "full mode".
	if(uc_time_mode==OUTPUT_FULL)
	{
		LCD_write_byte(0x3A, LCD_DATA);
		uc_temp=((uc_seconds>>4)&((1<<2)|(1<<1)|(1<<0)));
		LCD_write_byte(uc_temp, LCD_DIGIT);
		uc_temp=(uc_seconds&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
		LCD_write_byte(uc_temp, LCD_DIGIT);
	}
}

//-------------------------------------- Display date.
void LCD_upd_page_date(unsigned char uc_date, unsigned char uc_month,
	unsigned char uc_year, unsigned char uc_date_mode)
{
	unsigned char uc_temp;
// Check if DS1307 is not ok.
	if(((uc_date==0)||(uc_month==0)||(uc_year==0))&&(!(uc_date_mode==OUTPUT_SHORT)))
	{
	// Output error message.
		LCD_write_flash_line(cch_error_rtc);
	}
	else
	{
	// Output day.
		uc_temp=(uc_date>>4);
		LCD_write_byte(uc_temp, LCD_NONZERO_DIGIT);
		uc_temp=(uc_date&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
		LCD_write_byte(uc_temp, LCD_DIGIT);
	// Output month in digits in "short" mode.
		if(uc_date_mode==OUTPUT_SHORT)
		{
			LCD_write_byte(0x2F, LCD_DATA);
			uc_temp=(uc_month>>4);
			LCD_write_byte(uc_temp, LCD_DIGIT);
			uc_temp=(uc_month&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
			LCD_write_byte(uc_temp, LCD_DIGIT);
		}
	// Output month and year in "full" mode.
		else
		{
			LCD_write_byte(0x20, LCD_DATA);
			uc_temp=(uc_month&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
			if((uc_month&(1<<4))==0)
			{
				if(uc_temp==1)
				{
					LCD_write_flash_line(cch_date_jan);
				}
				else if(uc_temp==2)
				{
					LCD_write_flash_line(cch_date_feb);
				}
				else if(uc_temp==3)
				{
					LCD_write_flash_line(cch_date_mar);
				}
				else if(uc_temp==4)
				{
					LCD_write_flash_line(cch_date_apr);
				}
				else if(uc_temp==5)
				{
					LCD_write_flash_line(cch_date_may);
				}
				else if(uc_temp==6)
				{
					LCD_write_flash_line(cch_date_jun);
				}
				else if(uc_temp==7)
				{
					LCD_write_flash_line(cch_date_jul);
				}
				else if(uc_temp==8)
				{
					LCD_write_flash_line(cch_date_aug);
				}
				else if(uc_temp==9)
				{
					LCD_write_flash_line(cch_date_sep);
				}
			}
			else
			{
				if(uc_temp==0)
				{
					LCD_write_flash_line(cch_date_oct);
				}
				if(uc_temp==1)
				{
					LCD_write_flash_line(cch_date_nov);
				}
				if(uc_temp==2)
				{
					LCD_write_flash_line(cch_date_dec);
				}
			}
			LCD_write_byte(0x20, LCD_DATA);
		// Output year.
			LCD_write_line("20");
			uc_temp=(uc_year>>4);
			LCD_write_byte(uc_temp, LCD_DIGIT);
			uc_temp=(uc_year&((1<<3)|(1<<2)|(1<<1)|(1<<0)));
			LCD_write_byte(uc_temp, LCD_DIGIT);
		}
	}
}

//-------------------------------------- Display overall distance.
void LCD_upd_page_odo(unsigned int ui_odo, unsigned char uc_odo_frac)
{
// Send strings.
	LCD_write_flash_line(cch_drive_odo);
// Convert and output.
	LCD_write_byte(ui_odo/10000, LCD_NONZERO_DIGIT);
	if(ui_odo<1000)
		LCD_write_byte(ui_odo%10000/1000, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte(ui_odo%10000/1000, LCD_DIGIT);
	if(ui_odo<100)
		LCD_write_byte(ui_odo%1000/100, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte(ui_odo%1000/100, LCD_DIGIT);
	if(ui_odo<10)
		LCD_write_byte(ui_odo%100/10, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte(ui_odo%100/10, LCD_DIGIT);
	LCD_write_byte(ui_odo%10, LCD_DIGIT);
	LCD_write_byte('.', LCD_DATA);
	LCD_write_byte(uc_odo_frac%10, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_km);
}

//-------------------------------------- Display day distance.
void LCD_upd_page_trip(unsigned long ul_input)
{
	unsigned long ul_trip, uc_trip_frac;
// Convert trip for displaying.
	ul_trip=ul_input/1000;	// m.
	uc_trip_frac=(ul_input%1000)/100;
// Output strings.
	LCD_write_flash_line(cch_drive_trip);
// Convert and output.
	ltoa(ul_trip, ch_LCD_buf, 10);
	if(ul_trip<100000)
		LCD_write_byte(0x20, LCD_DATA);
	if(ul_trip<10000)
		LCD_write_byte(0x20, LCD_DATA);
	if(ul_trip<1000)
		LCD_write_byte(0x20, LCD_DATA);
	if(ul_trip<100)
		LCD_write_byte(0x20, LCD_DATA);
	if(ul_trip<10)
		LCD_write_byte(0x20, LCD_DATA);
	LCD_write_line(ch_LCD_buf);
	LCD_write_byte('.', LCD_DATA);
	LCD_write_byte(uc_trip_frac%10, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_m);
}

//-------------------------------------- Display trip time.
void LCD_upd_page_ttrip(unsigned long ul_time)
{
	unsigned char uc_temp;
// Send strings.
	LCD_write_flash_line(cch_drive_ttrip);
// Calculate hours.
	uc_temp=ul_time/36000;
// Convert and output.
	itoa(uc_temp,ch_LCD_buf,10);
	if(uc_temp<10)
		LCD_write_byte(0x20, LCD_DATA);
	LCD_write_line(ch_LCD_buf);
	LCD_write_byte(0x3A, LCD_DATA);
// Calculate minutes.
	uc_temp=(ul_time%36000)/60;
// Convert and output.
	itoa(uc_temp,ch_LCD_buf,10);
	if(uc_temp<10)
		LCD_write_byte(0x30, LCD_DATA);
	LCD_write_line(ch_LCD_buf);
	LCD_write_byte(0x3A, LCD_DATA);
// Calculate seconds.
	uc_temp=ul_time%60;
// Convert and output.
	itoa(uc_temp,ch_LCD_buf,10);
	if(uc_temp<10)
		LCD_write_byte(0x30, LCD_DATA);
	LCD_write_line(ch_LCD_buf);
}

//-------------------------------------- Display current temperature.
void LCD_upd_page_envtemp(unsigned char uc_status, unsigned char uc_temperature,
	unsigned char uc_sign, unsigned char uc_temp_mode)
{
// Write text in "full" mode.
	if(uc_temp_mode==OUTPUT_FULL)
	{
	// Check if sensor not present.
		if(uc_status!=ONEWIRE_GOOD)
		{
			LCD_write_flash_line(cch_error_1wire);
			return;
		}
	// Send strings.
		LCD_write_flash_line(cch_drive_tenv);
	}
// Convert and output.
// Check if sensor present.
	if(uc_status==ONEWIRE_GOOD)
	{
		if(uc_sign==0)
			LCD_write_byte(uc_temperature/100, LCD_NONZERO_DIGIT);
		else
			LCD_write_byte('-', LCD_DATA);
		if(uc_temperature<100)
			LCD_write_byte(uc_temperature%100/10, LCD_NONZERO_DIGIT);
		else
			LCD_write_byte(uc_temperature%100/10, LCD_DIGIT);
		LCD_write_byte(uc_temperature%10, LCD_DIGIT);
	}
	else
	{
		LCD_write_line("---");
	}
	LCD_write_byte(0xB0, LCD_DATA);
	LCD_write_byte(0x43, LCD_DATA);
}

//-------------------------------------- Display minimum/maximum temperature.
void LCD_upd_page_minmaxtemp(unsigned char uc_temperature, unsigned char uc_sign,
	unsigned char uc_mode)
{
	if(uc_mode==DISP_TEMP_MIN)
	{
	// Send string.
		LCD_write_flash_line(cch_drive_tmin);
	}
	else if(uc_mode==DISP_TEMP_MAX)
	{
	// Send string.
		LCD_write_flash_line(cch_drive_tmax);
	}
// Convert and output.
	if(uc_sign==0)
		LCD_write_byte(uc_temperature/100, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte('-', LCD_DATA);
	if(uc_temperature<100)
		LCD_write_byte(uc_temperature%100/10, LCD_NONZERO_DIGIT);
	else
		LCD_write_byte(uc_temperature%100/10, LCD_DIGIT);
	LCD_write_byte(uc_temperature%10, LCD_DIGIT);
	LCD_write_byte(0xB0, LCD_DATA);
	LCD_write_byte(0x43, LCD_DATA);
}

//-------------------------------------- Display voltage, light level, cycle run time [debug].
void LCD_upd_page_debug(unsigned char uc_voltage, unsigned int ui_light,
	unsigned char uc_runtime)
{
	unsigned char uc_temp;
	unsigned int ui_temp;
	LCD_write_line("  ");
// Convert value.
	ui_temp=(unsigned int)uc_voltage*100;
	ui_temp=ui_temp/255;
	uc_temp=(unsigned char)ui_temp;
// Output voltage level.
	LCD_write_number(uc_temp, LCD_NUMBER_SPACES);
	LCD_write_byte('%', LCD_DATA);
// Divider.
	LCD_write_byte(0x2F, LCD_DATA);
// Convert value.
	uc_temp=(unsigned char)ui_light;
	uc_temp=255-uc_temp;
	ui_temp=(unsigned int)uc_temp*100;
	ui_temp=ui_temp/255;
	uc_temp=(unsigned char)ui_temp;
// Output light level.
	LCD_write_number(uc_temp, LCD_NUMBER_SPACES);
	LCD_write_byte('%', LCD_DATA);
// Divider.
	LCD_write_byte(0x2F, LCD_DATA);
// Convert and output.
	LCD_write_byte(uc_runtime%100/10, LCD_NONZERO_DIGIT);
	LCD_write_byte(uc_runtime%10, LCD_DIGIT);
	LCD_write_flash_line(cch_misc_msec);
}

//-------------------------------------- Display LEDs status [debug].
void LCD_upd_page_LEDs(void)
{
// Send strings.
	LCD_write_flash_line(cch_debug_leds);
// LCD BL LED.
	if((PIND&(1<<7))==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
// Divider.
	LCD_write_byte(0x7C, LCD_DATA);
// Front light, low power.
	if((LIGHT_FRONT_PIN&LIGHT_FRONT_LOW_PIN)==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
// Front light, high power.
	if((LIGHT_FRONT_PIN&LIGHT_FRONT_HI_PIN)==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
// Divider.
	LCD_write_byte(0x7C, LCD_DATA);
// Rear light, low power.
	if((LIGHT_REAR_PIN&LIGHT_REAR_LOW_PIN)==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
// Rear light, high power.
	if((LIGHT_REAR_PIN&LIGHT_REAR_HI_PIN)==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
// Divider.
	LCD_write_byte(0x7C, LCD_DATA);
// Turn light, left.
	if((LIGHT_TURN_PIN&LIGHT_TURN_L_PIN)==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
// Turn light, right.
	if((LIGHT_TURN_PIN&LIGHT_TURN_R_PIN)==0)
		LCD_write_byte(0xB3, LCD_DATA);
	else
		LCD_write_byte(0xB2, LCD_DATA);
}

//-------------------------------------- Display battery charge level
void LCD_upd_page_battery(unsigned int ui_pending, unsigned char uc_voltage,
	unsigned char uc_bat_low, unsigned char uc_bat_mid, unsigned char uc_bat_high,
	unsigned char uc_bat_full)
{
	if(uc_voltage<uc_bat_low)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(0x0F, LCD_DATA);
		else
			LCD_write_byte(0x20, LCD_DATA);
	else if(uc_voltage<uc_bat_mid)
		if((ui_pending&(1<<14))==0)
			LCD_write_byte(0x0E, LCD_DATA);
		else
			LCD_write_byte(0x20, LCD_DATA);
	else if(uc_voltage<uc_bat_high)
		LCD_write_byte(0x0D, LCD_DATA);
	else if(uc_voltage<uc_bat_full)
		LCD_write_byte(0x0C, LCD_DATA);
	else
		LCD_write_byte(0x0B, LCD_DATA);
}

//-------------------------------------- Display counter.
/*void LCD_upd_page_sys(const unsigned long l_page_sys)
{
// Send strings.
	LCD_write_flash_line(cch_misc_sys);
// Convert and output.
	ltoa(l_page_sys,ch_LCD_buf,16);
	if(l_page_sys<100000)
		LCD_write_byte(0x20, LCD_DATA);
	if(l_page_sys<10000)
		LCD_write_byte(0x20, LCD_DATA);
	if(l_page_sys<1000)
		LCD_write_byte(0x20, LCD_DATA);
	if(l_page_sys<100)
		LCD_write_byte(0x20, LCD_DATA);
	if(l_page_sys<10)
		LCD_write_byte(0x20, LCD_DATA);
	LCD_write_line(ch_LCD_buf);
	LCD_write_byte(0x20, LCD_DATA);
	LCD_write_byte(0x20, LCD_DATA);
	LCD_write_byte(0x20, LCD_DATA);
}
*/
