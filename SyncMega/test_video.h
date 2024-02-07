/*
 * test_video.h
 *
 * Created: 06.02.2024 13:38:28
 *  Author: kryukov
 */ 


#ifndef TEST_VIDEO_H_
#define TEST_VIDEO_H_

// Timings are for sync timer (Tsync) running @ 16 MHz (62.5 ns/tick), active line timer (Tact) running @ 2 MHz (500 ns/tick).

// Sync timing constants.
enum
{
	COMP_ACT_DELAY_525i		= 15,	// (Tact) Timer value for start of active part of the line for composite 525i (~5.5us)
	COMP_ACT_LEN_525i		= 101,	// (Tact) Duration of the active part of the line for composite 525i (front porch ~1.6us)
	COMP_LINE_LEN_525i		= 1015,	// (Tsync) Line duration in field mode (normal line timing) for composite 525i (~63.5us)
	COMP_HALF_LEN_525i		= 507,	// (Tsync) Half-line duration in field sync region for composite 525i (~31.8us)
	COMP_SYNC_V_LEN_525i	= 432,	// (Tsync) Duration of negative pulse for composite vertical sync pulses (~27us)
	COMP_SYNC_H_LEN_525i	= 74,	// (Tsync) Duration of negative pulse for composite normal horizontal sync (~4.6us)
	COMP_EQ_PULSE_LEN_525i	= 36,	// (Tsync) Duration of negative pulse for composite equalization pulses (~2.3us)
	
	COMP_ACT_DELAY_625i		= 15,	// (Tact) Timer value for start of active part of the line for composite 525i (~5.9us)
	COMP_ACT_LEN_625i		= 103,	// (Tact) Duration of the active part of the line for composite 625i (front porch ~1.5us)
	COMP_LINE_LEN_625i		= 1023,	// (Tsync) Line duration in field mode (normal line timing) for composite 625i (~64us)
	COMP_HALF_LEN_625i		= 511,	// (Tsync) Half-line duration in field sync region for composite 625i (~32us)
	COMP_SYNC_V_LEN_625i	= 435,	// (Tsync) Duration of negative pulse for composite vertical sync pulses (~27.2us)
	COMP_SYNC_H_LEN_625i	= 75,	// (Tsync) Duration of negative pulse for composite normal horizontal sync (~4.7us)
	COMP_EQ_PULSE_LEN_625i	= 37,	// (Tsync) Duration of negative pulse for composite equalization pulses (~2.4us)
	
	VGA_ACT_DELAY			= 6,	// (Tact) Timer value for start of active part of the line for VGA 640x480 (~1.9us back porch)
	VGA_ACT_LEN				= 47,	// (Tact) Duration of the active part of the line for VGA 640x480 (~25.5us)
	VGA_LINE_LEN			= 507,	// (Tsync) Line duration for VGA 640x480 (~31.8us)
	VGA_SYNC_H_LEN			= 60,	// (Tsync) Duration of negative pulse for VGA 640x480 vertical sync pulses (~3.8us)
};

// Composite video 625i (PAL/SECAM) step count constants.
enum
{
	ST_COMP625_F1_VS_START	= 0,	// 625i, field 1, end of pre-EQ pulses, start of vertical sync
	ST_COMP625_F1_VS_STOP	= 4,	// 625i, field 1, end of vertical sync, start of post-EQ pulses
	ST_COMP625_F1_EQ_STOP	= 9,	// 625i, field 1, end of post-EQ pulses, start of normal video lines
	ST_COMP625_F1_EQ_START	= 314,	// 625i, field 1, end of normal video lines, start of pre-EQ pulses
	ST_COMP625_F2_VS_START	= 319,	// 625i, field 2, end of pre-EQ pulses, start of vertical sync
	ST_COMP625_F2_VS_STOP	= 324,	// 625i, field 2, end of vertical sync, start of post-EQ pulses
	ST_COMP625_F2_EQ_STOP	= 329,	// 625i, field 2, end of post-EQ pulses, start of normal video lines
	ST_COMP625_F2_EQ_START	= 634,	// 625i, field 2, end of normal video lines, start of pre-EQ pulses
	ST_COMP625_LOOP			= 639,	// Maximum number of steps for 625i loop
};

// Composite video 625i (PAL/SECAM) line count constants.
enum
{
	ST_COMP625_F1_ACT_START	= 23,	// 625i, field 1, start of active region
	ST_COMP625_F1_ACT_STOP	= 310,	// 625i, field 1, end of active region
	ST_COMP625_F2_ACT_START	= 335,	// 625i, field 2, start of active region
	ST_COMP625_F2_ACT_STOP	= 622,	// 625i, field 2, end of active region
};

// Composite video 525i (NTSC) step count constants.
enum
{
	ST_COMP525_F1_EQ_START	= 0,	// 525i, field 1, start of pre-EQ pulses
	ST_COMP525_F1_VS_START	= 5,	// 525i, field 1, end of pre-EQ pulses, start of vertical sync
	ST_COMP525_F1_VS_STOP	= 11,	// 525i, field 1, end of vertical sync, start of post-EQ pulses
	ST_COMP525_F1_EQ_STOP	= 17,	// 525i, field 1, end of post-EQ pulses, start of normal video lines
	ST_COMP525_F2_EQ_START	= 271,	// 525i, field 2, end of normal video lines, start of pre-EQ pulses
	ST_COMP525_F2_VS_START	= 277,	// 525i, field 2, end of pre-EQ pulses, start of vertical sync
	ST_COMP525_F2_VS_STOP	= 283,	// 525i, field 2, end of vertical sync, start of post-EQ pulses
	ST_COMP525_F2_EQ_STOP	= 289,	// 525i, field 2, end of post-EQ pulses, start of normal video lines
	ST_COMP525_LOOP			= 542,	// Maximum number of steps for 525i loop
};

// Composite video 525i (NTSC) line count constants.
enum
{
	ST_COMP525_F1_ACT_START	= 20,	// 525i, field 1, start of active region
	ST_COMP525_F1_ACT_STOP	= 262,	// 525i, field 1, end of active region
	ST_COMP525_F2_ACT_START	= 283,	// 525i, field 2, start of active region
	ST_COMP525_F2_ACT_STOP	= 525,	// 525i, field 2, end of active region
};

// VGA 640x480 60Hz step/line count constants.
enum
{
	VGA_HS_START			= 0,	// VGA, start of horizontal sync
	VGA_VS_STOP				= 1,	// VGA, end of vertical sync, start of back porch + top border
	VGA_BP_STOP				= 32,	// VGA, end of back porch + top border, start of normal video lines
	VGA_FP_START			= 515,	// VGA, end of normal video lines, start of bottom border + front porch
	VGA_VS_START			= 525,	// VGA, start of vertical sync
};

// Lines per frame count constants.
enum
{
	LINES_COMP625			= 625,	// Number of video lines for composite 625i frame
	LINES_COMP525			= 525,	// Number of video lines for composite 525i frame
	LINES_VGA				= 525,	// Number of video lines for VGA 640x480 60Hz (neg/neg sync)
};

// Frames per second constants.
enum
{
	FPS_COMP625				= 25,
	FPS_COMP525				= 30,
	FPS_VGA					= 60,
};

#endif /* TEST_VIDEO_H_ */