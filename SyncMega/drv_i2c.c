#include "drv_I2C.h"

#ifdef FGR_DRV_I2C_HW_FOUND

static uint8_t
	u8_i2c_error = I2C_ERR_NO,
	u8_i2c_mode = I2C_MODE_IDLE,		// Current mode of I2C transmittion
	u8_i2c_ping_timeout = 0,			// I2C communication timeout timer
	u8_i2c_error_counter = 0,			// Number of connection retries
	u8_i2c_total_bytes = 0,				// Number of bytes to be transmitted via I2C
	u8_i2c_current_byte = 0,			// Number of current byte in transmittion
	u8a_i2c_buffer[I2C_MAX_SEND_LEN];	// Buffer for I2C transmittions

extern void __builtin_avr_delay_cycles(unsigned long);

void I2C_set_speed(uint16_t in_spd)
{
	I2C_BITRATE = (F_CPU/1000/in_spd-16)/2;
}

inline void I2C_set_speed_100kHz()
{
	I2C_BITRATE = (F_CPU/1000/I2C_SPD_100KHZ-16)/2;
}

inline void I2C_set_speed_400kHz()
{
	I2C_BITRATE = (F_CPU/1000/I2C_SPD_400KHZ-16)/2;
}

void I2C_set_address(uint8_t new_addr)
{
	// Save 7-bit address.
	I2C_ADDRESS = (new_addr&0xFE);
}

void I2C_set_data(uint8_t new_data)
{
	I2C_DATA = new_data;
}

#ifdef I2C_MASTER_EN
void I2C_master_processor(void)
{
	uint8_t tmp_data;
	// Get I2C hardware state.
	tmp_data = I2C_STATUS&0xF8;
	// Reset I2C timeout timer.
	u8_i2c_ping_timeout=0;
	// Universal states.
	if((tmp_data==I2C_STAT_BUS_ERR)||(tmp_data==I2C_STAT_NOARB))
	{
		// Bus error, lost arbitration.
		u8_i2c_error = I2C_ERR_NO;
#ifdef I2C_ENABLE_LOG
		UART_debug_add_flash_string(cch_dbg_i2c_00, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
		// Switch I2C off, terminate everything until next call.
		I2C_MASTER_NEXT_STEP;
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_total_bytes = 0;
		u8_i2c_current_byte = 0;
		__builtin_avr_delay_cycles(1);
		I2C_SW_OFF;
		// Manage online/offline state of the counter.
		if(u8_i2c_error_counter<I2C_MAX_ERRORS) u8_i2c_error_counter++;
	}
	else if((tmp_data==I2C_STAT_W_ADR_N)||(tmp_data==I2C_STAT_W_DATA_N)||(tmp_data==I2C_STAT_R_ADR_N))
	{
		// NACK while sending.
		u8_i2c_error = I2C_ERR_M_NACK;
#ifdef I2C_ENABLE_LOG
		UART_debug_add_flash_string(cch_dbg_i2c_20, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
		// Switch I2C off, terminate everything until next poll.
		I2C_DO_STOP;
		u8_i2c_mode = I2C_MODE_IDLE;
		u8_i2c_total_bytes = 0;
		u8_i2c_current_byte = 0;
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
#ifdef I2C_ENABLE_LOG		
		UART_debug_add_flash_string(cch_dbg_i2c_08, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif		
		// All transmittions start with writing command ID, A+W every time.
		// Send A+W.
		//I2C_set_data(I2C_COUNTER_ADDRESS);
		I2C_MASTER_NEXT_STEP;
	}
	else if(tmp_data==I2C_STAT_RESTART)
	{
		// RESTART is sent.
		// Send A+W or A+R.
		if((u8_i2c_mode==I2C_MODE_WR_SPD)||(u8_i2c_mode==I2C_MODE_WR_POS)||(u8_i2c_mode==I2C_MODE_WR_FLAGS)||
			(u8_i2c_mode==I2C_MODE_RD_MOVE_S)||(u8_i2c_mode==I2C_MODE_RD_POS_S)||(u8_i2c_mode==I2C_MODE_RD_SPD_S)||(u8_i2c_mode==I2C_MODE_RD_D_S))
		{
#ifdef I2C_ENABLE_LOG			
			UART_debug_add_flash_string(cch_dbg_i2c_10w, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif			
			// Send A+W.
			//I2C_set_data(I2C_COUNTER_ADDRESS);
		}
		else
		{
#ifdef I2C_ENABLE_LOG			
			UART_debug_add_flash_string(cch_dbg_i2c_10r, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif			
			// Send A+R.
			//I2C_set_data(I2C_COUNTER_ADDRESS|I2C_READ_BIT);
		}
		I2C_MASTER_NEXT_STEP;
	}
	else if(tmp_data==I2C_STAT_W_ADR_A)
	{
		// A+W is sent, ACK received.
		// Pre-set data limits.
		u8_i2c_total_bytes = 0;
		u8_i2c_current_byte = 0;
#ifdef I2C_ENABLE_LOG		
		UART_debug_add_flash_string(cch_dbg_i2c_18, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif		
		// Need to send command byte.
		if(u8_i2c_mode==I2C_MODE_WR_D)
		{
			//I2C_set_data(I2C_CMD_SET_DIAM);
			// Set data limits.
			//u8_i2c_total_bytes = 2;
			// Fill some data to be sent.
			//u8a_i2c_buffer[0]=(u16_roller_diam&0xFF);
			//u8a_i2c_buffer[1]=((u16_roller_diam>>8)&0xFF);
		}
		I2C_MASTER_NEXT_STEP;
	}
	else if(tmp_data==I2C_STAT_W_DATA_A)
	{
		// Data byte is sent, ACK received.
		if(u8_i2c_current_byte<u8_i2c_total_bytes)
		{
#ifdef I2C_ENABLE_LOG
			UART_debug_add_flash_string(cch_dbg_i2c_28a, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
			// Output next byte from queue.
			I2C_set_data(u8a_i2c_buffer[u8_i2c_current_byte]);
			u8_i2c_current_byte++;
			I2C_MASTER_NEXT_STEP;
		}
		else
		{
			// Transmittion finished.
#ifdef I2C_ENABLE_LOG
			UART_debug_add_flash_string(cch_dbg_i2c_28b, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
			// Choose next I2C mode (part of the default I2C modes route).
			if(u8_i2c_mode==I2C_MODE_WR_STR) u8_i2c_mode = I2C_MODE_WR_SPD;
			else if(u8_i2c_mode==I2C_MODE_WR_SPD) u8_i2c_mode = I2C_MODE_WR_FLAGS;
			else if(u8_i2c_mode==I2C_MODE_WR_FLAGS) u8_i2c_mode = I2C_MODE_RD_MOVE_S;
			else if(u8_i2c_mode==I2C_MODE_RD_MOVE_S) u8_i2c_mode = I2C_MODE_RD_MOVE_R;
			else if(u8_i2c_mode==I2C_MODE_RD_D_S) u8_i2c_mode = I2C_MODE_RD_D_R;
			else if(u8_i2c_mode==I2C_MODE_RD_POS_S) u8_i2c_mode = I2C_MODE_RD_POS_R;
			else if(u8_i2c_mode==I2C_MODE_RD_SPD_S) u8_i2c_mode = I2C_MODE_RD_SPD_R;
			else u8_i2c_mode = I2C_MODE_IDLE;
			if(u8_i2c_mode==I2C_MODE_IDLE)
			{
				// If going to idle = terminate transmittion.
				I2C_DO_STOP;
			}
			else
			{
				// In all other modes need to do RESTART.
				I2C_DO_RESTART;
			}
		}
	}
	// Master Receiver states.
	else if(tmp_data==I2C_STAT_R_ADR_A)
	{
		// A+R is sent, ACK received.
		// Pre-set data limits.
		u8_i2c_total_bytes = 1;
		u8_i2c_current_byte = 1;
		// Set data limits.
		if(u8_i2c_mode==I2C_MODE_RD_MOVE_R) u8_i2c_total_bytes = 1;
		else if(u8_i2c_mode==I2C_MODE_RD_POS_R) u8_i2c_total_bytes = 4;
		else if(u8_i2c_mode==I2C_MODE_RD_SPD_R) u8_i2c_total_bytes = 4;
		else if(u8_i2c_mode==I2C_MODE_RD_D_R) u8_i2c_total_bytes = 2;
		// Set (N)ACK action for the next received byte.
		if(u8_i2c_total_bytes<2)
		{
#ifdef I2C_ENABLE_LOG
			UART_debug_add_flash_string(cch_dbg_i2c_40a, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
			I2C_READ_ONE;
		}
		else
		{
#ifdef I2C_ENABLE_LOG
			UART_debug_add_flash_string(cch_dbg_i2c_40b, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
			I2C_READ_NEXT;
		}
	}
	else if(tmp_data==I2C_STAT_R_DATA_A)
	{
		// Data byte is received, ACK returned.
		tmp_data = I2C_GET_DATA;
		// Put received byte into the buffer.
		u8a_i2c_buffer[(u8_i2c_current_byte-1)] = tmp_data;
		u8_i2c_current_byte++;
		if(u8_i2c_current_byte==u8_i2c_total_bytes)
		{
#ifdef I2C_ENABLE_LOG
			UART_debug_add_flash_string(cch_dbg_i2c_50a, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
			// Next received byte will be NACKed.
			I2C_READ_ONE;
		}
		else
		{
#ifdef I2C_ENABLE_LOG
			UART_debug_add_flash_string(cch_dbg_i2c_50b, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
			// Next received byte will be ACKed.
			I2C_READ_NEXT;
		}
	}
	else if(tmp_data==I2C_STAT_R_DATA_N)
	{
#ifdef I2C_ENABLE_LOG
		UART_debug_add_flash_string(cch_dbg_i2c_58, (u8_dbg_enabled&DBG_EN_I2C_L1));
#endif
		// Last data byte is received, NACK returned.
		tmp_data = I2C_GET_DATA;
		// Put received byte into the buffer.
		if(u8_i2c_current_byte>0) u8a_i2c_buffer[(u8_i2c_current_byte-1)] = tmp_data;
		if(u8_i2c_mode==I2C_MODE_RD_MOVE_R)
		{
			// Update "motion detected" flag.
			//if((tmp_data&I2C_FLAG_MOTION)==0) u8_state&=~STAT_MOTION; else u8_state|=STAT_MOTION;
			// Update "motion direction" flag.
			//if((tmp_data&I2C_FLAG_REVERSE)==0) u8_state&=~STAT_MOVE_REV; else u8_state|=STAT_MOVE_REV;
			// Check "BAD EEPROM" flag.
			//if((tmp_data&I2C_FLAG_BAD_EEP)==0)
			{
				// Counter EEPROM is OK.
				//u8_i2c_flags&=~(I2C_HW_BAD_COUNTER|I2C_HW_RST_BAD_CNT);
			}
			//else
			{
				// Counter EEPROM is bad.
				//u8_i2c_flags|=I2C_HW_BAD_COUNTER;
			}
			// RESTART.
			I2C_DO_RESTART;
			// Next: read position data.
			u8_i2c_mode=I2C_MODE_RD_POS_S;
		}
		else if(u8_i2c_mode==I2C_MODE_RD_SPD_R)
		{
			// STOP.
			I2C_DO_STOP;
			// Store current tape speed.
			//u32_tape_speed=(((uint32_t)u8a_i2c_buffer[3]<<24)&0xFF000000);
			//u32_tape_speed+=(((uint32_t)u8a_i2c_buffer[2]<<16)&0xFF0000);
			//u32_tape_speed+=(((uint16_t)u8a_i2c_buffer[1]<<8)&0xFF00);
			//u32_tape_speed+=u8a_i2c_buffer[0];
			// Next: switch to idle.
			u8_i2c_mode=I2C_MODE_IDLE;
			u8_i2c_total_bytes=0;
			u8_i2c_current_byte=0;
			// Reset connection retry counter.
			u8_i2c_error_counter=0;
			// Set flag "I2C is running".
			//u8_i2c_flags|=I2C_HW_RUNNING;
		}
		else
		{
			// Absurd situation.
			// STOP.
			I2C_DO_STOP;
			// Reset.
			u8_i2c_mode=I2C_MODE_IDLE;
			u8_i2c_total_bytes=0;
			u8_i2c_current_byte=0;
			// Shutdown.
			I2C_SW_OFF;
		}
	}
#ifdef I2C_ENABLE_LOG
	UART_dump_out();
#endif
}
#endif /* I2C_MASTER_EN */

#ifdef I2C_SLAVE_EN
inline void I2C_slave_processor(void)
{
	uint8_t tmp_data;
	tmp_data=I2C_STATUS&0xF8;
	if(tmp_data==I2C_STAT_BUS_ERR)
	{
		// Bus error detected, need to recover.
		I2C_RECOVER_ERROR;
		I2C_SW_OFF;
		u8_i2c_mode=I2C_MODE_IDLE;
		u8_i2c_total_bytes=0;
		u8_i2c_current_byte=0;
		u8_i2c_last_command=I2C_CMD_UNKNOWN;
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
		u8_i2c_mode=I2C_MODE_IDLE;
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
				u8_i2c_total_bytes=2;
				u8_i2c_current_byte=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_SET_POS)
			{
				// Set tape speed.
				u8_i2c_total_bytes=4;
				u8_i2c_current_byte=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_SET_SPEED)
			{
				// Set tape speed.
				u8_i2c_total_bytes=2;
				u8_i2c_current_byte=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_SET_FLAGS)
			{
				// Set misc. flags for the counter.
				u8_i2c_total_bytes=1;
				u8_i2c_current_byte=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				// Prepare for the next data byte.
				I2C_READ_NEXT;
			}
			else if(tmp_data==I2C_CMD_CLEAR)
			{
				// Clear tape counter.
				u8_i2c_total_bytes=0;
				u8_i2c_current_byte=0;
				u8_i2c_last_command=tmp_data;
				u8_i2c_mode=I2C_MODE_RECEIVE;
				clear_counter();
				// Prepare for the one data byte.
				I2C_READ_ONE;
			}
			else if(tmp_data==I2C_CMD_DISP_STR)
			{
				// Set symbols to display.
				u8_i2c_total_bytes=4;
				u8_i2c_current_byte=0;
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
				u8_i2c_mode=I2C_MODE_IDLE;
				u8_i2c_total_bytes=0;
				u8_i2c_current_byte=0;
				u8_i2c_last_command=I2C_CMD_UNKNOWN;
				I2C_INIT_SLAVE;
			}
		}
		else if(u8_i2c_mode==I2C_MODE_RECEIVE)
		{
			// Put received byte into the buffer.
			if(u8_i2c_current_byte<u8_i2c_total_bytes)
			{
				u8a_i2c_buffer[u8_i2c_current_byte]=tmp_data;
				u8_i2c_current_byte++;
				if(u8_i2c_current_byte==u8_i2c_total_bytes)
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
				if(u8_i2c_current_byte==u8_i2c_total_bytes)
				{
					u8_i2c_current_byte++;
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
			u8_i2c_total_bytes=0;
			u8_i2c_current_byte=0;
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
			if(u8_i2c_current_byte<=u8_i2c_total_bytes)
			{
				u8a_i2c_buffer[u8_i2c_current_byte]=tmp_data;
				u8_i2c_current_byte++;
				// Last byte received, do something.
				I2C_dispatcher();
			}
		}
		u8_i2c_mode=I2C_MODE_IDLE;
		u8_i2c_total_bytes=0;
		u8_i2c_current_byte=0;
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
			u8_i2c_total_bytes=2;
			u8_i2c_current_byte=0;
			// Calculate diameter.
			u16_tmp=(uint16_t)set_data.u32i_roller_length/PI_CONST;
			u8a_i2c_buffer[0]=(u16_tmp&0xFF);
			u8a_i2c_buffer[1]=((u16_tmp>>8)&0xFF);
			// Output first byte.
			I2C_set_data(u8a_i2c_buffer[0]);
			u8_i2c_current_byte++;
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
			u8_i2c_total_bytes=4;
			u8_i2c_current_byte=0;
			// Put data into buffer.
			u8a_i2c_buffer[0]=(tmp32&0xFF);
			u8a_i2c_buffer[1]=((tmp32>>8)&0xFF);
			u8a_i2c_buffer[2]=((tmp32>>16)&0xFF);
			u8a_i2c_buffer[3]=((tmp32>>24)&0xFF);
			// Output first byte.
			I2C_set_data(u8a_i2c_buffer[0]);
			u8_i2c_current_byte++;
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
			u8_i2c_total_bytes=4;
			u8_i2c_current_byte=0;
			// Put data into buffer.
			u8a_i2c_buffer[0]=(u32_tape_speed&0xFF);
			u8a_i2c_buffer[1]=((u32_tape_speed>>8)&0xFF);
			u8a_i2c_buffer[2]=((u32_tape_speed>>16)&0xFF);
			u8a_i2c_buffer[3]=((u32_tape_speed>>24)&0xFF);
			// Output first byte.
			I2C_set_data(u8a_i2c_buffer[0]);
			u8_i2c_current_byte++;
			u8_i2c_mode=I2C_MODE_TRANSMIT;
			I2C_WRITE_NEXT;
		}
		else if(u8_i2c_last_command==I2C_CMD_GET_MOTION)
		{
			uint8_t u8_i2c_flags=0, tmp8;
			// Send misc. flags: direction of motion, cal warning, ...
			u8_i2c_total_bytes=1;
			u8_i2c_current_byte=1;
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
		u8_i2c_total_bytes=0;
		u8_i2c_current_byte=0;
		u8_i2c_last_command=I2C_CMD_UNKNOWN;
		I2C_TRANSMITTION_END;
	}
	else if(tmp_data==I2C_STAT_RD_DATA_A)
	{
		// Addressed with A+R, byte was transmitted, ACK received.
		// Determine next step.
		if(u8_i2c_mode==I2C_MODE_TRANSMIT)
		{
			if(u8_i2c_current_byte<u8_i2c_total_bytes)
			{
				I2C_set_data(u8a_i2c_buffer[u8_i2c_current_byte]);
				u8_i2c_current_byte++;
				if(u8_i2c_current_byte==u8_i2c_total_bytes)
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
		u8_i2c_total_bytes=0;
		u8_i2c_current_byte=0;
		u8_i2c_last_command=I2C_CMD_UNKNOWN;
		I2C_INIT_SLAVE;
	}
}
#endif /* I2C_SLAVE_EN */

#endif /* FGR_DRV_I2C_HW_FOUND */