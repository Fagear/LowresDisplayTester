/**************************************************************************************************************************************************************
drv_io.h

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
I/O and peripheral hardware driver for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.
This driver provides startup initialization of the hardware.

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A), ATmega48(-/A/P/AP), ATmega88(-/A/P/AP), ATmega168(-/A/P/AP), ATmega328(-/P).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_IO_H_
#define FGR_DRV_IO_H_

//#undef FGR_DRV_HD44780P_FOUND
//#undef FGR_DRV_HD44780S_FOUND
//#undef FGR_DRV_UART_HW_FOUND
//#undef FGR_DRV_SPI_HW_FOUND
//#undef FGR_DRV_UARTSPI_HW_FOUND
//#undef FGR_DRV_I2C_HW_FOUND

#include <avr/io.h>

#include "config.h"
#include "drv_spi.h"				// Used for bar pattern generation
#ifdef CONF_EN_I2C
	#include "drv_i2c.h"
#endif /* CONF_EN_I2C */
#ifdef CONF_EN_UART
	#include "drv_uart.h"
#endif /* CONF_EN_UART */

// Include HAL for specific MCU.
#if SIGNATURE_1 == 0x92
	#if SIGNATURE_2 == 0x05			// ATmega48, ATmega48A
		#include "drv_io_mxx8.h"
	#elif SIGNATURE_2 == 0x0A		// ATmega48P, ATmega48AP
		#include "drv_io_mxx8.h"
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x93
	#if SIGNATURE_2 == 0x07			// ATmega8, ATmega8A
		#include "drv_io_mxx.h"
	#elif SIGNATURE_2 == 0x0A		// ATmega88, ATmega88A
		#include "drv_io_mxx8.h"
	#elif SIGNATURE_2 == 0x0F		// ATmega88P, ATmega88PA
		#include "drv_io_mxx8.h"
	#else
		#error Not supported MCU (by SIGNATURE_2)!
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x94
	#if SIGNATURE_2 == 0x03			// ATmega16, ATmega16A
		#include "drv_io_mxx.h"
	#elif SIGNATURE_2 == 0x06		// ATmega168, ATmega168A
		#include "drv_io_mxx8.h"
	#elif SIGNATURE_2 == 0x0B		// ATmega168P, ATmega168PA
		#include "drv_io_mxx8.h"
	#else
		#error Not supported MCU (by SIGNATURE_2)!
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x95
	#if SIGNATURE_2 == 0x02			// ATmega32, ATmega32A
		#include "drv_io_mxx.h"
	#elif SIGNATURE_2 == 0x14		// ATmega328
		#include "drv_io_mxx8.h"
	#elif SIGNATURE_2 == 0x0F		// ATmega328P
		#include "drv_io_mxx8.h"
	#else
		#error Not supported MCU (by SIGNATURE_2)!
	#endif /* SIGNATURE_2 */
#else
	#error Not supported MCU (by SIGNATURE_1)!
#endif /* SIGNATURE_1 */

#ifdef CONF_EN_HD44780P
	#include "drv_hd44780[4bit].h"
#endif	/* CONF_EN_HD44780P */

#ifdef CONF_EN_HD44780S
	#include "drv_hd44780[serial].h"
#endif /* CONF_EN_I2C */

/*#ifdef CONF_EN_UART
	#ifndef FGR_DRV_UART_HW_FOUND
		#warning UART driver not found!
	#endif
#endif

#ifdef FGR_DRV_SPI_HW_FOUND
	#ifndef FGR_DRV_UARTSPI_HW_FOUND
		#warning SPI-through-UART driver not found!
	#endif
#endif

#ifdef CONF_EN_I2C
	#ifndef FGR_DRV_I2C_HW_FOUND
		#warning I2C driver not found!
	#endif
#endif*/

//-------------------------------------- IO initialization.
inline void HW_init(void)
{
	// Init SPI interface.
#ifdef FGR_DRV_SPI_HW_FOUND
	SPI_init_HW();
#endif /* FGR_DRV_SPI_HW_FOUND */
#ifdef FGR_DRV_UART_HW_FOUND
	UART_init_HW();
#endif /* FGR_DRV_UART_HW_FOUND */
#ifdef FGR_DRV_UARTSPI_HW_FOUND
	UART_SPI_init_HW();
#endif /* FGR_DRV_UARTSPI_HW_FOUND */
#ifdef FGR_DRV_I2C_HW_FOUND
	I2C_init_HW();
#endif /* FGR_DRV_I2C_HW_FOUND */
	
	// Enable debug pins.
	DBG_SETUP1; DBG_SETUP2;
	
	// Set inputs.
	BTN_SETUP1; BTN_SETUP2; BTN_SETUP3; BTN_SETUP4;
	
	// Set outputs.
	VSYNC_SETUP1; VSYNC_SETUP2;
	// Timer1 output compare pin is used to generate sync signal for video.
	// Timer is re-tuned every horizontal line to produce various impulses.
	SYNC_CONFIG_PIN1; SYNC_CONFIG_PIN2;
	
	// Configure output for video sync.
	SYNC_CONFIG_NEG; SYNC_CONFIG_RUN;
	SYNC_STEP_DUR = 0xFFFF;
	SYNC_PULSE_DUR = SYNC_STEP_DUR/2;
	
	// Configure active line timer.
	LACT_CONFIG_REG1; LACT_CONFIG_REG2;
	//LACT_PULSE_DUR = 0xFF;
	//LACT_START;
	
	// Enable draw starting interrupts.
	// INT0 tied to Timer1 compare output, used to re-configure PWM for H-sync.
	SYNC_IN_CFG_PIN1; SYNC_IN_CFG_PIN2;
	SYNC_INT_CFG_REG;

	// Turn off not used devices for power saving.
	PWR_COMP_OFF;
#ifdef PWR_IO_MODULES
	PWR_ADC_OFF; PWR_T0_OFF;
#endif	/* PWR_IO_MODULES */
}

#endif /* FGR_DRV_IO_H_ */
