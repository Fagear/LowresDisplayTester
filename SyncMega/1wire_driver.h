#ifndef BMCH_ONEWIRE_DRIVER
#define BMCH_ONEWIRE_DRIVER

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h> 

#define ONEWIRE_PORT		PORTB
#define ONEWIRE_PIN			PINB
#define ONEWIRE_DDR			DDRB
#define ONEWIRE_PINNUM		(1<<7)
#define ONEWIRE_GOOD		0
#define ONEWIRE_NOLOW		1
#define ONEWIRE_NOHIGH		2
#define ONEWIRE_BAD_COUNTER	3
#define ONEWIRE_T_READY		0
#define ONEWIRE_T_NOTREADY	1

unsigned char OneWire_init(void);
unsigned char OneWire_get_bit(const unsigned char, const unsigned char);
unsigned char OneWire_send_bit(const unsigned char, const unsigned char);
unsigned char OneWire_process_queue(unsigned char *);
void OneWire_t_result(unsigned char *, unsigned char *, unsigned char *);

#endif // BMCH_ONEWIRE_DRIVER
