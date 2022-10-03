#include "1wire_driver.h"

unsigned char uc_onewire_temp=0,
	uc_onewire_read_counter=0,
	uc_onewire_send_counter=0,
	uc_onewire_queue_counter=0,
	uc_onewire_read_byte=0,
	uc_onewire_status=ONEWIRE_GOOD,
	uc_onewire_t=0,
	uc_onewire_t_sign=0,
	uc_onewire_t_abs=0;

const unsigned char uca_onewire_queue[] PROGMEM ={0x01,0xCC,0x44,0x01,0xCC,0xBE,0x02,0x03,
	0x02,0x04,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x00};

//-------------------------------------- 1-wire initialization procedure.
unsigned char OneWire_init(void)
{
	unsigned char uc_ds_wait;
// Set 1-wire pin as output.
	ONEWIRE_DDR|=ONEWIRE_PINNUM;
// Send "reset" pulse to 1-wire bus.
	ONEWIRE_PORT&=~ONEWIRE_PINNUM;
// 480us delay.
	_delay_us(80);
	_delay_us(80);
	_delay_us(80);
	_delay_us(80);
	_delay_us(80);
	_delay_us(80);
// Set 1-wire pin as input.
	ONEWIRE_DDR&=~ONEWIRE_PINNUM;
// Wait for low level.
	_delay_us(50);
// Wait for maximum 190 us.
	for(uc_ds_wait=0;uc_ds_wait<20;uc_ds_wait++)
	{
		if((ONEWIRE_PIN&ONEWIRE_PINNUM)==0)
			break;
		_delay_us(10);
	}
	if(uc_ds_wait==20)
		return ONEWIRE_NOLOW;
// Got low on bus.
// Wait for maximum 400 us.
	for(uc_ds_wait=0;uc_ds_wait<41;uc_ds_wait++)
	{
		if((ONEWIRE_PIN&ONEWIRE_PINNUM)!=0)
			break;
		_delay_us(10);
	}
	if(uc_ds_wait==41)
		return ONEWIRE_NOHIGH;
// Send 1-wire state "good".
	return ONEWIRE_GOOD;
}

//-------------------------------------- Read byte from 1-wire bus.
unsigned char OneWire_get_bit(const unsigned char uc_readbyte, const unsigned char uc_bit_number)
{
// Check if error.
	if(uc_bit_number>7)
		return ONEWIRE_BAD_COUNTER;
	unsigned char uc_temp;
	uc_temp=uc_readbyte;
	asm("cli");
// Set 1-wire pin as output.
	ONEWIRE_DDR|=ONEWIRE_PINNUM;
// Pull 1-wire line low.
	ONEWIRE_PORT&=~ONEWIRE_PINNUM;
	_delay_us(2);
// Set 1-wire bus as input w/o pull-up.
	ONEWIRE_DDR&=~ONEWIRE_PINNUM;
// Wait.
	_delay_us(11);
// Read 1-wire bus.
	if((ONEWIRE_PIN&ONEWIRE_PINNUM)!=0)
	{
		uc_temp|=(1<<uc_bit_number);
	}
	else
	{
		uc_temp&=~(1<<uc_bit_number);
	}
	asm("sei");
// Wait for the end of timeslot.
	_delay_us(45);
	return uc_temp;
}

//-------------------------------------- Write byte to 1-wire bus.
unsigned char OneWire_send_bit(const unsigned char uc_sendbyte, const unsigned char uc_bit_number)
{
// Check if error.
	if(uc_bit_number>7)
		return ONEWIRE_BAD_COUNTER;
	unsigned char uc_temp;
	uc_temp=uc_sendbyte;
	uc_temp&=(1<<uc_bit_number);
	uc_temp=(uc_temp>>uc_bit_number);
	asm("cli");
// Set 1-wire pin as output.
	ONEWIRE_DDR|=ONEWIRE_PINNUM;
// Write 0 time slot.
	if(uc_temp==0)
	{
	// Pull 1-wire line low.
		ONEWIRE_PORT&=~ONEWIRE_PINNUM;
		_delay_us(65);
	}
// Write 1 time slot.
	else
	{
	// Pull 1-wire line low.
		ONEWIRE_PORT&=~ONEWIRE_PINNUM;
		_delay_us(2);
	// Pull 1-wire line high.
		ONEWIRE_PORT|=ONEWIRE_PINNUM;
		_delay_us(60);
	}
// Set 1-wire bus as input w/o pull-up.
	ONEWIRE_DDR&=~ONEWIRE_PINNUM;
	ONEWIRE_PORT&=~ONEWIRE_PINNUM;
	asm("sei");
// Recovery time.
	_delay_us(1);
	return ONEWIRE_GOOD;
};

//-------------------------------------- Process 1-wire command queue.
unsigned char OneWire_process_queue(unsigned char *uc_status)
{
	uc_onewire_temp=pgm_read_byte_near(uca_onewire_queue+uc_onewire_queue_counter);
	if(uc_onewire_temp==0x00)
	{
	// Restart queue.
		uc_onewire_queue_counter=0;
		return ONEWIRE_T_NOTREADY;
	}
	else if(uc_onewire_temp==0x01)
	{
	// Init 1-wire.
		uc_onewire_status=OneWire_init();
		*uc_status=uc_onewire_status;
		uc_onewire_queue_counter++;
		return ONEWIRE_T_NOTREADY;
	}
	else if(uc_onewire_temp==0x02)
	{
	// Read byte.
		uc_onewire_read_byte=OneWire_get_bit(uc_onewire_read_byte,uc_onewire_read_counter);
		uc_onewire_read_counter++;
		if(uc_onewire_read_counter>7)
		{
			uc_onewire_read_counter=0;
			uc_onewire_queue_counter++;
		}
		return ONEWIRE_T_NOTREADY;
	}
	else if(uc_onewire_temp==0x03)
	{
	// Store LS-byte from 1-wire.
		uc_onewire_t=uc_onewire_read_byte;
		uc_onewire_t=(uc_onewire_t>>1);
	// Check sign.
		if(uc_onewire_t_sign!=0)
		{
		// Invert.
			uc_onewire_t^=(0xFF);
			uc_onewire_t&=~(1<<7);
		}
		uc_onewire_t_abs=uc_onewire_read_byte;
		uc_onewire_t_abs=(uc_onewire_t_abs>>1);
	// Check sign.
		if(uc_onewire_t_sign==0)
		{
		// "+128".
			uc_onewire_t_abs|=(1<<7);
		}
		else
		{
		// Invert.
			uc_onewire_t_abs&=~(1<<7);
		}
		uc_onewire_queue_counter++;
		return ONEWIRE_T_READY;
	}
	else if(uc_onewire_temp==0x04)
	{
	// Store MS-byte from 1-wire.
		uc_onewire_t_sign=uc_onewire_read_byte;
		uc_onewire_queue_counter++;
		return ONEWIRE_T_NOTREADY;
	}
	else
	{
	// Send byte.
		OneWire_send_bit(uc_onewire_temp,uc_onewire_send_counter);
		uc_onewire_send_counter++;
		if(uc_onewire_send_counter>7)
		{
			uc_onewire_send_counter=0;
			uc_onewire_queue_counter++;
		}
		return ONEWIRE_T_NOTREADY;
	}
}

//-------------------------------------- Return measured temperature.
void OneWire_t_result(unsigned char *uc_val, unsigned char *uc_sign,
	unsigned char *uc_abs)
{
	*uc_val=uc_onewire_t;
	*uc_sign=uc_onewire_t_sign;
	*uc_abs=uc_onewire_t_abs;
}
