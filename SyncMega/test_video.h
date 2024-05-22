/**************************************************************************************************************************************************************
test_video.h

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

Created: 2024-02

Part of the [LowresDisplayTester] project.
Timing constants for sync generation via Timer PWM on AVR MCUs.
Generation for:
- composite sync 525i30
- composite sync 625i25
- horizontal + vertical sync CGA/EGA 60Hz
- horizontal + vertical sync VGA 640x480@60

**************************************************************************************************************************************************************/

#ifndef TEST_VIDEO_H_
#define TEST_VIDEO_H_

// Timings are for sync timer (Tsync) running @ 16 MHz (62.5 ns/tick), active line timer (Tact) running @ 2 MHz (500 ns/tick).

// Sync timing constants.
enum
{
	H_STBL_DELAY			= 51,	// Delay to stabilize active region timing
	A_MONO_STBL_DELAY		= 13,	// Delay to stabilize bar drawing (for monochrome output)
	A_RGB_STBL_DELAY		= 8,	// Delay to stabilize bar drawing (for RGB output)
	
	COMP_ACT_DELAY_525i		= 89,	// (Tact) Timer value for start of active part of the line for composite 525i (~5.5us)
	COMP_ACT_LEN_525i		= 103,	// (Tact) Duration of the active part of the line for composite 525i (front porch ~1.6us)
	COMP_LINE_LEN_525i		= 1015,	// (Tsync) Line duration in field mode (normal line timing) for composite 525i (~63.5us)
	COMP_HALF_LEN_525i		= 507,	// (Tsync) Half-line duration in field sync region for composite 525i (~31.8us)
	COMP_SYNC_V_LEN_525i	= 432,	// (Tsync) Duration of negative pulse for composite vertical sync pulses (~27us)
	COMP_SYNC_H_LEN_525i	= 74,	// (Tsync) Duration of negative pulse for composite normal horizontal sync (~4.6us)
	COMP_EQ_PULSE_LEN_525i	= 36,	// (Tsync) Duration of negative pulse for composite equalization pulses (~2.3us)
	
	COMP_ACT_DELAY_625i		= 87,	// (Tact) Timer value for start of active part of the line for composite 525i (~5.9us)
	COMP_ACT_LEN_625i		= 103,	// (Tact) Duration of the active part of the line for composite 625i (front porch ~1.5us)
	COMP_LINE_LEN_625i		= 1023,	// (Tsync) Line duration in field mode (normal line timing) for composite 625i (~64us)
	COMP_HALF_LEN_625i		= 511,	// (Tsync) Half-line duration in field sync region for composite 625i (~32us)
	COMP_SYNC_V_LEN_625i	= 435,	// (Tsync) Duration of negative pulse for composite vertical sync pulses (~27.2us)
	COMP_SYNC_H_LEN_625i	= 75,	// (Tsync) Duration of negative pulse for composite normal horizontal sync (~4.7us)
	COMP_EQ_PULSE_LEN_625i	= 37,	// (Tsync) Duration of negative pulse for composite equalization pulses (~2.4us)
	
	EGA_ACT_DELAY			= 73,	// (Tact) Timer value for start of active part of the line for CGA/EGA
	EGA_ACT_LEN				= 85,	// (Tact) Duration of the active part of the line for CGA/EGA
	EGA_LINE_LEN			= 1017,	// (Tsync) Line duration for CGA/EGA
	EGA_SYNC_H_LEN			= 64,	// (Tsync) Duration of positive pulse for CGA/EGA vertical sync pulses
	
	VGA_ACT_DELAY			= 43,	// (Tact) Timer value for start of active part of the line for VGA 640x480 (~1.9us back porch)
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

// CGA/EGA 60Hz step/line count constants.
enum
{
	EGA_HS_START			= 0,	// EGA, start of horizontal sync
	EGA_VS_STOP				= 2,	// EGA, end of vertical sync, start of back porch + top border
	EGA_BP_STOP				= 20,	// EGA, end of back porch + top border, start of normal video lines
	EGA_FP_START			= 250,	// EGA, end of normal video lines, start of bottom border + front porch
	EGA_VS_START			= 260,	// EGA, start of vertical sync
};

// VGA 640x480 60Hz step/line count constants.
enum
{
	VGA_HS_START			= 0,	// VGA, start of horizontal sync
	VGA_VS_STOP				= 1,	// VGA, end of vertical sync, start of back porch + top border
	VGA_BP_STOP				= 32,	// VGA, end of back porch + top border, start of normal video lines
	VGA_FP_START			= 515,	// VGA, end of normal video lines, start of bottom border + front porch
	VGA_VS_START			= 524,	// VGA, start of vertical sync
};

// Lines per frame count constants.
enum
{
	LINES_COMP625			= 625,	// Number of video lines for composite 625i frame
	LINES_COMP525			= 525,	// Number of video lines for composite 525i frame
	LINES_EGA				= 261,	// Number of video lines for CGA/EGA 60Hz (pos/pos sync)
	LINES_VGA				= 524,	// Number of video lines for VGA 640x480 60Hz (neg/neg sync)
};

// Frames per second constants.
enum
{
	FPS_COMP625				= 25,
	FPS_COMP525				= 30,
	FPS_EGA					= 60,
	FPS_VGA					= 60,
};

#endif /* TEST_VIDEO_H_ */
