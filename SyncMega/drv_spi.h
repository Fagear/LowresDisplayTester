/**************************************************************************************************************************************************************
drv_spi.h

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

Created: 2016-09

Part of the [LowresDisplayTester] project.
SPI and SPI-through-UART driver for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A), ATmega48(-/A/P/AP), ATmega88(-/A/P/AP), ATmega168(-/A/P/AP), ATmega328(-/P).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_SPI_H_
#define FGR_DRV_SPI_H_

#include <avr/io.h>
#include "drv_CPU.h"	// Contains [F_CPU].

#undef FGR_DRV_SPI_Mxx
#undef FGR_DRV_SPI_Mxx8

#if SIGNATURE_1 == 0x92
	#if SIGNATURE_2 == 0x05			// ATmega48, ATmega48A
		#define  FGR_DRV_SPI_Mxx8
	#elif SIGNATURE_2 == 0x0A		// ATmega48P, ATmega48AP
		#define  FGR_DRV_SPI_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x93
	#if SIGNATURE_2 == 0x07			// ATmega8, ATmega8A
		#define FGR_DRV_SPI_Mxx
	#elif SIGNATURE_2 == 0x0A		// ATmega88, ATmega88A
		#define  FGR_DRV_SPI_Mxx8
	#elif SIGNATURE_2 == 0x0F		// ATmega88P, ATmega88PA
		#define  FGR_DRV_SPI_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x94
	#if SIGNATURE_2 == 0x03			// ATmega16, ATmega16A
		#define FGR_DRV_SPI_Mxx
	#elif SIGNATURE_2 == 0x06		// ATmega168, ATmega168A
		#define  FGR_DRV_SPI_Mxx8
	#elif SIGNATURE_2 == 0x0B		// ATmega168P, ATmega168PA
		#define  FGR_DRV_SPI_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x95
	#if SIGNATURE_2 == 0x02			// ATmega32, ATmega32A
		#define FGR_DRV_SPI_Mxx
	#elif SIGNATURE_2 == 0x14		// ATmega328
		#define  FGR_DRV_SPI_Mxx8
	#elif SIGNATURE_2 == 0x0F		// ATmega328P
		#define  FGR_DRV_SPI_Mxx8
	#endif /* SIGNATURE_2 */
#endif /* SIGNATURE_1 */

#ifdef FGR_DRV_SPI_Mxx
	// SPI hardware.
	#define FGR_DRV_SPI_HW_FOUND		// ATmega8(-/A), ATmega16(-/A), ATmega32(-/A)
	#define SPI_PORT			PORTB
	#define SPI_DIR				DDRB
	#define SPI_CLK				(1<<5)
	#define SPI_MOSI			(1<<3)
	#define SPI_MISO			(1<<4)
	#define SPI_CS				(1<<2)
	#define SPI_INT				SPI_STC_vect		// SPI Serial Transfer interrupt vector alias
	#define SPI_CONTROL			SPCR				// Control register
	#define SPI_STATUS			SPSR				// Status register
	#define SPI_DATA			SPDR				// Data register
#elif defined(FGR_DRV_SPI_Mxx8)
	// SPI hardware.
	#define FGR_DRV_SPI_HW_FOUND		// ATmega48(-/A/P/PA), ATmega88(-/A/P/PA), ATmega168(-/A/P/PA), ATmega328(-/P)
	#define SPI_PORT			PORTB
	#define SPI_DIR				DDRB
	#define SPI_CLK				(1<<5)
	#define SPI_MOSI			(1<<3)
	#define SPI_MISO			(1<<4)
	#define SPI_CS				(1<<2)
	#define SPI_INT				SPI_STC_vect		// SPI Serial Transfer interrupt vector alias
	#define SPI_CONTROL			SPCR				// Control register
	#define SPI_STATUS			SPSR				// Status register
	#define SPI_DATA			SPDR				// Data register
	// SPI through UART hardware.
	#define FGR_DRV_UARTSPI_HW_FOUND
	#define UART_SPI_PORT		PORTD
	#define UART_SPI_DIR		DDRD
	#define UART_SPI_CLK		(1<<4)
	#define UART_SPI_MOSI		(1<<1)
	#define UART_SPI_MISO		(1<<0)
	#define UART_SPI_INT		USART_TX_vect_num	// SPI through UART Serial Transfer interrupt vector alias
	#define UART_SPI_STATUS		UCSR0A				// Status register
	#define UART_SPI_CONTROL1	UCSR0B				// Control register 1
	#define UART_SPI_CONTROL2	UCSR0C				// Control register 2
	#define UART_SPI_DATA		UDR0				// Data register
	#define UART_SPI_DIVIDER	UBRR0				// Bitrate register
#endif /* FGR_DRV_SPI_Mxx */

#ifdef FGR_DRV_SPI_HW_FOUND

#define SPI_TX_START		SPI_PORT &= ~SPI_CS
#define SPI_TX_END			SPI_PORT |= SPI_CS
#define SPI_CONFIG_MSTR_M0	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)	// Configure SPI for Master operation Mode 0 (sample on rising, setup on falling)
#define SPI_CONFIG_MSTR_M1	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)	// Configure SPI for Master operation Mode 1 (setup on rising, sample on falling)
#define SPI_CONFIG_MSTR_M2	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)	// Configure SPI for Master operation Mode 2 (sample on falling, setup on rising)
#define SPI_CONFIG_MSTR_M3	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)	// Configure SPI for Master operation Mode 3 (setup on falling, sample on rising)
#define SPI_START			SPI_CONTROL |= (1<<SPE)			// Start SPI HW
#define SPI_STOP			SPI_CONTROL &= ~(1<<SPE)		// Stop SPI HW
#define SPI_INT_EN			SPI_CONTROL |= (1<<SPIE)		// Enable interrupt on transfer completion
#define SPI_INT_DIS			SPI_CONTROL &= ~(1<<SPIE)		// Disable interrupts

// Available SPI hardware clock dividers.
enum
{
	SPI_DIV_2 = 2,
	SPI_DIV_4 = 4,
	SPI_DIV_8 = 8,
	SPI_DIV_16 = 16,
	SPI_DIV_32 = 32,
	SPI_DIV_64 = 64,
	SPI_DIV_128 = 128,
};
#endif /* FGR_DRV_SPI_HW_FOUND */

#ifndef UDORD0
	// Workaround for broken headers for xxxPA MCUs: disable SPI through UART.
	#undef FGR_DRV_UARTSPI_HW_FOUND
#endif

#ifdef FGR_DRV_UARTSPI_HW_FOUND

#define UART_SPI_CONFIG_M0	UART_SPI_CONTROL2 = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UDORD0)|(0<<UCPHA0)|(0<<UCPOL0)	// Configure SPI for Master operation Mode 0 (sample on rising, setup on falling)
#define UART_SPI_CONFIG_M1	UART_SPI_CONTROL2 = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UDORD0)|(1<<UCPHA0)|(0<<UCPOL0)	// Configure SPI for Master operation Mode 1 (setup on rising, sample on falling)
#define UART_SPI_CONFIG_M2	UART_SPI_CONTROL2 = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UDORD0)|(0<<UCPHA0)|(1<<UCPOL0)	// Configure SPI for Master operation Mode 2 (sample on falling, setup on rising)
#define UART_SPI_CONFIG_M3	UART_SPI_CONTROL2 = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UDORD0)|(1<<UCPHA0)|(1<<UCPOL0)	// Configure SPI for Master operation Mode 3 (setup on falling, sample on rising)
#define UART_SPI_START		UART_SPI_CONTROL1 |= (1<<RXEN0)|(1<<TXEN0)						// Start SPI HW
#define UART_SPI_STOP		UART_SPI_CONTROL1 &= ~((1<<RXEN0)|(1<<TXEN0))					// Stop SPI HW
#define UART_SPI_INT_EN		UART_SPI_CONTROL1 |= (1<<TXCIE0)								// Enable interrupt on transfer completion
#define UART_SPI_INT_DIS	UART_SPI_CONTROL1 &= ~((1<<TXCIE0)|(1<<RXCIE0)|(1<<UDRIE0));	// Disable interrupts

#endif /* FGR_DRV_UARTSPI_HW_FOUND */

#ifdef FGR_DRV_SPI_HW_FOUND

inline void SPI_init_HW(void)
{
	SPI_CONTROL = 0;
	// Init pins.
	SPI_PORT &= ~(SPI_CLK|SPI_MOSI|SPI_CS);
	SPI_DIR |= SPI_CLK|SPI_MOSI|SPI_CS;
	SPI_DIR &= ~SPI_MISO;
	SPI_PORT |= SPI_CLK|SPI_MOSI|SPI_CS;
}

inline void SPI_set_target_clock(uint32_t target_speed)
{
	uint32_t calc_div;
	//calc_div = target_speed;
	calc_div = F_CPU/target_speed;
	// Round up divider to available dividers.
	if(calc_div<SPI_DIV_4)
	{
		calc_div = SPI_DIV_2;
	}
	else if(calc_div<SPI_DIV_8)
	{
		calc_div = SPI_DIV_4;
	}
	else if(calc_div<SPI_DIV_16)
	{
		calc_div = SPI_DIV_8;
	}
	else if(calc_div<SPI_DIV_32)
	{
		calc_div = SPI_DIV_16;
	}
	else if(calc_div<SPI_DIV_64)
	{
		calc_div = SPI_DIV_32;
	}
	else if(calc_div<SPI_DIV_128)
	{
		calc_div = SPI_DIV_64;
	}
	else
	{
		calc_div = SPI_DIV_128;
	}
	// Set highest possible speed.
	if(calc_div==SPI_DIV_2)
	{
		SPI_STATUS |= (1<<SPI2X);
		SPI_CONTROL &= ~((1<<SPR0)|(1<<SPR1));
	}
	else if(calc_div==SPI_DIV_4)
	{
		SPI_CONTROL &= ~((1<<SPR0)|(1<<SPR1));
		SPI_STATUS &= ~(1<<SPI2X);
	}
	else if(calc_div==SPI_DIV_8)
	{
		SPI_STATUS |= (1<<SPI2X);
		SPI_CONTROL |= (1<<SPR0);
		SPI_CONTROL &= ~(1<<SPR1);
	}
	else if(calc_div==SPI_DIV_16)
	{
		SPI_CONTROL |= (1<<SPR0);
		SPI_CONTROL &= ~(1<<SPR1);
		SPI_STATUS &= ~(1<<SPI2X);
	}
	else if(calc_div==SPI_DIV_32)
	{
		SPI_STATUS |= (1<<SPI2X);
		SPI_CONTROL |= (1<<SPR1);
		SPI_CONTROL &= ~(1<<SPR0);
	}
	else if(calc_div==SPI_DIV_64)
	{
		SPI_CONTROL |= (1<<SPR1);
		SPI_CONTROL &= ~(1<<SPR0);
		SPI_STATUS &= ~(1<<SPI2X);
	}
	else if(calc_div==SPI_DIV_128)
	{
		SPI_CONTROL |= (1<<SPR0)|(1<<SPR1);
		SPI_STATUS &= ~(1<<SPI2X);
	}
}

#endif /* FGR_DRV_SPI_HW_FOUND */

#ifdef FGR_DRV_UARTSPI_HW_FOUND

inline void UART_SPI_init_HW(void)
{
	// Init pins.
	UART_SPI_PORT &= ~(UART_SPI_CLK|UART_SPI_MOSI);
	UART_SPI_DIR |= UART_SPI_CLK|UART_SPI_MOSI;
	UART_SPI_DIR &= ~UART_SPI_MISO;
	UART_SPI_PORT |= (UART_SPI_CLK|UART_SPI_MOSI);
}

inline void UART_SPI_set_target_clock(uint32_t target_speed)
{
	uint32_t calc_div;
	calc_div = F_CPU/2/target_speed;
	// Check for lower limit.
	if(calc_div==0)
	{
		calc_div = 1;
	}
	calc_div -= 1;
	// Check for higher limit.
	if(calc_div>4095)
	{
		calc_div = 4095;
	}
	UART_SPI_DIVIDER = calc_div;
}

#endif /* FGR_DRV_UARTSPI_HW_FOUND */

#endif /* FGR_DRV_SPI_H_ */