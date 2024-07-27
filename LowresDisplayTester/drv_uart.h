/**************************************************************************************************************************************************************
drv_uart.h

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

Created: 2009-12

Part of the [LowresDisplayTester] project.
UART driver for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

The driver is buffer-based, both on transmit and receive sides.
The driver is interrupt-based and targeted for real-time systems with no wait loops inside it.
Buffer length is configurable via defines [UART_IN_LEN] and [UART_OUT_LEN].
The driver can set UART speed on-the-fly, using [UART_BAUD_xxxx] defines
and [F_CPU] define for CPU clock (in Hz).

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A), ATmega48(-/A/P/AP), ATmega88(-/A/P/AP), ATmega168(-/A/P/AP), ATmega328(-/P).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_UART
#define FGR_DRV_UART

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "config.h"		// Contains [UART_IN_LEN], [UART_OUT_LEN] and [UART_EN_OVWR].
#include "drv_CPU.h"	// Contains [F_CPU].

#undef FGR_DRV_UART_Mxx
#undef FGR_DRV_UART_Mxx8

#if SIGNATURE_1 == 0x92
	#if SIGNATURE_2 == 0x05			// ATmega48, ATmega48A
		#define  FGR_DRV_UART_Mxx8
	#elif SIGNATURE_2 == 0x0A		// ATmega48P, ATmega48AP
		#define  FGR_DRV_UART_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x93
	#if SIGNATURE_2 == 0x07			// ATmega8, ATmega8A
		#define FGR_DRV_UART_Mxx
	#elif SIGNATURE_2 == 0x0A		// ATmega88, ATmega88A
		#define  FGR_DRV_UART_Mxx8
	#elif SIGNATURE_2 == 0x0F		// ATmega88P, ATmega88PA
		#define  FGR_DRV_UART_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x94
	#if SIGNATURE_2 == 0x03			// ATmega16, ATmega16A
		#define FGR_DRV_UART_Mxx
	#elif SIGNATURE_2 == 0x06		// ATmega168, ATmega168A
		#define  FGR_DRV_UART_Mxx8
	#elif SIGNATURE_2 == 0x0B		// ATmega168P, ATmega168PA
		#define  FGR_DRV_UART_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x95
	#if SIGNATURE_2 == 0x02			// ATmega32, ATmega32A
		#define FGR_DRV_UART_Mxx
	#elif SIGNATURE_2 == 0x14		// ATmega328
		#define  FGR_DRV_UART_Mxx8
	#elif SIGNATURE_2 == 0x0F		// ATmega328P
		#define  FGR_DRV_UART_Mxx8
	#endif /* SIGNATURE_2 */
#endif /* SIGNATURE_1 */

#ifdef FGR_DRV_UART_Mxx
	#define FGR_DRV_UART_HW_FOUND
	#define UART_PORT			PORTD
	#define UART_DIR			DDRD
	#define UART_SRC			PIND
	#define UART_TX				(1<<1)
	#define UART_RX				(1<<0)
	#define UART_RX_INT			USART_RXC_vect
	#define UART_TX_INT			USART_TXC_vect
	#define UART_CONF1_REG		UCSRA
	#define UART_CONF2_REG		UCSRB
	#define UART_SPD_H_REG		UBRRH
	#define UART_SPD_L_REG		UBRRL
	#define UART_DATA_REG		UDR
	#define UART_STATE_REG		UCSRA
	#define UART_DBL_SPEED		(1<<U2X)
	#define UART_RX_EN			UART_CONF2_REG |= (1<<RXEN)
	#define UART_TX_EN			UART_CONF2_REG |= (1<<TXEN)
	#define UART_RX_DIS			UART_CONF2_REG &= ~(1<<RXEN)
	#define UART_TX_DIS			UART_CONF2_REG &= ~(1<<TXEN)
	#define UART_DIS			UART_CONF2_REG &= ~((1<<RXEN)|(1<<TXEN))
	#define UART_RX_INT_EN		UART_CONF2_REG |= (1<<RXCIE)
	#define UART_TX_INT_EN		UART_CONF2_REG |= (1<<TXCIE)
	#define UART_RX_INT_DIS		UART_CONF2_REG &= ~(1<<RXCIE)
	#define UART_TX_INT_DIS		UART_CONF2_REG &= ~(1<<TXCIE)
	#define UART_RX_COMPLETE	(1<<RXC)
	#define UART_DATA_EMPTY		(1<<UDRE)
	#define UART_FRAME_ERR		(1<<FE)
	#ifdef PE
		#define UART_PARITY_ERR		(1<<PE)
	#else
		#define UART_PARITY_ERR		(1<<UPE)
	#endif /* PE */
	#define UART_DATA_OVERRUN	(1<<DOR)
	#define UART_MODE_8N1		UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0)
#elif defined(FGR_DRV_UART_Mxx8)
	#define FGR_DRV_UART_HW_FOUND
	#define UART_PORT			PORTD
	#define UART_DIR			DDRD
	#define UART_SRC			PIND
	#define UART_TX				(1<<1)
	#define UART_RX				(1<<0)
	#define UART_RX_INT			USART_RX_vect
	#define UART_TX_INT			USART_TX_vect
	#define UART_CONF1_REG		UCSR0A
	#define UART_CONF2_REG		UCSR0B
	#define UART_CONF3_REG		UCSR0C
	#define UART_SPD_H_REG		UBRR0H
	#define UART_SPD_L_REG		UBRR0L
	#define UART_DATA_REG		UDR0
	#define UART_STATE_REG		UCSR0A
	#define UART_DBL_SPEED		(1<<U2X0)
	#define UART_RX_EN			UART_CONF2_REG |= (1<<RXEN0)
	#define UART_TX_EN			UART_CONF2_REG |= (1<<TXEN0)
	#define UART_RX_DIS			UART_CONF2_REG &= ~(1<<RXEN0)
	#define UART_TX_DIS			UART_CONF2_REG &= ~(1<<TXEN0)
	#define UART_DIS			UART_CONF2_REG &= ~((1<<RXEN0)|(1<<TXEN0))
	#define UART_RX_INT_EN		UART_CONF2_REG |= (1<<RXCIE0)
	#define UART_TX_INT_EN		UART_CONF2_REG |= (1<<TXCIE0)
	#define UART_RX_INT_DIS		UART_CONF2_REG &= ~(1<<RXCIE0)
	#define UART_TX_INT_DIS		UART_CONF2_REG &= ~(1<<TXCIE0)
	#define UART_RX_COMPLETE	(1<<RXC0)
	#define UART_DATA_EMPTY		(1<<UDRE0)
	#define UART_FRAME_ERR		(1<<FE0)
	#define UART_PARITY_ERR		(1<<UPE0)
	#define UART_DATA_OVERRUN	(1<<DOR0)
	#define UART_MODE_8N1		UCSR0C=(1<<UCSZ01)|(1<<UCSZ00)
#endif /* FGR_DRV_UART_Mxx */

#ifndef CONF_EN_UART
	#undef FGR_DRV_UART_HW_FOUND
#endif /* CONF_EN_UART */

#ifdef FGR_DRV_UART_HW_FOUND

// Speed defines (for use in [UART_set_speed()]).
#define UART_BAUD_1200			(F_CPU/1200/8-1)
#define UART_BAUD_2400			(F_CPU/2400/8-1)
#define UART_BAUD_4800			(F_CPU/4800/8-1)
#define UART_BAUD_9600			(F_CPU/9600/8-1)
#define UART_BAUD_19200			(F_CPU/19200/8-1)
#define UART_BAUD_38400			(F_CPU/38400/8-1)
#define UART_BAUD_57600			(F_CPU/57600/8-1)
#define UART_BAUD_250k			(F_CPU/250000/8-1)
#define UART_BAUD_500k			(F_CPU/500000/8-1)
#define UART_BAUD_1M			(F_CPU/1000000/8-1)

// Buffer sizes.
#define UART_INPUT_BUF_LEN		UART_IN_LEN		// For receiving buffer.
#define UART_OUTPUT_BUF_LEN		UART_OUT_LEN	// For transmitting buffer.

#if UART_INPUT_BUF_LEN>65535U
	#error Buffer size more than 65535 (16-bit) is not supported! (UART_INPUT_BUF_LEN)
#endif
#if UART_OUTPUT_BUF_LEN>65535U
	#error Buffer size more than 65535 (16-bit) is not supported! (UART_OUTPUT_BUF_LEN)
#endif

// Input string type.
#define UART_CHAR		0	// char* from RAM
#define UART_ROM		1	// uint8_t* from ROM

// Interrupt header and footer if [ISR_NAKED] is used.
#define INTR_UART_IN	asm volatile("push	r1\npush	r0\nin	r0, 0x3f\npush	r0\neor	r1, r1\npush	r24\npush	r25\npush	r30\npush	r31\n")
#define INTR_UART_OUT	asm volatile("pop	r31\npop	r30\npop	r25\npop	r24\npop	r0\nout	0x3f, r0\npop	r0\npop	r1")

// Allow transmitting buffer to be flushed and overwritten if overflow occurs.
//#define UART_EN_OVWR	1	

inline void UART_init_HW(void)
{
	UART_PORT &= ~(UART_RX|UART_TX);
	UART_DIR &= ~UART_RX;
	UART_DIR |= UART_TX;
};

void UART_set_speed(uint16_t);					// Set UART speed with "UART_BAUD_xxxx" defines.
void UART_enable(void);							// Enable UART hardware.
void UART_add_string(const char*);				// Add char* string into transmitting buffer (buffer length in [UART_OUTPUT_BUF_LEN]).
void UART_add_flash_string(const uint8_t*);		// Add string from PROGMEM into transmitting buffer (buffer length in [UART_OUTPUT_BUF_LEN]).
void UART_send_byte(void);						// Transmit on byte from transmitting buffer to UART.
void UART_receive_byte(void);					// Receive on byte from UART and put it into receiving buffer (buffer length in [UART_INPUT_BUF_LEN]).
int8_t UART_get_byte(void);						// Read on byte from receiving buffer.
uint16_t UART_get_received_number(void);		// Get number of unread bytes in receiving buffer.
uint16_t UART_get_sending_number(void);			// Get number of not transmitted bytes in transmitting buffer.
void UART_flush_in(void);						// Clear out receiving buffer.
void UART_dump_out(void);						// Dump all bytes one-by-one from transmitting buffer to UART (clear space in transmitting buffer).

#endif /* FGR_DRV_UART_HW_FOUND */

#endif /* FGR_DRV_UART */
