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

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A), ATmega48(-/A/P/AP), ATmega88(-/A/P/AP), ATmega168(-/A/P/AP), ATmega328(-/P).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_IO_H_
#define FGR_DRV_IO_H_

#undef FGR_DRV_SPI_HW_FOUND
#undef FGR_DRV_UARTSPI_HW_FOUND
#undef FGR_DRV_I2C_HW_FOUND

#include <avr/io.h>
#include "config.h"
#include "drv_i2c.h"
#include "drv_spi.h"

#ifdef CONF_EN_HD44780
	
#endif /* CONF_EN_HD44780 */

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

#ifndef FGR_DRV_SPI_HW_FOUND
	#ifndef FGR_DRV_UARTSPI_HW_FOUND
		#warning SPI driver not found!
	#endif
#endif

#ifndef FGR_DRV_I2C_HW_FOUND
	#warning I2C driver not found!
#endif

#endif /* FGR_DRV_IO_H_ */