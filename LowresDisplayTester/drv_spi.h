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

#undef FGR_DRV_SPI_Mx
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
		#define FGR_DRV_SPI_Mx
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

#ifdef FGR_DRV_SPI_Mx
	// SPI hardware.
	#define FGR_DRV_SPI_HW_FOUND		// ATmega8(-/A)
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
#elif defined(FGR_DRV_SPI_Mxx)
	#define FGR_DRV_SPI_HW_FOUND		// ATmega16(-/A), ATmega32(-/A)
	#define SPI_PORT			PORTB
	#define SPI_DIR				DDRB
	#define SPI_CLK				(1<<7)
	#define SPI_MOSI			(1<<5)
	#define SPI_MISO			(1<<6)
	#define SPI_CS				(1<<4)
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
#endif /* FGR_DRV_SPI_Mx */

#ifdef FGR_DRV_SPI_HW_FOUND

#define SPI_TX_START		SPI_PORT &= ~SPI_CS
#define SPI_TX_END			SPI_PORT |= SPI_CS
#define SPI_CONFIG_MSTR_M0	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)	// Configure SPI for Master operation Mode 0 (sample on rising, setup on falling)
#define SPI_CONFIG_MSTR_M1	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(0<<CPOL)|(1<<CPHA)	// Configure SPI for Master operation Mode 1 (setup on rising, sample on falling)
#define SPI_CONFIG_MSTR_M2	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(1<<CPOL)|(0<<CPHA)	// Configure SPI for Master operation Mode 2 (sample on falling, setup on rising)
#define SPI_CONFIG_MSTR_M3	SPI_CONTROL = (0<<DORD)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)	// Configure SPI for Master operation Mode 3 (setup on falling, sample on rising)
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

#endif /* FGR_DRV_SPI_H_ */
