#include <stdlib.h>
#include "drv_LCD[4bit].h"
#include "1wire_driver.h"
#include "Lamp_driver.h"
#include "Thresholds.h"
#include "setup_parameters.h"

#define DISP_SPEED_AVG		1
#define DISP_SPEED_MAX		2
#define DISP_TEMP_MIN		3
#define DISP_TEMP_MAX		4
#define OUTPUT_SHORT		0
#define OUTPUT_FULL			1
#define DISP_CURSOR_SYMBOL	0x5F

void LCD_upd_page_system_reset(void);
void LCD_upd_misc_setup_level(unsigned char);
void LCD_upd_misc_setup_selection(unsigned char);
void LCD_upd_page_error(void);
void LCD_upd_page_error_full(void);
void LCD_upd_page_confirm(void);
void LCD_upd_page_copyright(void);
void LCD_upd_page_version(void);
void LCD_upd_page_set0_header(unsigned char);
void LCD_upd_page_set0_wheel_length(void);
void LCD_upd_page_set0_magnets(void);
void LCD_upd_page_set0_time(unsigned int, unsigned char, unsigned char,
	unsigned char);
void LCD_upd_page_set0_date(unsigned char, unsigned char, unsigned char);
void LCD_upd_page_set0_ui_timeout(unsigned char);
void LCD_upd_page_set0_trip_reset(void);
void LCD_upd_page_set0_front_light(void);
void LCD_upd_page_set0_rear_light(void);
void LCD_upd_page_set0_ledtest(void);
void LCD_upd_page_set0_temperature(void);
void LCD_upd_page_set0_fullreset(void);
void LCD_upd_page_set0_odoenter(void);
void LCD_upd_page_set0_debug(unsigned int);
void LCD_upd_page_set0_thresholds(void);
void LCD_upd_page_set1_wheel_length(unsigned char, unsigned char);
void LCD_upd_page_set1_magnets(unsigned char, unsigned char);
void LCD_upd_page_set1_time(unsigned char, unsigned int, unsigned char,
	set_parameters *);
void LCD_upd_page_set1_date(unsigned char, unsigned int, unsigned char,
	set_parameters *);
void LCD_upd_page_set1_ui_timeout(unsigned char, unsigned char);
void LCD_upd_page_set1_trip_reset(unsigned char, unsigned char);
void LCD_upd_page_set1_front_light(unsigned char, unsigned char);
void LCD_upd_page_set1_rear_light(unsigned char, unsigned char);
void LCD_upd_page_set1_ledtest(unsigned char, unsigned char);
void LCD_upd_page_set1_temperature(unsigned char, unsigned char);
void LCD_upd_page_set1_fullreset(unsigned char);
void LCD_upd_page_set1_odoenter(unsigned char, unsigned int, unsigned char,
	set_parameters *);
void LCD_upd_page_set1_debug(unsigned char, unsigned char);
void LCD_upd_page_set1_thresholds(unsigned char, unsigned char);
void LCD_upd_page_set2_wdiam(unsigned char, unsigned int, unsigned char,
	set_parameters *);
void LCD_upd_page_set2_wlength(unsigned char, unsigned int, unsigned char,
	set_parameters *);
void LCD_upd_page_set2_interactive(unsigned char, unsigned char, unsigned char);
void LCD_upd_page_set2_ADC_BAT_FULL(unsigned char);
void LCD_upd_page_set2_ADC_BAT_HIGH(unsigned char);
void LCD_upd_page_set2_ADC_BAT_MID(unsigned char);
void LCD_upd_page_set2_ADC_BAT_LOW(unsigned char);
void LCD_upd_page_set2_LIGHT_AUTO_ON(unsigned char);
void LCD_upd_page_set2_LIGHT_AUTO_OFF(unsigned char);
void LCD_upd_page_set2_SPEED_L_HIGH(unsigned char);
void LCD_upd_page_set2_SPEED_L_MID_HI(unsigned char);
void LCD_upd_page_set2_SPEED_L_MID_LO(unsigned char);
void LCD_upd_page_set2_SPEED_L_OFF(unsigned char);
void LCD_upd_page_set2_SPEED_B_BLINK(unsigned char);
void LCD_upd_page_set2_thres(unsigned int, set_parameters *);
void LCD_upd_page_set3_path(unsigned char, unsigned int, unsigned char,
	set_parameters *);
void LCD_upd_page_auto_frontlight(unsigned char);
void LCD_upd_page_auto_lcd(unsigned char);
void LCD_upd_page_auto_rearlight(unsigned char);
void LCD_upd_page_speed(unsigned int);
void LCD_upd_page_avgmaxspeed(unsigned int, unsigned char);
void LCD_upd_page_clock(unsigned int, unsigned char, unsigned char, unsigned char,
	unsigned char);
void LCD_upd_page_date(unsigned char, unsigned char, unsigned char, unsigned char);
void LCD_upd_page_odo(unsigned int, unsigned char);
void LCD_upd_page_trip(unsigned long);
void LCD_upd_page_ttrip(unsigned long);
void LCD_upd_page_envtemp(unsigned char, unsigned char, unsigned char,
	unsigned char);
void LCD_upd_page_minmaxtemp(unsigned char, unsigned char, unsigned char);
void LCD_upd_page_debug(unsigned char, unsigned int, unsigned char);
void LCD_upd_page_LEDs(void);
void LCD_upd_page_battery(unsigned int, unsigned char, unsigned char,
	unsigned char, unsigned char, unsigned char);
void LCD_upd_page_sys(unsigned long);

char ch_LCD_buf[17];
