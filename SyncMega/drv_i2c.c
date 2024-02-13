#include "drv_CPU.h"					// Contains [F_CPU].
#include "drv_I2C.h"

#ifdef FGR_DRV_I2C_HW_FOUND

static uint8_t
	u8_i2c_error = I2C_ERR_NO,			// Last I2C error
	u8_i2c_mode = I2C_MODE_IDLE,		// Current mode of I2C transmittion
	u8_i2c_ping_timeout = 0,			// I2C communication timeout timer
	u8_i2c_error_counter = 0,			// Number of connection retries
	u8_i2c_total_write_bytes = 0,		// Number of bytes to be transmitted via I2C
	u8_i2c_total_read_bytes = 0,		// Number of bytes to be received via I2C
	u8_i2c_write_idx = 0,				// Number of current byte in [u8a_i2c_write_buffer]
	u8_i2c_read_idx = 0,				// Number of current byte in [u8a_i2c_read_buffer]
	u8_i2c_tasks = 0,					// Tasks for transmittions
	u8a_i2c_write_buffer[I2C_MAX_SEND_LEN],	// Buffer for I2C write-to-target transmittions
	u8a_i2c_read_buffer[I2C_MAX_SEND_LEN];	// Buffer for I2C read-from-target transmittions

extern void __builtin_avr_delay_cycles(unsigned long);

extern uint8_t HD44780_write_string(const uint8_t *);

//-------------------------------------- Set I2C speed with "I2C_SPD__xxxKHZ" defines.
void I2C_set_speed(uint16_t in_spd)
{
	I2C_BITRATE = (F_CPU/1000/in_spd-16)/2;
}

//-------------------------------------- Set I2C clock to 100 kHz.
inline void I2C_set_speed_100kHz(void)
{
	I2C_BITRATE = (F_CPU/1000/I2C_SPD_100KHZ-16)/2;
}

//-------------------------------------- Set I2C clock to 400 kHz.
inline void I2C_set_speed_400kHz(void)
{
	I2C_BITRATE = (F_CPU/1000/I2C_SPD_400KHZ-16)/2;
}

//-------------------------------------- Set address of I2C device to connect to.
void I2C_set_target_address(uint8_t addr)
{
	// Mask out non-address bits.
	addr &= I2C_ADDRESS_MASK;
	// Store address.
	u8a_i2c_write_buffer[I2C_BUF_ADDR] &= I2C_ADDRESS_MASK;
	u8a_i2c_write_buffer[I2C_BUF_ADDR] += addr;
	u8a_i2c_read_buffer[I2C_BUF_ADDR] &= I2C_ADDRESS_MASK;
	u8a_i2c_read_buffer[I2C_BUF_ADDR] += addr;
	u8a_i2c_read_buffer[I2C_BUF_ADDR] += I2C_READ_BIT;
	// Reset transmittion data.
	u8_i2c_total_read_bytes = 0;
	u8_i2c_total_write_bytes = 0;
	u8_i2c_read_idx = u8_i2c_write_idx = 0;
}

//-------------------------------------- Set data array to be send and number of bytes.
void I2C_set_data(uint8_t cnt, uint8_t *data)
{
	// Limit data count to set [I2C_MAX_SEND_LEN] define.
	if(cnt>=I2C_MAX_SEND_LEN)
	{
		cnt = (I2C_MAX_SEND_LEN-1);
	}
	// Copy data.
	for(uint8_t idx=0;idx<cnt;idx++)
	{
		u8_i2c_total_write_bytes++;
		u8a_i2c_write_buffer[u8_i2c_total_write_bytes] = data[idx];
	}
}

//-------------------------------------- Get length of the received data.
uint8_t I2C_get_len(void)
{
	return u8_i2c_total_read_bytes;
}

//-------------------------------------- Get received data.
void I2C_get_data(uint8_t *data)
{
	uint8_t cnt;
	cnt = I2C_get_len();
	for(uint8_t idx=0;idx<cnt;idx++)
	{
		data[idx] = u8a_i2c_read_buffer[(idx+1)];
	}
}

//-------------------------------------- Send data to the target address.
void I2C_write_data(uint8_t addr, uint8_t cnt, uint8_t *data)
{
	HD44780_write_string("NEW_DATA|");
	I2C_set_target_address(addr);
	I2C_set_data(cnt, data);
	u8_i2c_tasks |= (1<<I2C_MODE_WR);
}

#ifdef I2C_MASTER_EN
void I2C_master_processor(void)
{
	uint8_t tmp_data;
	// Get I2C hardware state.
	tmp_data = I2C_STATUS&I2C_STAT_MASK;
	HD44780_write_string("PRC-");
	HD44780_write_8bit_number(tmp_data, HD44780_NUMBER_HEX); HD44780_write_string("-");
	// Reset I2C timeout timer.
	u8_i2c_ping_timeout=0;
	// Universal states.
	if(u8_i2c_mode==I2C_MODE_IDLE)
	{
		// No transmittion taking place.
		HD44780_write_8bit_number(u8_i2c_write_idx, HD44780_NUMBER_HEX); HD44780_write_string("-");
		HD44780_write_8bit_number(u8_i2c_total_write_bytes, HD44780_NUMBER_HEX); HD44780_write_string("|");
		// Check if there is something to write to I2C target.
		if((u8_i2c_tasks&(1<<I2C_MODE_WR))!=0)
		{
			HD44780_write_string("RUN_WR|");
			// Initiate transmittion.
			I2C_DO_START;
			u8_i2c_mode = I2C_MODE_WR;
			// Reset error.
			u8_i2c_error = I2C_ERR_NO;
		}
		// Check if there is something to read from I2C target.
		/*else if(u8_i2c_total_read_bytes>(u8_i2c_read_idx+1))
		{
			HD44780_write_string("RUN_RD|");
			// Initiate transmittion.
			I2C_DO_START;
			u8_i2c_mode = I2C_MODE_RD;
		}*/
	}
	else if((tmp_data==I2C_STAT_BUS_ERR)||(tmp_data==I2C_STAT_NOARB))
	{
		HD44780_write_string("ARB_L|");
		// Bus error, lost arbitration.
		u8_i2c_error = I2C_ERR_NO;
		// Switch I2C off, terminate everything until next call.
		I2C_MASTER_NEXT_STEP;
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_tasks = 0;
		u8_i2c_total_read_bytes = u8_i2c_total_write_bytes = 0;
		u8_i2c_read_idx = u8_i2c_write_idx = 0;
		__builtin_avr_delay_cycles(1);
		I2C_SW_OFF;
		// Manage online/offline state of the counter.
		if(u8_i2c_error_counter<I2C_MAX_ERRORS) u8_i2c_error_counter++;
	}
	else if((tmp_data==I2C_STAT_W_ADR_N)||(tmp_data==I2C_STAT_W_DATA_N)||(tmp_data==I2C_STAT_R_ADR_N))
	{
		HD44780_write_string("TX NACK|");
		// NACK while sending.
		u8_i2c_error = I2C_ERR_M_NACK;
		// Switch I2C off, terminate everything until next poll.
		I2C_DO_STOP;
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_tasks = 0;
		u8_i2c_total_read_bytes = u8_i2c_total_write_bytes = 0;
		u8_i2c_read_idx = u8_i2c_write_idx = 0;
		if(tmp_data!=I2C_STAT_W_DATA_N)
		{
			// Manage online/offline state of the counter.
			if(u8_i2c_error_counter<I2C_MAX_ERRORS) u8_i2c_error_counter++;
		}
	}
	// Master Transmitter states.
	else if(tmp_data==I2C_STAT_START)
	{
		// START is sent.
		if(u8_i2c_mode==I2C_MODE_WR)
		{
			HD44780_write_string("A+W|");
			// Send A+W.
			I2C_DATA = u8a_i2c_write_buffer[I2C_BUF_ADDR];
			u8_i2c_write_idx++;
			HD44780_write_8bit_number(u8_i2c_write_idx, HD44780_NUMBER_HEX); HD44780_write_string("|");
		}
		else
		{
			HD44780_write_string("A+R|");
			// Send A+R.
			I2C_DATA = u8a_i2c_read_buffer[I2C_BUF_ADDR];
			u8_i2c_read_idx++;
			HD44780_write_8bit_number(u8_i2c_read_idx, HD44780_NUMBER_HEX); HD44780_write_string("|");
		}
		I2C_MASTER_NEXT_STEP;
	}
	else if(tmp_data==I2C_STAT_RESTART)
	{
		// RESTART is sent.
		if(u8_i2c_mode==I2C_MODE_WR)
		{
			HD44780_write_string("A+W|");
			// Send A+W.
			I2C_DATA = u8a_i2c_write_buffer[I2C_BUF_ADDR];
			u8_i2c_write_idx++;
		}
		else
		{
			HD44780_write_string("A+R|");
			// Send A+R.
			I2C_DATA = u8a_i2c_read_buffer[I2C_BUF_ADDR];
			u8_i2c_read_idx++;
		}
		I2C_MASTER_NEXT_STEP;
	}
	else if(tmp_data==I2C_STAT_W_ADR_A)
	{
		// A+W is sent, ACK received.
		if(u8_i2c_write_idx<u8_i2c_total_write_bytes)
		{
			HD44780_write_string("A+W,ACK,DATA|");
			// Not all data was sent.
			// Send next byte from the buffer.
			I2C_DATA = u8a_i2c_write_buffer[u8_i2c_write_idx];
			u8_i2c_write_idx++;
			I2C_MASTER_NEXT_STEP;
			HD44780_write_8bit_number(u8_i2c_write_idx, HD44780_NUMBER_HEX); HD44780_write_string("|");
		}
		else
		{
			HD44780_write_string("A+W,ACK,STOP|");
			// Everything is sent.
			I2C_DO_STOP;
		}
	}
	else if(tmp_data==I2C_STAT_W_DATA_A)
	{
		// Data byte is sent, ACK received.
		if(u8_i2c_write_idx<u8_i2c_total_write_bytes)
		{
			HD44780_write_string("W,ACK,DATA|");
			// Not all data was sent.
			// Send next byte from the buffer.
			I2C_DATA = u8a_i2c_write_buffer[u8_i2c_write_idx];
			u8_i2c_write_idx++;
			I2C_MASTER_NEXT_STEP;
			HD44780_write_8bit_number(u8_i2c_write_idx, HD44780_NUMBER_HEX); HD44780_write_string("|");
		}
		else
		{
			HD44780_write_string("W,ACK,STOP|");
			// Transmittion finished.
			I2C_DO_STOP;
		}
	}
	// Master Receiver states.
	else if(tmp_data==I2C_STAT_R_ADR_A)
	{
		// A+R is sent, ACK received.
		if(u8_i2c_total_write_bytes<=(u8_i2c_write_idx+1))
		{
			HD44780_write_string("A+R,ACK,ONE|");
			// Set NACK action for the next received byte.
			// (receive only one byte more)
			I2C_READ_ONE;
		}
		else
		{
			HD44780_write_string("A+R,ACK,NEXT|");
			// Set ACK action for the next received byte.
			// (receive not the last byte)
			I2C_READ_NEXT;
		}
	}
	else if(tmp_data==I2C_STAT_R_DATA_A)
	{
		// Data byte is received, ACK returned.
		// Put received byte into the buffer.
		u8a_i2c_read_buffer[(u8_i2c_read_idx-1)] = I2C_DATA;
		u8_i2c_read_idx++;
		if(u8_i2c_total_read_bytes<=(u8_i2c_read_idx+1))
		{
			HD44780_write_string("R,ACK,ONE|");
			// Next received byte will be NACKed and be the last one.
			I2C_READ_ONE;
		}
		else
		{
			HD44780_write_string("R,ACK,NEXT|");
			// Next received byte will be ACKed.
			I2C_READ_NEXT;
		}
		HD44780_write_8bit_number(u8_i2c_read_idx, HD44780_NUMBER_HEX); HD44780_write_string("|");
	}
	else if(tmp_data==I2C_STAT_R_DATA_N)
	{
		HD44780_write_string("R,NACK,STOP|");
		// Last data byte is received, NACK returned.
		tmp_data = I2C_DATA;
		// Put received byte into the buffer.
		if(u8_i2c_read_idx<I2C_MAX_SEND_LEN)
		{
			u8a_i2c_read_buffer[(u8_i2c_read_idx-1)] = tmp_data;
		}
		HD44780_write_8bit_number(u8_i2c_read_idx, HD44780_NUMBER_HEX); HD44780_write_string("|");
		// STOP.
		I2C_DO_STOP;
		// Reset.
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_tasks = 0;
	}
	// Re-enable interrupt.
	I2C_INT_CLR;
	I2C_INT_EN;
}
#endif /* I2C_MASTER_EN */

#ifdef I2C_SLAVE_EN
void I2C_set_slave_address(uint8_t new_addr)
{
	// Save 7-bit address.
	I2C_ADDRESS = (new_addr&0xFE);
}

inline void I2C_slave_processor(void)
{
	uint8_t tmp_data;
	tmp_data=I2C_STATUS&I2C_STAT_MASK;
	if(tmp_data==I2C_STAT_BUS_ERR)
	{
		// Bus error detected, need to recover.
		I2C_RECOVER_ERROR;
		I2C_SW_OFF;
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_tasks = 0;
		u8_i2c_total_write_bytes = 0;
		u8_i2c_write_idx = 0;
		I2C_INIT_SLAVE;
	}
	else if((tmp_data==I2C_STAT_RESTART)||(tmp_data==I2C_STAT_STOP))
	{
		// Received STOP or RESTART.
		if(u8_i2c_mode==I2C_MODE_RECEIVE)
		{
			// Was receiving data, need to do something with it.
			I2C_dispatcher();
		}
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_tasks = 0;
		// Switch to not addressed Slave.
		I2C_SLAVE_NEXT_STEP;
	}
/*
	else if((tmp_data==I2C_STAT_GEN_CALL)||(tmp_data==I2C_STAT_GEN_NOARB))
	{
		// General call received, ACK returned.
		// Prepare for the next data byte, but ignore everything else.
		u8_i2c_mode=I2C_MODE_IDLE;
		I2C_READ_ONE;
	}
	else if(tmp_data==I2C_STAT_GEN_DATA_A)
	{
		// General call received, data byte received, ACK returned.
		// Read byte, prepare for the next data byte, but ignore everything else.
		u8_i2c_mode=I2C_MODE_IDLE;
		tmp_data=I2C_GET_DATA;
		I2C_READ_ONE;
	}
	else if(tmp_data==I2C_STAT_GEN_DATA_N)
	{
		// General call received, data byte received, NOT ACK returned.
		// Read byte, switch to not addressed Slave.
		u8_i2c_mode=I2C_MODE_IDLE;
		tmp_data=I2C_GET_DATA;
		I2C_TRANSMITTION_END;
	}
*/
	else if((tmp_data==I2C_STAT_WR)||(tmp_data==I2C_STAT_WR_NOARB))
	{
		// Own A+W received, ACK returned.
		// Prepare for the next data byte.
		u8_i2c_mode=I2C_MODE_IDLE;
		// Reset I2C ping timeout.
		u8_i2c_ping_timeout=I2C_PING_TIMEOUT;
		// Set "I2C link detected".
		u8_flags|=STATE_I2C_OK;
		I2C_READ_NEXT;
	}
	else if(tmp_data==I2C_STAT_WR_DATA_A)
	{
		// Addressed with A+W, received a next byte.
		// Read a byte and determine next step.
		tmp_data=I2C_GET_DATA;
		if(u8_i2c_mode==I2C_MODE_IDLE)
		{
			// I2C was in waiting state, decode received command.
			if(tmp_data==I2C_CMD_SET_DIAM)
			{
				// Set roller diameter.
				u8_i2c_total_write_bytes=2;
				u8_i2c_write_idx=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_SET_POS)
			{
				// Set tape speed.
				u8_i2c_total_write_bytes=4;
				u8_i2c_write_idx=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_SET_SPEED)
			{
				// Set tape speed.
				u8_i2c_total_write_bytes=2;
				u8_i2c_write_idx=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_SET_FLAGS)
			{
				// Set misc. flags for the counter.
				u8_i2c_total_write_bytes=1;
				u8_i2c_write_idx=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_CLEAR)
			{
				// Clear tape counter.
				u8_i2c_total_write_bytes=0;
				u8_i2c_write_idx=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				clear_counter();
				// Prepare for the one data byte.
				I2C_READ_ONE;
			}
			else if(tmp_data==I2C_CMD_DISP_STR)
			{
				// Set symbols to display.
				u8_i2c_total_write_bytes=4;
				u8_i2c_write_idx=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if((tmp_data==I2C_CMD_GET_DIAM)||(tmp_data==I2C_CMD_GET_POS)||(tmp_data==I2C_CMD_GET_SPEED)||(tmp_data==I2C_CMD_GET_MOTION))
			{
				// Prepare to send data to Master.
				u8_i2c_last_command=tmp_data;
				I2C_READ_ONE;
			}
			else
			{
				// Unknown command.
				I2C_RECOVER_ERROR;
				I2C_SW_OFF;
				u8_i2c_mode = I2C_MODE_IDLE;
				u8_i2c_tasks = 0;
				u8_i2c_total_write_bytes = 0;
				u8_i2c_write_idx = 0;
				u8_i2c_last_command=I2C_CMD_UNKNOWN;
				I2C_INIT_SLAVE;
			}
		}
		else if(u8_i2c_mode==I2C_MODE_RECEIVE)
		{
			// Put received byte into the buffer.
			if(u8_i2c_write_idx<u8_i2c_total_write_bytes)
			{
				u8a_i2c_write_buffer[u8_i2c_write_idx]=tmp_data;
				u8_i2c_write_idx++;
				if(u8_i2c_write_idx==u8_i2c_total_write_bytes)
				{
					I2C_READ_ONE;
				}
				else
				{
					I2C_READ_NEXT;
				}
			}
			else
			{
				// Received all required bytes, do something.
				if(u8_i2c_write_idx==u8_i2c_total_write_bytes)
				{
					u8_i2c_write_idx++;
					I2C_dispatcher();
				}
				I2C_READ_ONE;
			}
		}
		else
		{
			// Abnormal state.
			I2C_RECOVER_ERROR;
			I2C_SW_OFF;
			u8_i2c_mode=I2C_MODE_IDLE;
			u8_i2c_total_write_bytes=0;
			u8_i2c_write_idx=0;
			u8_i2c_last_command=I2C_CMD_UNKNOWN;
			I2C_INIT_SLAVE;
		}
	}
	else if(tmp_data==I2C_STAT_WR_DATA_N)
	{
		// Addressed with A+W, received a last byte.
		// Read a byte, switch to not addressed Slave.
		tmp_data=I2C_GET_DATA;
		if(u8_i2c_mode==I2C_MODE_RECEIVE)
		{
			if(u8_i2c_write_idx<=u8_i2c_total_write_bytes)
			{
				u8a_i2c_write_buffer[u8_i2c_write_idx]=tmp_data;
				u8_i2c_write_idx++;
				// Last byte received, do something.
				I2C_dispatcher();
			}
		}
		u8_i2c_mode=I2C_MODE_IDLE;
		u8_i2c_total_write_bytes=0;
		u8_i2c_write_idx=0;
		u8_i2c_last_command=I2C_CMD_UNKNOWN;
		I2C_TRANSMITTION_END;
	}
	else if((tmp_data==I2C_STAT_RD)||(tmp_data==I2C_STAT_RD_NOARB))
	{
		// Own A+R received, ACK returned.
		// Determine next step.
		if(u8_i2c_last_command==I2C_CMD_GET_DIAM)
		{
			uint16_t u16_tmp;
			// Send current roller diameter.
			u8_i2c_total_write_bytes=2;
			u8_i2c_write_idx=0;
			// Calculate diameter.
			u16_tmp=(uint16_t)set_data.u32i_roller_length/PI_CONST;
			u8a_i2c_write_buffer[0]=(u16_tmp&0xFF);
			u8a_i2c_write_buffer[1]=((u16_tmp>>8)&0xFF);
			// Output first byte.
			I2C_set_data(u8a_i2c_write_buffer[0]);
			u8_i2c_write_idx++;
			u8_i2c_mode=I2C_MODE_TRANSMIT;
			I2C_WRITE_NEXT;
		}
		else if(u8_i2c_last_command==I2C_CMD_GET_POS)
		{
			int32_t tmp32;
			cli();
			tmp32=s32i_roller_seg_cnt;
			sei();
			// Send current segment count.
			u8_i2c_total_write_bytes=4;
			u8_i2c_write_idx=0;
			// Put data into buffer.
			u8a_i2c_write_buffer[0]=(tmp32&0xFF);
			u8a_i2c_write_buffer[1]=((tmp32>>8)&0xFF);
			u8a_i2c_write_buffer[2]=((tmp32>>16)&0xFF);
			u8a_i2c_write_buffer[3]=((tmp32>>24)&0xFF);
			// Output first byte.
			I2C_set_data(u8a_i2c_write_buffer[0]);
			u8_i2c_write_idx++;
			u8_i2c_mode=I2C_MODE_TRANSMIT;
			I2C_WRITE_NEXT;
		}
		else if(u8_i2c_last_command==I2C_CMD_GET_SPEED)
		{
			//uint32_t tmp32;
			/*cli();
			tmp32=u32_tape_speed;
			sei();*/
			// Send current tape speed.
			u8_i2c_total_write_bytes=4;
			u8_i2c_write_idx=0;
			// Put data into buffer.
			u8a_i2c_write_buffer[0]=(u32_tape_speed&0xFF);
			u8a_i2c_write_buffer[1]=((u32_tape_speed>>8)&0xFF);
			u8a_i2c_write_buffer[2]=((u32_tape_speed>>16)&0xFF);
			u8a_i2c_write_buffer[3]=((u32_tape_speed>>24)&0xFF);
			// Output first byte.
			I2C_set_data(u8a_i2c_write_buffer[0]);
			u8_i2c_write_idx++;
			u8_i2c_mode=I2C_MODE_TRANSMIT;
			I2C_WRITE_NEXT;
		}
		else if(u8_i2c_last_command==I2C_CMD_GET_MOTION)
		{
			uint8_t u8_i2c_flags=0, tmp8;
			// Send misc. flags: direction of motion, cal warning, ...
			u8_i2c_total_write_bytes=1;
			u8_i2c_write_idx=1;
			u8_i2c_mode=I2C_MODE_TRANSMIT;
			// Assemble bits.
			cli();
			tmp8=u8i_tape_state;
			sei();
			if(tmp8==TAPE_IDLE)
			{
				u8_i2c_flags&=~I2C_FLAG_MOTION;
			}
			else if(tmp8==TAPE_FWD)
			{
				u8_i2c_flags|=I2C_FLAG_MOTION;
				u8_i2c_flags&=~I2C_FLAG_REVERSE;
			}
			else if(tmp8==TAPE_REV)
			{
				u8_i2c_flags|=I2C_FLAG_MOTION|I2C_FLAG_REVERSE;
			}
			if(u8_cal_sync_state==CAL_NOT_SYNCED)
			{
				u8_i2c_flags|=I2C_FLAG_NEW_CAL;
			}
			if(u8_eep_state==EEP_HW_BAD)
			{
				u8_i2c_flags|=I2C_FLAG_BAD_EEP;
			}
			// Output data.
			I2C_set_data(u8_i2c_flags);
			I2C_WRITE_ONE;
		}
		else
		{
			// Output dummy byte.
			I2C_set_data(0x5A);
			I2C_WRITE_ONE;
		}
	}
	else if((tmp_data==I2C_STAT_RD_DATA_N)||(tmp_data==I2C_STAT_RD_LAST_A))
	{
		// Addressed with A+R, last byte transmitted.
		// Switch to not addressed Slave.
		u8_i2c_mode=I2C_MODE_IDLE;
		u8_i2c_total_write_bytes=0;
		u8_i2c_write_idx=0;
		u8_i2c_last_command=I2C_CMD_UNKNOWN;
		I2C_TRANSMITTION_END;
	}
	else if(tmp_data==I2C_STAT_RD_DATA_A)
	{
		// Addressed with A+R, byte was transmitted, ACK received.
		// Determine next step.
		if(u8_i2c_mode==I2C_MODE_TRANSMIT)
		{
			if(u8_i2c_write_idx<u8_i2c_total_write_bytes)
			{
				I2C_set_data(u8a_i2c_write_buffer[u8_i2c_write_idx]);
				u8_i2c_write_idx++;
				if(u8_i2c_write_idx==u8_i2c_total_write_bytes)
				{
					I2C_WRITE_ONE;
				}
				else
				{
					I2C_WRITE_NEXT;
				}
			}
			else
			{
				// Output dummy byte.
				I2C_set_data(0x5A);
				I2C_WRITE_ONE;
			}
		}
	}
	else
	{	
		// Abnormal state.
		I2C_RECOVER_ERROR;
		I2C_SW_OFF;
		u8_i2c_mode=I2C_MODE_IDLE;
		u8_i2c_total_write_bytes=0;
		u8_i2c_write_idx=0;
		u8_i2c_last_command=I2C_CMD_UNKNOWN;
		I2C_INIT_SLAVE;
	}
}
#endif /* I2C_SLAVE_EN */

#endif /* FGR_DRV_I2C_HW_FOUND */