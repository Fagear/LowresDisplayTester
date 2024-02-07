/*
 * drv_io_mxx8.h
 *
 * Created:			2022-05-17 15:03:00
 * Modified:		2022-05-23
 * Author:			Maksim Kryukov aka Fagear (fagear@mail.ru)
 *
 * Supported MCUs:	ATmega88(-/A/P/AP), ATmega168(-/A/P/AP), ATmega328(-/P).
 *
 */ 

#ifndef FGR_DRV_IO_MXX8_H_
#define FGR_DRV_IO_MXX8_H_

#include <avr/io.h>

// Buttons.
#define SW1_PORT			PORTB
#define SW1_DIR				DDRB
#define SW1_SRC				PINB
#define SW2_PORT			PORTB
#define SW2_DIR				DDRB
#define SW2_SRC				PINB
#define SW_1				(1<<0)
#define SW_2				(1<<4)
#define BTN_SETUP1			SW1_DIR &= ~(SW_1)
#define BTN_SETUP2			SW1_PORT |= (SW_1)
#define BTN_SETUP3			SW2_DIR &= ~(SW_2)
#define BTN_SETUP4			SW2_PORT |= (SW_2)
#define BTN_1_STATE			(SW1_SRC&SW_1)
#define BTN_2_STATE			(SW2_SRC&SW_2)

#define VSYNC_PORT			PORTB
#define VSYNC_DIR			DDRB
#define VSYNC_PIN			(1<<2)
#define VSYNC_SETUP1		VSYNC_DIR |= VSYNC_PIN
#define VSYNC_SETUP2		VSYNC_PORT |= VSYNC_PIN
#define VSYNC_PULL_UP		VSYNC_PORT |= VSYNC_PIN
#define VSYNC_PULL_DOWN		VSYNC_PORT &= ~VSYNC_PIN

#define INT0_INT			INT0_vect					// External INT0 interrupt vector alias
#define INT0_CONFIG_REG		EICRA |= (1<<ISC01)			// Configure INT0 to trigger on falling edge
#define INT0_EN				EIMSK |= (1<<INT0)			// Enable interrupt
#define INT0_DIS			EIMSK &= ~(1<<INT0)			// Disable interrupt
#define INT0_CLEAR_FLAG		EIFR |= (1<<INTF0)			// Clear INT0 trigger
#define INT0_CONFIG_PIN1	DDRD &= ~(1<<2)				// INT0 input pin direction
#define INT0_CONFIG_PIN2	PORTD |= (1<<2)				// INT0 input pin port

// Video horizontal sync timing setup.
#define SYNC_INT			TIMER1_OVF_vect				// Timer 1 interrupt vector alias
#define SYNC_EN_INTR		TIMSK1 = (1<<TOIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<ICIE1)		// Enable interrupt
#define SYNC_DIS_INTR		TIMSK1 = (0<<TOIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<ICIE1)		// Disable interrupt
#define SYNC_CONFIG_REG1	TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(1<<WGM11)|(0<<WGM10)		// Enable Fast-PWM with TOP = ICR1
#define SYNC_CONFIG_REG2	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10)	// Start timer with clk/1 clock
#define SYNC_DATA			TCNT1L						// Count register
#define SYNC_STEP_DUR		ICR1						// TOP for counter
#define SYNC_PULSE_DUR		OCR1A						// Duty cycle register
#define SYNC_CONFIG_PIN1	PORTB &= ~(1<<1)			// Output pin port
#define SYNC_CONFIG_PIN2	DDRB |= (1<<1)				// Output pin direction

// Video active line timing setup.
#define FGR_DRV_IO_T0OC_HW_FOUND
#define LACT_INT			TIMER0_COMPA_vect									// Timer 0 interrupt vector alias
#define LACT_EN_INTR		TIMSK0 = (0<<OCIE0B)|(1<<OCIE0A)|(0<<TOIE0)			// Enable interrupt
#define LACT_DIS_INTR		TIMSK0 = (0<<OCIE0B)|(0<<OCIE0A)|(0<<TOIE0)			// Disable interrupt
#define LACT_CONFIG_REG1	TCCR0A = (1<<COM0A1)|(0<<COM0A0)|(0<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(0<<WGM00)
#define LACT_CONFIG_REG2	TCCR0B = (0<<FOC0A)|(0<<FOC0B)|(0<<WGM02)|(0<<CS02)|(0<<CS01)|(0<<CS00)
#define LACT_START			TCCR0B |= (1<<CS01)									// Start timer with clk/8 clock
#define LACT_STOP			TCCR0B &= ~((1<<CS00)|(1<<CS01)|(1<<CS02))			// Stop timer
#define LACT_OC_CLEAR1		TCCR0A &= ~(1<<COM0A0)								// Switch to "Clear OC pin on compare" (part 1)
#define LACT_OC_CLEAR2		TCCR0A |= (0<<COM0A1)								// Switch to "Clear OC pin on compare" (part 2)
#define LACT_OC_TOGGLE1		TCCR0A |= (1<<COM0A0)								// Switch to "Toggle OC pin on compare" (part 1)
#define LACT_OC_TOGGLE2		TCCR0A &= ~(0<<COM0A1)								// Switch to "Toggle OC pin on compare" (part 2)
#define LACT_OC_FORCE		TCCR0B |= (1<<FOC0A)								// Force OC event
#define LACT_DATA			TCNT0												// Count register
#define LACT_PULSE_DUR		OCR0A												// Compare register
#define LACT_CONFIG_PIN1	PORTD &= ~(1<<6)									// OC pin port
#define LACT_CONFIG_PIN2	DDRD |= (1<<6)										// OC pin direction

// Watchdog setup.
#define WDT_RESET_DIS		MCUSR &= ~(1<<WDRF)									// Clear Watchdog System Reset Flag to prevent boot-loop on unhandled Watchdog reset
#define WDT_PREP_OFF		WDTCSR |= (1<<WDCE)|(1<<WDE)						// Prepare to turn Watchdog OFF
#define WDT_SW_OFF			WDTCSR = 0x00										// Turn Watchdog OFF
#define WDT_FLUSH_REASON	MCUSR = (0<<WDRF)|(0<<BORF)|(0<<EXTRF)|(0<<PORF)	// Clear reason for MCU reset
#define WDT_PREP_ON			WDTCSR |= (1<<WDCE)|(1<<WDE)						// Prepare to turn Watchdog ON
#define WDT_SW_ON			WDTCSR = (1<<WDE)|(1<<WDP0)|(1<<WDP1)|(1<<WDP2)		// Turn Watchdog ON, MCU reset after ~2.0 s

// Power consumption optimizations.
#define PWR_COMP_OFF		ACSR |= (1<<ACD)			// Turn off Analog Comparator
#define PWR_ADC_OFF			PRR |= (1<<PRADC)			// Turn off Analog-to-Digital Converter
#define PWR_T0_OFF			PRR |= (1<<PRTIM0)			// Turn off Timer/Counter0
#define PWR_T1_OFF			PRR |= (1<<PRTIM1)			// Turn off Timer/Counter1
#define PWR_T2_OFF			PRR |= (1<<PRTIM2)			// Turn off Timer/Counter2
#define PWR_I2C_OFF			PRR |= (1<<PRTWI)			// Turn off 2-wire Serial Interface
#define PWR_SPI_OFF			PRR |= (1<<PRSPI)			// Turn off Serial Peripheral Interface
#define PWR_UART_OFF		PRR |= (1<<PRUSART0)		// Turn off Universal Synchronous and Asynchronous serial Receiver and Transmitter
#define PWR_ADC_OPT			DIDR0 |= (1<<ADC0D)|(1<<ADC1D)|(1<<ADC2D)|(1<<ADC3D)|(1<<ADC4D)		// Turn off digital buffers on used ADC inputs

// Debug output.
#ifndef CONF_NO_DEBUG_PINS
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
	PWR_COMP_OFF; PWR_ADC_OFF; PWR_T2_OFF;
	
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
	// Timer0 output compare pin is used to indicate active part of the line (and to produce black level).
	LACT_CONFIG_PIN1; LACT_CONFIG_PIN2;
	
	// Configure output for video sync.
	SYNC_CONFIG_REG1; SYNC_CONFIG_REG2;
	SYNC_STEP_DUR = 0xFFFF;
	SYNC_PULSE_DUR = SYNC_STEP_DUR/2;
	
	// Configure active line timer.
	LACT_CONFIG_REG1; LACT_CONFIG_REG2;
	LACT_PULSE_DUR = 100;
	LACT_EN_INTR;
	
	// Enable draw starting interrupts.
	// INT0 tied to Timer1 compare output is used to trigger Timer0 to generate active region pulse.
	INT0_CONFIG_PIN1; INT0_CONFIG_PIN2;
	INT0_CONFIG_REG; INT0_EN;
}

#endif /* FGR_DRV_IO_MXX8_H_ */
