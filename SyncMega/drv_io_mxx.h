/**************************************************************************************************************************************************************
drv_io_mxx.h

Copyright � 2024 Maksim Kryukov <fagear@mail.ru>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Created: 2022-05

Part of the [LowresDisplayTester] project.
Part of the I/O and peripheral hardware driver for AVR MCUs.
This driver provides HAL and startup initialization of the hardware.

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_IO_MXX_H_
#define FGR_DRV_IO_MXX_H_

#undef FGR_DRV_IO_M8
#undef FGR_DRV_IO_T0OC_HW_FOUND
#ifdef WDCE
	// Detect ATmega8 and ATmega8A.
	#define FGR_DRV_IO_M8
#endif /* WDCE */

// Buttons.
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define SW1_PORT			PORTB
	#define SW1_DIR				DDRB
	#define SW1_SRC				PINB
	#define SW2_PORT			PORTB
	#define SW2_DIR				DDRB
	#define SW2_SRC				PINB
	#define SW_1				(1<<0)
	#define SW_2				(1<<4)
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define SW1_PORT			PORTB
	#define SW1_DIR				DDRB
	#define SW1_SRC				PINB
	#define SW2_PORT			PORTB
	#define SW2_DIR				DDRB
	#define SW2_SRC				PINB
	#define SW_1				(1<<0)
	#define SW_2				(1<<1)
#endif
#define BTN_SETUP1			SW1_DIR &= ~(SW_1)
#define BTN_SETUP2			SW1_PORT |= (SW_1)
#define BTN_SETUP3			SW2_DIR &= ~(SW_2)
#define BTN_SETUP4			SW2_PORT |= (SW_2)
#define BTN_1_STATE			(SW1_SRC&SW_1)
#define BTN_2_STATE			(SW2_SRC&SW_2)

// Vertical sync output for VGA.
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define VSYNC_PORT			PORTB
	#define VSYNC_DIR			DDRB
	#define VSYNC_PIN			(1<<2)
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define VSYNC_PORT			PORTD
	#define VSYNC_DIR			DDRD
	#define VSYNC_PIN			(1<<6)
#endif /* FGR_DRV_IO_M8 */
#define VSYNC_SETUP1		VSYNC_DIR |= VSYNC_PIN
#define VSYNC_SETUP2		VSYNC_PORT |= VSYNC_PIN
#define VSYNC_PULL_UP		VSYNC_PORT |= VSYNC_PIN
#define VSYNC_PULL_DOWN		VSYNC_PORT &= ~VSYNC_PIN

// Feedback trigger for sync generated with Timer 1.
// (INT0 starts 0.5us faster than Timer's own compare interrupt)
#define INT0_INT			INT0_vect					// External INT0 interrupt vector alias
#define INT0_CONFIG_REG		MCUCR |= (1<<ISC01)			// Configure INT0 to trigger on falling edge
#define INT0_EN				GICR |= (1<<INT0)			// Enable interrupt
#define INT0_DIS			GICR &= ~(1<<INT0)			// Disable interrupt
#define INT0_CLEAR_FLAG		GIFR |= (1<<INTF0)			// Clear INT0 trigger
#define INT0_CONFIG_PIN1	DDRD &= ~(1<<2)				// INT0 input pin direction
#define INT0_CONFIG_PIN2	PORTD |= (1<<2)				// INT0 input pin port

// Video sync timing setup.
#define SYNC_CONFIG_NEG		TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(1<<WGM11)|(0<<WGM10)		// Enable Fast-PWM (negative pulse) with TOP = ICR1
#define SYNC_CONFIG_POS		TCCR1A = (1<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(1<<WGM11)|(0<<WGM10)		// Enable Fast-PWM (positive pulse) with TOP = ICR1
#define SYNC_CONFIG_RUN		TCCR1B = (0<<ICNC1)|(0<<ICES1)|(1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10)	// Start timer with clk/1 clock
#define SYNC_STEP_DUR		ICR1									// TOP for counter
#define SYNC_DATA			TCNT1									// Count register
#define SYNC_PULSE_DUR		OCR1A									// Duty cycle register
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define SYNC_CONFIG_PIN1	PORTB &= ~(1<<1)					// Output pin port
	#define SYNC_CONFIG_PIN2	DDRB |= (1<<1)						// Output pin direction
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define SYNC_CONFIG_PIN1	PORTD &= ~(1<<5)					// Output pin port
	#define SYNC_CONFIG_PIN2	DDRD |= (1<<5)						// Output pin direction
#endif /* FGR_DRV_IO_M8 */

#ifdef FGR_DRV_IO_M8
	#warning Partially supported MCU, video bars generation will not work!
#else
	// Video active line timing setup.
	#define FGR_DRV_IO_T0OC_HW_FOUND
	#define LACT_COMP_INT		TIMER0_COMP_vect					// Timer 0 Compare interrupt vector alias
	#define LACT_EN_INTR		TIMSK |= (1<<OCIE0)|(0<<TOIE0)		// Enable interrupt
	#define LACT_DIS_INTR		TIMSK &= ~((1<<OCIE0)|(1<<TOIE0))	// Disable interrupt
	#define LACT_CONFIG_REG1	TCCR0 = (0<<FOC0)|(0<<WGM00)|(0<<COM01)|(0<<COM00)|(1<<WGM01)|(0<<CS02)|(0<<CS01)|(0<<CS00)
	#define LACT_CONFIG_REG2	
	#define LACT_START			TCCR0 |= (1<<CS01)					// Start timer with clk/8 clock
	#define LACT_STOP			TCCR0 &= ~((1<<CS00)|(1<<CS01)|(1<<CS02))	// Stop timer
	#define LACT_OC_DIS			TCCR0 &= ~((1<<COM00)|(1<<COM01))			// Disable OC operation
	#define LACT_OC_CLEAR		TCCR0 |= (1<<COM01)					// Switch to "Clear OC pin on compare"
	#define LACT_OC_SET			TCCR0 |= (1<<COM01)|(1<<COM00)		// Switch to "Set OC pin on compare"
	#define LACT_OC_TOGGLE		TCCR0 |= (1<<COM00)					// Switch to "Toggle OC pin on compare"
	#define LACT_OC_FORCE		TCCR0 |= (1<<FOC0)					// Force OC event
	#define LACT_DATA			TCNT0								// Count register
	#define LACT_PULSE_DUR		OCR0								// Compare register
	#define LACT_CONFIG_PIN1	PORTB &= ~(1<<3)					// OC pin port
	#define LACT_CONFIG_PIN2	DDRB |= (1<<3)						// OC pin direction
#endif /* FGR_DRV_IO_M8 */

// HD44780-compatible character display.
#ifdef CONF_EN_HD44780
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define HD44780CTRL_DIR		DDRD
	#define HD44780CTRL_PORT	PORTD
	#define HD44780DATA_DIR		DDRC
	#define HD44780DATA_PORT	PORTC
	#define HD44780DATA_SRC		PINC
	#define HD44780_A0			(1<<7)
	#define HD44780_RW			(1<<5)
	#define HD44780_E			(1<<3)
	#define HD44780_D4			(1<<0)
	#define HD44780_D5			(1<<1)
	#define HD44780_D6			(1<<2)
	#define HD44780_D7			(1<<3)
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define HD44780CTRL_DIR		DDRA
	#define HD44780CTRL_PORT	PORTA
	#define HD44780DATA_DIR		DDRC
	#define HD44780DATA_PORT	PORTC
	#define HD44780DATA_SRC		PINC
	#define HD44780_A0			(1<<5)
	#define HD44780_RW			(1<<6)
	#define HD44780_E			(1<<7)
	#define HD44780_D4			(1<<5)
	#define HD44780_D5			(1<<4)
	#define HD44780_D6			(1<<3)
	#define HD44780_D7			(1<<2)
#endif /* FGR_DRV_IO_M8 */
#endif /* CONF_EN_HD44780 */

// Watchdog setup.
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define WDT_RESET_DIS		MCUCSR &= ~(1<<WDRF)								// Clear Watchdog System Reset Flag to prevent boot-loop on unhandled Watchdog reset
	#define WDT_PREP_OFF		WDTCR |= (1<<WDCE)|(1<<WDE)							// Prepare to turn Watchdog OFF
	#define WDT_SW_OFF			WDTCR = 0x00										// Turn Watchdog OFF
	#define WDT_FLUSH_REASON	MCUCSR = (0<<WDRF)|(0<<BORF)|(0<<EXTRF)|(0<<EXTRF)	// Clear reason for MCU reset
	#define WDT_PREP_ON			WDTCR |= (1<<WDCE)|(1<<WDE)							// Prepare to turn Watchdog ON
	#define WDT_SW_ON			WDTCR = (1<<WDE)|(1<<WDP0)|(1<<WDP1)|(1<<WDP2)		// Turn Watchdog ON, MCU reset after ~2.0 s
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define WDT_RESET_DIS		MCUCSR &= ~(1<<WDRF)								// Clear Watchdog System Reset Flag to prevent boot-loop on unhandled Watchdog reset
	#define WDT_PREP_OFF		WDTCR |= (1<<WDTOE)|(1<<WDE)						// Prepare to turn Watchdog OFF
	#define WDT_SW_OFF			WDTCR = 0x00										// Turn Watchdog OFF
	#define WDT_FLUSH_REASON	MCUCSR = (0<<JTRF)|(0<<WDRF)|(0<<BORF)|(0<<EXTRF)|(0<<EXTRF)	// Clear reason for MCU reset
	#define WDT_PREP_ON			WDTCR |= (1<<WDTOE)|(1<<WDE)						// Prepare to turn Watchdog ON
	#define WDT_SW_ON			WDTCR = (1<<WDE)|(1<<WDP0)|(1<<WDP1)|(1<<WDP2)		// Turn Watchdog ON, MCU reset after ~2.0 s
#endif /* FGR_DRV_IO_M8 */

// Power consumption optimizations.
#define PWR_COMP_OFF			ACSR |= (1<<ACD)			// Turn off Analog Comparator

// Debug output.
#ifndef CONF_NO_DEBUG_PINS
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define DBG_PORT1			PORTC
	#define DBG_DIR1			DDRC
	#define DBG_OUT1			(1<<0)
	#define DBG_OUT2			(1<<1)
	#define DBG_OUT3			(1<<2)
	#define DBG_OUT4			(1<<3)
	#define DBG_PORT2			PORTB
	#define DBG_DIR2			DDRB
	#define DBG_OUT5			(1<<0)
	#define DBG_SETUP1			DBG_PORT1 &= ~(DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4)
	#define DBG_SETUP2			DBG_DIR1 |= (DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4)
	#define DBG_SETUP3			DBG_PORT2 &= ~(DBG_OUT5)
	#define DBG_SETUP4			DBG_DIR2 |= (DBG_OUT5)
	#define DBG_1_ON			DBG_PORT1 |= DBG_OUT1
	#define DBG_1_OFF			DBG_PORT1 &= ~DBG_OUT1
	#define DBG_2_ON			DBG_PORT1 |= DBG_OUT2
	#define DBG_2_OFF			DBG_PORT1 &= ~DBG_OUT2
	#define DBG_3_ON			DBG_PORT1 |= DBG_OUT3
	#define DBG_3_OFF			DBG_PORT1 &= ~DBG_OUT3
	#define DBG_4_ON			DBG_PORT1 |= DBG_OUT4
	#define DBG_4_OFF			DBG_PORT1 &= ~DBG_OUT4
	#define DBG_5_ON			DBG_PORT2 |= DBG_OUT5
	#define DBG_5_OFF			DBG_PORT2 &= ~DBG_OUT5
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define DBG_PORT1			PORTA
	#define DBG_DIR1			DDRA
	#define DBG_OUT1			(1<<7)
	#define DBG_OUT2			(1<<6)
	#define DBG_OUT3			(1<<5)
	#define DBG_OUT4			(1<<4)
	#define DBG_OUT5			(1<<3)
	#define DBG_SETUP1			DBG_PORT1 &= ~(DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4|DBG_OUT5)
	#define DBG_SETUP2			DBG_DIR1 |= (DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4|DBG_OUT5)
	#define DBG_SETUP3			
	#define DBG_SETUP4			
	#define DBG_1_ON			DBG_PORT1 |= DBG_OUT1
	#define DBG_1_OFF			DBG_PORT1 &= ~DBG_OUT1
	#define DBG_2_ON			DBG_PORT1 |= DBG_OUT2
	#define DBG_2_OFF			DBG_PORT1 &= ~DBG_OUT2
	#define DBG_3_ON			DBG_PORT1 |= DBG_OUT3
	#define DBG_3_OFF			DBG_PORT1 &= ~DBG_OUT3
	#define DBG_4_ON			DBG_PORT1 |= DBG_OUT4
	#define DBG_4_OFF			DBG_PORT1 &= ~DBG_OUT4
	#define DBG_5_ON			DBG_PORT1 |= DBG_OUT5
	#define DBG_5_OFF			DBG_PORT1 &= ~DBG_OUT5
#endif /* FGR_DRV_IO_M8 */
#else
	#define DBG_SETUP1
	#define DBG_SETUP2
	#define DBG_SETUP3
	#define DBG_SETUP4
	#define DBG_1_ON
	#define DBG_1_OFF
	#define DBG_2_ON
	#define DBG_2_OFF
	#define DBG_3_ON
	#define DBG_3_OFF
	#define DBG_4_ON
	#define DBG_4_OFF
	#define DBG_5_ON
	#define DBG_5_OFF
#endif /* CONF_NO_DEBUG_PINS */

//-------------------------------------- IO initialization.
inline void HW_init(void)
{
	// Turn off not used devices for power saving.
	PWR_COMP_OFF;
	
	// Init SPI interface.
#ifdef FGR_DRV_SPI_HW_FOUND
	SPI_init_HW();
#endif /* FGR_DRV_SPI_HW_FOUND */
#ifdef FGR_DRV_UARTSPI_HW_FOUND
	UART_SPI_init_HW();
#endif /* FGR_DRV_UARTSPI_HW_FOUND */
#ifdef FGR_DRV_I2C_HW_FOUND
	I2C_init_HW();
#endif /* FGR_DRV_I2C_HW_FOUND */
	
	// Enable debug pins.
	DBG_SETUP1; DBG_SETUP2; DBG_SETUP3; DBG_SETUP4;
	
	// Set inputs.
	BTN_SETUP1; BTN_SETUP2; BTN_SETUP3; BTN_SETUP4;
	
	// Set outputs.
	VSYNC_SETUP1; VSYNC_SETUP2;
	// Timer1 output compare pin is used to generate sync signal for video.
	// Timer is re-tuned every horizontal line to produce various impulses.
	SYNC_CONFIG_PIN1; SYNC_CONFIG_PIN2;
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
	// Timer0 output compare pin is used to indicate active part of the line (and to produce black level).
	LACT_CONFIG_PIN1; LACT_CONFIG_PIN2;
#endif
	
	// Configure output for video sync.
	SYNC_CONFIG_NEG; SYNC_CONFIG_RUN;
	SYNC_STEP_DUR = 0xFFFF;
	SYNC_PULSE_DUR = SYNC_STEP_DUR/2;
	
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
	// Configure active line timer.
	LACT_CONFIG_REG1; LACT_CONFIG_REG2;
	LACT_PULSE_DUR = 0xFF;
	LACT_START;
#endif
	
	// Enable draw starting interrupts.
	// INT0 tied to Timer1 compare output is used to re-configure PWM for H-sync.
	INT0_CONFIG_PIN1; INT0_CONFIG_PIN2;
	INT0_CONFIG_REG; INT0_EN;
}

#endif /* FGR_DRV_IO_MXX_H_ */
