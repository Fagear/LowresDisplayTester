/**************************************************************************************************************************************************************
drv_i2c.h

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
I2C driver for AVR MCUs and AVRStudio/WinAVR/AtmelStudio compilers.

This driver is interrupt-based and targeted for very light interrupt routines (set the event flag)
and delayed event processing in the main loop.
The driver is targeted for real-time systems with no wait loops inside it.
The driver can set I2C speed on-the-fly, using [I2C_SPD_xxxKHZ] defines
and [F_CPU] define for CPU clock (in Hz).

Supported MCUs:	ATmega8(-/A), ATmega16(-/A), ATmega32(-/A), ATmega48(-/A/P/AP), ATmega88(-/A/P/AP), ATmega168(-/A/P/AP), ATmega328(-/P).

**************************************************************************************************************************************************************/

#ifndef FGR_DRV_I2C
#define FGR_DRV_I2C

#include <avr/io.h>
#include "config.h"
#include "drv_hd44780[4bit].h"

#undef FGR_DRV_I2C_Mxx
#undef FGR_DRV_I2C_Mxx8

#if SIGNATURE_1 == 0x92
	#if SIGNATURE_2 == 0x05			// ATmega48, ATmega48A
		#define  FGR_DRV_I2C_Mxx8
	#elif SIGNATURE_2 == 0x0A		// ATmega48P, ATmega48AP
		#define  FGR_DRV_I2C_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x93
	#if SIGNATURE_2 == 0x07			// ATmega8, ATmega8A
		#define FGR_DRV_I2C_Mxx8
	#elif SIGNATURE_2 == 0x0A		// ATmega88, ATmega88A
		#define  FGR_DRV_I2C_Mxx8
	#elif SIGNATURE_2 == 0x0F		// ATmega88P, ATmega88PA
		#define  FGR_DRV_I2C_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x94
	#if SIGNATURE_2 == 0x03			// ATmega16, ATmega16A
		#define FGR_DRV_I2C_Mxx
	#elif SIGNATURE_2 == 0x06		// ATmega168, ATmega168A
		#define  FGR_DRV_I2C_Mxx8
	#elif SIGNATURE_2 == 0x0B		// ATmega168P, ATmega168PA
		#define  FGR_DRV_I2C_Mxx8
	#endif /* SIGNATURE_2 */
#elif SIGNATURE_1 == 0x95
	#if SIGNATURE_2 == 0x02			// ATmega32, ATmega32A
		#define FGR_DRV_I2C_Mxx
	#elif SIGNATURE_2 == 0x14		// ATmega328
		#define  FGR_DRV_I2C_Mxx8
	#elif SIGNATURE_2 == 0x0F		// ATmega328P
		#define  FGR_DRV_I2C_Mxx8
	#endif /* SIGNATURE_2 */
#endif /* SIGNATURE_1 */

// I2C hardware.
#ifdef FGR_DRV_I2C_Mxx
	#define FGR_DRV_I2C_HW_FOUND		// ATmega16(-/A), ATmega32(-/A)
	#define I2C_PORT			PORTC
	#define I2C_DIR				DDRC
	#define I2C_SRC				PINC
	#define I2C_SCL				(1<<0)
	#define I2C_SDA				(1<<1)
	#define I2C_TEST			I2C_SDA
	#define I2C_INT				TWI_vect	// 2-wire Serial Interface Interrupt Handler
	#define I2C_BITRATE			TWBR		// Bitrate register
	#define I2C_STATUS			TWSR		// Status register
	#define I2C_CONTROL			TWCR		// Control register
	#define I2C_DATA			TWDR		// Data register
	#define I2C_ADDRESS			TWAR		// Address register
#elif defined(FGR_DRV_I2C_Mxx8)
	#define FGR_DRV_I2C_HW_FOUND		// ATmega8(-/A), ATmega48(-/A/P/PA), ATmega88(-/A/P/PA), ATmega168(-/A/P/PA), ATmega328(-/P)
	#define I2C_PORT			PORTC
	#define I2C_DIR				DDRC
	#define I2C_SRC				PINC
	#define I2C_SCL				(1<<5)
	#define I2C_SDA				(1<<4)
	#define I2C_TEST			I2C_SDA
	#define I2C_INT				TWI_vect	// 2-wire Serial Interface Interrupt Handler
	#define I2C_BITRATE			TWBR		// Bitrate register
	#define I2C_STATUS			TWSR		// Status register
	#define I2C_CONTROL			TWCR		// Control register
	#define I2C_DATA			TWDR		// Data register
	#define I2C_ADDRESS			TWAR		// Address register
#endif /* FGR_DRV_I2C_Mxx8 */

#ifndef CONF_EN_I2C
	#undef FGR_DRV_I2C_HW_FOUND
#endif /* CONF_EN_I2C */

#ifdef FGR_DRV_I2C_HW_FOUND

enum
{
	I2C_STAT_MASK = 0xF8,		// Mask for status bits
	// Misc. state codes.
	I2C_STAT_BUS_ERR = 0x00,	// I2C bus error
	I2C_STAT_BUSY = 0xF8,		// No relevant info, wait
	// Master state codes.
	I2C_STAT_START = 0x08,		// START has been transmitted
	I2C_STAT_RESTART = 0x10,	// RESTART has been transmitted
	I2C_STAT_W_ADR_A = 0x18,	// SLA+W transmitted, ACK received
	I2C_STAT_W_ADR_N = 0x20,	// SLA+W transmitted, NACK received
	I2C_STAT_W_DATA_A = 0x28,	// Byte transmitted, ACK received
	I2C_STAT_W_DATA_N = 0x30,	// Byte transmitted, NACK received
	I2C_STAT_NOARB = 0x38,		// Arbitration lost while transmitting/receiving
	I2C_STAT_R_ADR_A = 0x40,	// SLA+R transmitted, ACK received
	I2C_STAT_R_ADR_N = 0x48,	// SLA+R transmitted, NACK received
	I2C_STAT_R_DATA_A = 0x50,	// Byte received, ACK transmitted
	I2C_STAT_R_DATA_N = 0x58,	// Byte received, NACK transmitted
	// Slave state codes.
	I2C_STAT_WR = 0x60,			// Got SLA+W from Master
	I2C_STAT_WR_NOARB = 0x68,	// Got SLA+W from Master, own Master arbitration lost
	I2C_STAT_GEN_CALL = 0x70,	// General call received
	I2C_STAT_GEN_NOARB = 0x78,	// General call received, own Master arbitration lost
	I2C_STAT_WR_DATA_A = 0x80,	// Previously addressed with SLA+W, data received, ACK returned
	I2C_STAT_WR_DATA_N = 0x88,	// Previously addressed with SLA+W, data received, NACK returned
	I2C_STAT_GEN_DATA_A = 0x90,	// Previously addressed with general call, data received, ACK returned
	I2C_STAT_GEN_DATA_N = 0x98,	// Previously addressed with general call, data received, NACK returned
	I2C_STAT_STOP = 0xA0,		// STOP or RESTART received, while addressed as Slave
	I2C_STAT_RD = 0xA8,			// Got SLA+R from Master
	I2C_STAT_RD_NOARB = 0xB0,	// Got SLA+R from Master, own Master arbitration lost
	I2C_STAT_RD_DATA_A = 0xB8,	// Data byte transmitted, ACK received
	I2C_STAT_RD_DATA_N = 0xC0,	// Data byte transmitted, NACK received
	I2C_STAT_RD_LAST_A = 0xC8,	// Last data byte transmitted, ACK received
};

enum
{
	I2C_SPD_100KHZ = 100,		// Select 100 kHz clock speed for I2C bus
	I2C_SPD_400KHZ = 400,		// Select 400 kHz clock speed for I2C bus
};

#define I2C_ADDRESS_MASK		0xFE													// Mask for 7-bit I2C address
#define I2C_READ_BIT			(1<<0)													// Read flag in I2C address byte

#define I2C_INIT_MASTER			I2C_CONTROL=(1<<TWEN)|(1<<TWIE)							// Initaliaze master operation
#define I2C_INIT_SLAVE			I2C_CONTROL=(1<<TWEN)|(1<<TWEA)|(1<<TWIE)				// Initaliaze slave operation
#define I2C_INT_EN				I2C_CONTROL|=(1<<TWIE)									// Enable I2C interrupts
#define I2C_INT_DIS				I2C_CONTROL&=~(1<<TWIE)									// Disable I2C interrupts (for use inside I2C interrupt)
#define I2C_INT_CLR				I2C_CONTROL|=(1<<TWINT)									// Clear processed I2C interrupt
#define I2C_SW_OFF				I2C_CONTROL=(1<<TWINT)									// Switch I2C hardware off, release GPIO
#define I2C_DO_START			I2C_CONTROL=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE)	// MASTER: put START condition onto the bus
#define I2C_DO_RESTART			I2C_DO_START											// MASTER: put RESTART condition onto the bus
#define I2C_DO_STOP				I2C_CONTROL=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE)	// MASTER: put STOP condition onto the bus
#define I2C_READ_NEXT			I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE)	// Prepare to read next (not last) byte from I2C
#define I2C_READ_ONE			I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)				// Prepare to read last byte from I2C
#define I2C_WRITE_NEXT			I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE)	// SLAVE: prepare to write next (not last) byte to I2C
#define I2C_WRITE_ONE			I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)				// SLAVE: prepare to write last byte to I2C
#define I2C_MASTER_NEXT_STEP	I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)				// MASTER: allow I2C hardware state machine to do next step
#define I2C_SLAVE_NEXT_STEP		I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE)	// SLAVE: allow I2C hardware state machine to do next step
#define I2C_TRANSMITTION_END	I2C_CONTROL=(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE)	// SLAVE: end transmittion, switch for waiting
#define I2C_RECOVER_ERROR		I2C_CONTROL=(1<<TWINT)|(1<<TWSTO)						// Try to recover from an error

// I2C transmittion states.
enum
{
	I2C_MODE_IDLE,		// I2C waiting
	I2C_MODE_WR,		// I2C writing to a target
	I2C_MODE_RD,		// I2C reading from a target
};

// I2C error codes.
enum
{
	I2C_ERR_NO,			// No error
	I2C_ERR_LOST_ARB,	// I2C bus error or lost arbitration
	I2C_ERR_M_NACK,		// I2C-M got NACK while sending
};

// I2C driver configuration.
//#define I2C_ENABLE_UART_LOG			1			// Enable verbose logging
#define I2C_BUF_ADDR			0			// Index to store address in internal transmittion buffer
#define I2C_MAX_SEND_LEN		15			// Maximum length (in bytes) of single data transmittion
#define I2C_MAX_ERRORS			5			// Number of I2C transmittion failures before setting "device offline" flag
#define I2C_MASTER_EN			1			// Enable I2C-Master functions
//#define I2C_SLAVE_EN			1			// Enable I2C-Slave functions

void I2C_set_speed(uint16_t);				// Set I2C speed with "I2C_SPD__xxxKHZ" defines
void I2C_set_speed_100kHz(void);			// Set I2C clock to 100 kHz
void I2C_set_speed_400kHz(void);			// Set I2C clock to 400 kHz
void I2C_set_target_address(uint8_t addr);	// Set address of I2C device to connect to
void I2C_set_data(uint8_t cnt, uint8_t *data);	// Set data array to be send and number of bytes
uint8_t I2C_get_len(void);						// Get length of the received data
void I2C_get_data(uint8_t *data);				// Get received data
void I2C_write_data(uint8_t addr, uint8_t cnt, uint8_t *data);
void I2C_read_data(uint8_t addr, uint8_t cnt);

#ifdef I2C_MASTER_EN
	void I2C_master_processor(void);
#endif
#ifdef I2C_SLAVE_EN
	void I2C_set_slave_address(uint8_t);	// Set I2C slave address
	void I2C_slave_processor(void);
#endif

inline void I2C_init_HW(void)
{
	// Init pins.
	I2C_PORT |= (I2C_SCL|I2C_SDA);
	I2C_DIR |= (I2C_SCL|I2C_SDA);
}
#endif /* FGR_DRV_I2C_HW_FOUND */

#endif /* FGR_DRV_I2C */
