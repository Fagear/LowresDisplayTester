/*
 * drv_mxx.h
 *
 * Created:			2022-05-20 13:21:37
 * Modified:		2022-05-20
 * Author:			Maksim Kryukov aka Fagear (fagear@mail.ru)
 *
 * Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A).
 *
 */

#ifndef FGR_DRV_IO_MXX_H_
#define FGR_DRV_IO_MXX_H_

#include <avr/io.h>

#define NO_DEBUG_PINS

#undef FGR_DRV_IO_M8
#undef FGR_DRV_IO_T0OC_HW_FOUND
#ifdef WDCE
	// Detect ATmega8 and ATmega8A.
	#define FGR_DRV_IO_M8
#endif

// Buttons.
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define SW1_PORT			PORTB
	#define SW1_DIR				DDRB
	#define SW1_SRC				PINB
	#define SW2_PORT			PORTC
	#define SW2_DIR				DDRC
	#define SW2_SRC				PINC
	#define SW_1				(1<<0)
	#define SW_2				(1<<6)
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define SW1_PORT			PORTB
	#define SW1_DIR				DDRB
	#define SW1_SRC				PINB
	#define SW_1				(1<<0)
	#define SW_2				(1<<1)
#endif
#define BTN_SETUP1			SW1_DIR |= (SW_1)
#define BTN_SETUP2			SW1_PORT &= ~(SW_1)
#define BTN_SETUP3			SW2_DIR &= ~(SW_2)
#define BTN_SETUP4			SW2_PORT |= (SW_2)
#define BTN_1_STATE			(SW1_SRC&SW_1)
#define BTN_2_STATE			(SW2_SRC&SW_2)

#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define VSYNC_PORT			PORTB
	#define VSYNC_DIR			DDRB
	#define VSYNC_PIN			(1<<0)
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define VSYNC_PORT			PORTD
	#define VSYNC_DIR			DDRD
	#define VSYNC_PIN			(1<<6)
#endif
#define VSYNC_SETUP1		VSYNC_DIR &= ~VSYNC_PIN
#define VSYNC_SETUP2		VSYNC_PORT |= VSYNC_PIN
#define VSYNC_PULL_UP		VSYNC_PORT |= VSYNC_PIN
#define VSYNC_PULL_DOWN		VSYNC_PORT &= ~VSYNC_PIN

#define INT0_INT			INT0_vect					// External INT0 interrupt vector alias
#define INT0_CONFIG_REG		MCUCR |= (1<<ISC01)			// Configure INT0 to trigger on falling edge
#define INT0_EN				GICR |= (1<<INT0)			// Enable interrupt
#define INT0_DIS			GICR &= ~(1<<INT0)			// Disable interrupt
#define INT0_CLEAR_FLAG		GIFR |= (1<<INTF0)			// Clear INT0 trigger
#define INT0_CONFIG_PIN1	DDRD &= ~(1<<2)				// INT0 input pin direction
#define INT0_CONFIG_PIN2	PORTD |= (1<<2)				// INT0 input pin port

// Video sync timing setup.
#define VTIM_INT			TIMER1_OVF_vect				// Timer 1 interrupt vector alias
#define VTIM_EN_INTR		TIMSK |= (1<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1)		// Enable interrupt
#define VTIM_DIS_INTR		TIMSK &= ~(1<<TICIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1)	// Disable interrupt
#define VTIM_CONFIG_REG1	TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(1<<WGM11)|(0<<WGM10)		// Enable Fast-PWM with TOP = ICR1
#define VTIM_CONFIG_REG2	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10)	// Start timer with clk/1 clock
#define VTIM_DATA			TCNT1						// Count register
#define VTIM_STEP_DUR		ICR1						// TOP for counter
#define VTIM_PULSE_DUR		OCR1A						// PWM duty cycle register
#ifdef FGR_DRV_IO_M8
	// ATmega8(-/A).
	#define VTIM_CONFIG_PIN1	PORTB &= ~(1<<1)		// PWM output pin port
	#define VTIM_CONFIG_PIN2	DDRB |= (1<<1)			// PWM output pin direction
#else
	// ATmega16(-/A), ATmega32(-/A).
	#define VTIM_CONFIG_PIN1	PORTD &= ~(1<<5)		// PWM output pin port
	#define VTIM_CONFIG_PIN2	DDRD |= (1<<5)			// PWM output pin direction
#endif

#ifndef FGR_DRV_IO_M8
	// Video active line timing setup.
	#define FGR_DRV_IO_T0OC_HW_FOUND
	#define LACT_INT			TIMER0_COMP_vect					// Timer 0 interrupt vector alias
	#define LACT_EN_INTR		TIMSK |= (1<<OCIE0)|(0<<TOIE0)		// Enable interrupt
	#define LACT_DIS_INTR		TIMSK &= ~(1<<OCIE0)|(1<<TOIE0)		// Disable interrupt
	#define LACT_CONFIG_REG1	TCCR0 = (0<<FOC0)|(0<<WGM00)|(1<<COM01)|(0<<COM00)|(1<<WGM01)|(0<<CS02)|(0<<CS01)|(0<<CS00)
	#define LACT_CONFIG_REG2	NOP
	#define LACT_START			TCCR0 |= (1<<CS01)					// Start timer with clk/8 clock
	#define LACT_STOP			TCCR0 &= ~(1<<CS01)					// Stop timer
	#define LACT_OC_CLEAR1		TCCR0 &= ~(1<<COM00)				// Switch to "Clear OC pin on compare" (part 1)
	#define LACT_OC_CLEAR2		TCCR0 |= (0<<COM01)					// Switch to "Clear OC pin on compare" (part 2)
	#define LACT_OC_TOGGLE1		TCCR0 |= (1<<COM00)					// Switch to "Toggle OC pin on compare" (part 1)
	#define LACT_OC_TOGGLE2		TCCR0 &= ~(0<<COM01)				// Switch to "Toggle OC pin on compare" (part 2)
	#define LACT_OC_FORCE		TCCR0 |= (1<<FOC0)					// Force OC event
	#define LACT_DATA			TCNT0								// Count register
	#define LACT_PULSE_DUR		OCR0								// Compare register
	#define LACT_CONFIG_PIN1	PORTB &= ~(1<<3)					// OC pin port
	#define LACT_CONFIG_PIN2	DDRB |= (1<<3)						// OC pin direction
#endif

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
#endif

// Power consumption optimizations.
#define PWR_COMP_OFF		ACSR |= (1<<ACD)			// Turn off Analog Comparator

// Debug output.
#ifndef NO_DEBUG_PINS
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
	#define DBG_SETUP3			NOP
	#define DBG_SETUP4			NOP
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
#endif

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
	VTIM_CONFIG_PIN1; VTIM_CONFIG_PIN2;
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
	LACT_CONFIG_PIN1; LACT_CONFIG_PIN2;
#endif
	
	// Configure output for video sync.
	VTIM_CONFIG_REG1; VTIM_CONFIG_REG2;
	VTIM_STEP_DUR = 0xFFFF;
	VTIM_PULSE_DUR = VTIM_STEP_DUR/2;
	
#ifdef FGR_DRV_IO_T0OC_HW_FOUND
	// Configure active line timer.
	LACT_CONFIG_REG1; LACT_CONFIG_REG2;
	LACT_PULSE_DUR = 127;
	LACT_EN_INTR;
#endif
	
	// Enable draw starting interrupts.
	INT0_CONFIG_PIN1; INT0_CONFIG_PIN2;
	INT0_CONFIG_REG; INT0_EN;
}

#endif /* FGR_DRV_IO_MXX_H_ */
