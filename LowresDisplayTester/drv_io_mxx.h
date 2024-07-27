/**************************************************************************************************************************************************************
drv_io_mxx.h

Copyright © 2024 Maksim Kryukov <fagear@mail.ru>

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
This driver provides HAL for the hardware.

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_IO_MXX_H_
#define FGR_DRV_IO_MXX_H_

#undef FGR_DRV_IO_M8
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
	#define SW1_PORT			PORTC
	#define SW1_DIR				DDRC
	#define SW1_SRC				PINC
	#define SW2_PORT			PORTC
	#define SW2_DIR				DDRC
	#define SW2_SRC				PINC
	#define SW_1				(1<<6)
	#define SW_2				(1<<7)
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
	#define VSYNC_PORT			PORTD
	#define VSYNC_DIR			DDRD
	#define VSYNC_PIN			(1<<6)
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
#define SYNC_INT			INT0_vect					// External INT0 interrupt vector alias
#define SYNC_INT_CFG_REG	MCUCR |= (1<<ISC01)			// Configure INT0 to trigger on falling edge
#define SYNC_INT_EN			GICR |= (1<<INT0)			// Enable interrupt
#define SYNC_INT_DIS		GICR &= ~(1<<INT0)			// Disable interrupt
#define SYNC_IN_CFG_PIN1	DDRD &= ~(1<<2)				// INT0 input pin direction
#define SYNC_IN_CFG_PIN2	PORTD |= (1<<2)				// INT0 input pin port

// Video sync timing setup.
#define SYNC_CONFIG_NEG		TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(1<<WGM11)|(0<<WGM10)		// Enable Fast-PWM (negative pulse) with TOP = ICR1
#define SYNC_CONFIG_POS		TCCR1A = (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(1<<WGM11)|(0<<WGM10)		// Enable Fast-PWM (positive pulse) with TOP = ICR1
#define SYNC_CONFIG_RUN		TCCR1B = (0<<ICNC1)|(0<<ICES1)|(1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10)	// Start timer with clk/1 clock
#define SYNC_STEP_DUR		ICR1									// TOP for counter
#define SYNC_DATA			TCNT1									// Count register
#define SYNC_DATA_8B		TCNT1L
#define SYNC_PULSE_DUR		OCR1A									// Horizontal sync duty cycle register
#define HACT_PULSE_DUR		OCR1B									// Line active part duty cycle register
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define HSYNC_PORT			PORTB
	#define HSYNC_DIR			DDRB
	#define HSYNC_PIN			(1<<1)								// Horizontal/composite sync output
	#define HACT_PIN			(1<<2)								// Active part of the line output
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define HSYNC_PORT			PORTD
	#define HSYNC_DIR			DDRD
	#define HSYNC_PIN			(1<<5)								// Horizontal/composite sync output
	#define HACT_PIN			(1<<4)								// Active part of the line output
#endif /* FGR_DRV_IO_M8 */
#define SYNC_CONFIG_PIN1	HSYNC_PORT &= ~(HSYNC_PIN|HACT_PIN)		// Output pins port
#define SYNC_CONFIG_PIN2	HSYNC_DIR |= (HSYNC_PIN|HACT_PIN)		// Output pins direction
#define HACT_ON				TCCR1A |= ((1<<COM1B1)|(1<<COM1B0))		// Enable drive for "active part" output
#define HACT_OFF			TCCR1A &= ~((1<<COM1B1)|(1<<COM1B0))	// Disable drive for "active part" output

// Video line active part timing setup.
#define LACT_COMP_INT		TIMER2_COMP_vect					// Timer 2 Compare interrupt vector alias
#define LACT_OVF_INT		TIMER2_OVF_vect						// Timer 2 Overflow interrupt vector alias
#define LACT_EN_INTR		TIMSK |= (1<<OCIE2)|(1<<TOIE2)		// Enable interrupts
#define LACT_DIS_INTR		TIMSK &= ~((1<<OCIE2)|(1<<TOIE2))	// Disable interrupts
#define LACT_CONFIG_REG1	TCCR2 = (0<<FOC2)|(0<<WGM20)|(0<<COM21)|(0<<COM20)|(0<<WGM21)|(0<<CS22)|(0<<CS21)|(0<<CS20)	// Normal mode
#define LACT_CONFIG_REG2	
#define LACT_START			TCCR2 |= (1<<CS21)					// Start timer with clk/8 clock
#define LACT_STOP			TCCR2 &= ~((1<<CS22)|(1<<CS21)|(1<<CS20))	// Stop timer
#define LACT_DATA			TCNT2								// Count register
#define LACT_PULSE_DUR		OCR2								// Compare register

// HD44780-compatible character display.
#ifdef CONF_EN_HD44780P
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
		#define HD44780DATA_DIR		DDRA
		#define HD44780DATA_PORT	PORTA
		#define HD44780DATA_SRC		PINA
		#define HD44780_A0			(1<<7)
		#define HD44780_RW			(1<<6)
		#define HD44780_E			(1<<5)
		#define HD44780_D4			(1<<3)
		#define HD44780_D5			(1<<2)
		#define HD44780_D6			(1<<1)
		#define HD44780_D7			(1<<0)
	#endif /* FGR_DRV_IO_M8 */
#endif /* CONF_EN_HD44780P */

#ifdef FGR_DRV_UART_HW_FOUND
	// UART busy pin.
	#ifdef FGR_DRV_IO_M8
		// ATmega8(-/A).
		#define UART_BUSY_DIR		DDRD
		#define UART_BUSY_PORT		PORTD
		#define UART_BUSY_PIN		(1<<4)
	#else
		// ATmega16(-/A), ATmega32(-/A).
		#define UART_BUSY_DIR		DDRD
		#define UART_BUSY_PORT		PORTD
		#define UART_BUSY_PIN		(1<<3)
	#endif /* FGR_DRV_IO_M8 */
#endif	/* FGR_DRV_UART_HW_FOUND */

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

#ifdef CONF_EN_HD44780P
	#define CONF_NO_DEBUG_PINS
#endif

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
		#define DBG_SETUP1			DBG_PORT1 &= ~(DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4)
		#define DBG_SETUP2			DBG_DIR1 |= (DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4)
		#define DBG_1_ON			DBG_PORT1 |= DBG_OUT1
		#define DBG_1_OFF			DBG_PORT1 &= ~DBG_OUT1
		#define DBG_2_ON			DBG_PORT1 |= DBG_OUT2
		#define DBG_2_OFF			DBG_PORT1 &= ~DBG_OUT2
		#define DBG_3_ON			DBG_PORT1 |= DBG_OUT3
		#define DBG_3_OFF			DBG_PORT1 &= ~DBG_OUT3
		#define DBG_4_ON			DBG_PORT1 |= DBG_OUT4
		#define DBG_4_OFF			DBG_PORT1 &= ~DBG_OUT4
	#else
		// ATmega16(-/A), ATmega32(-/A).
		#define DBG_PORT1			PORTC
		#define DBG_DIR1			DDRC
		#define DBG_OUT1			(1<<2)
		#define DBG_OUT2			(1<<3)
		#define DBG_OUT3			(1<<4)
		#define DBG_OUT4			(1<<5)
		#define DBG_SETUP1			DBG_PORT1 &= ~(DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4)
		#define DBG_SETUP2			DBG_DIR1 |= (DBG_OUT1|DBG_OUT2|DBG_OUT3|DBG_OUT4)
		#define DBG_1_ON			DBG_PORT1 |= DBG_OUT1
		#define DBG_1_OFF			DBG_PORT1 &= ~DBG_OUT1
		#define DBG_2_ON			DBG_PORT1 |= DBG_OUT2
		#define DBG_2_OFF			DBG_PORT1 &= ~DBG_OUT2
		#define DBG_3_ON			DBG_PORT1 |= DBG_OUT3
		#define DBG_3_OFF			DBG_PORT1 &= ~DBG_OUT3
		#define DBG_4_ON			DBG_PORT1 |= DBG_OUT4
		#define DBG_4_OFF			DBG_PORT1 &= ~DBG_OUT4
	#endif /* FGR_DRV_IO_M8 */
#else
	#define DBG_SETUP1
	#define DBG_SETUP2
	#define DBG_1_ON
	#define DBG_1_OFF
	#define DBG_2_ON
	#define DBG_2_OFF
	#define DBG_3_ON
	#define DBG_3_OFF
	#define DBG_4_ON
	#define DBG_4_OFF
#endif /* CONF_NO_DEBUG_PINS */

#endif /* FGR_DRV_IO_MXX_H_ */
