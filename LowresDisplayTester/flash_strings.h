/**************************************************************************************************************************************************************
flash_strings.h

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

Created: 2022-06

Part of the [LowresDisplayTester] project.
Strings that are used for character display tests, stored in ROM. 

**************************************************************************************************************************************************************/

#ifndef FLASH_STRINGS_H_
#define FLASH_STRINGS_H_

#ifdef CONF_EN_CHARDISP

#include <avr/pgmspace.h>

// Rotating animation.
const int8_t usr_char_rot1[8] PROGMEM =
{
	0b00000000,
	0b00001110,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00001110,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_rot2[8] PROGMEM =
{
	0b00000000,
	0b00001110,
	0b00010011,
	0b00010101,
	0b00011001,
	0b00001110,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_rot3[8] PROGMEM =
{
	0b00000000,
	0b00001110,
	0b00010001,
	0b00011111,
	0b00010001,
	0b00001110,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_rot4[8] PROGMEM =
{
	0b00000000,
	0b00001110,
	0b00011001,
	0b00010101,
	0b00010011,
	0b00001110,
	0b00000000,
	0b00000000,
};

// Level bar graph animations.
const int8_t usr_char_lvl1[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_lvl2[8] PROGMEM =
{
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010101,
	0b00010101,
	0b00010000,
	0b00010000,
	0b00010000,
};

const int8_t usr_char_lvl3[8] PROGMEM =
{
	0b00010100,
	0b00010100,
	0b00010100,
	0b00010101,
	0b00010101,
	0b00010100,
	0b00010100,
	0b00010100,
};

const int8_t usr_char_lvl4[8] PROGMEM =
{
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl5[8] PROGMEM =
{
	0b00000000,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl6[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl7[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl8[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl9[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl10[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
};

const int8_t usr_char_lvl11[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010101,
};

const int8_t usr_char_fill[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl12[8] PROGMEM =
{
	0b00000000,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl13[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl14[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl15[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl16[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl17[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_lvl18[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
};

// Loading animation.
const int8_t usr_char_load1[8] PROGMEM =
{
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load2[8] PROGMEM =
{
	0b00011000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load3[8] PROGMEM =
{
	0b00011100,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load4[8] PROGMEM =
{
	0b00011110,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load5[8] PROGMEM =
{
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load6[8] PROGMEM =
{
	0b00001111,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load7[8] PROGMEM =
{
	0b00000111,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load8[8] PROGMEM =
{
	0b00000011,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load9[8] PROGMEM =
{
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load10[8] PROGMEM =
{
	0b00000000,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load11[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
};

const int8_t usr_char_load12[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
};

const int8_t usr_char_load13[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000011,
};

const int8_t usr_char_load14[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000001,
	0b00000001,
	0b00000111,
};

const int8_t usr_char_load15[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000001,
	0b00001111,
};

const int8_t usr_char_load17[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00011110,
};

const int8_t usr_char_load18[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00010000,
	0b00011100,
};

const int8_t usr_char_load19[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00011000,
};

const int8_t usr_char_load20[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
};

const int8_t usr_char_load21[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00000000,
};

const int8_t usr_char_load22[8] PROGMEM =
{
	0b00000000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load23[8] PROGMEM =
{
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load24[8] PROGMEM =
{
	0b00011000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load25[8] PROGMEM =
{
	0b00011100,
	0b00010000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_load26[8] PROGMEM =
{
	0b00011110,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_spiral1[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_spiral2[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_spiral3[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_spiral4[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_spiral5[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
};

const int8_t usr_char_spiral6[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
};

const int8_t usr_char_spiral7[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
};

const int8_t usr_char_spiral8[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000011,
};

const int8_t usr_char_spiral9[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000111,
};

const int8_t usr_char_spiral10[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00001111,
};

const int8_t usr_char_spiral11[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00011111,
};

const int8_t usr_char_spiral12[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral13[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral14[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral15[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00000001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral16[8] PROGMEM =
{
	0b00011111,
	0b00000001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral17[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral18[8] PROGMEM =
{
	0b00011111,
	0b00011001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral19[8] PROGMEM =
{
	0b00011111,
	0b00011101,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral20[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral21[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral22[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral23[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010001,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral24[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010001,
	0b00011111,
};

const int8_t usr_char_spiral25[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00011111,
};

const int8_t usr_char_spiral26[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010111,
	0b00011111,
};

const int8_t usr_char_spiral27[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral28[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00010011,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral29[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00010011,
	0b00011011,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral30[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00010011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral31[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral32[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral33[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011011,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_spiral34[8] PROGMEM =
{
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011011,
	0b00011111,
	0b00011111,
};

const int8_t usr_char_fgr1[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010111,
	0b00010011,
	0b00010111,
	0b00010111,
	0b00011111,
	0b00000000,
};

const int8_t usr_char_fgr2[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010111,
	0b00010101,
	0b00010101,
	0b00010001,
	0b00011111,
	0b00000000,
};

const int8_t usr_char_fgr3[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010101,
	0b00010011,
	0b00010101,
	0b00010101,
	0b00011111,
	0b00000000,
};

const int8_t usr_char_test1[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011111,
	0b00000000,
};

const int8_t usr_char_test2[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010111,
	0b00010011,
	0b00010111,
	0b00010001,
	0b00011111,
	0b00000000,
};

const int8_t usr_char_test3[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010111,
	0b00010001,
	0b00011101,
	0b00010001,
	0b00011111,
	0b00000000,
};

const int8_t usr_char_triag1[8] PROGMEM =
{
	0b00000001,
	0b00000001,
	0b00000011,
	0b00000011,
	0b00000111,
	0b00001111,
	0b00001111,
	0b00011111,
};

const int8_t usr_char_triag2[8] PROGMEM =
{
	0b00010000,
	0b00010000,
	0b00011000,
	0b00011000,
	0b00011100,
	0b00011110,
	0b00011110,
	0b00011111,
};

const int8_t usr_char_triag3[8] PROGMEM =
{
	0b00011111,
	0b00011110,
	0b00011110,
	0b00011100,
	0b00011000,
	0b00011000,
	0b00010000,
	0b00010000,
};

const int8_t usr_char_triag4[8] PROGMEM =
{
	0b00011111,
	0b00001111,
	0b00001111,
	0b00000111,
	0b00000011,
	0b00000011,
	0b00000001,
	0b00000001,
};

const int8_t chardisp_det[] PROGMEM = "Detected";
const int8_t chardisp_dsp_1x8[] PROGMEM = "[Row1x8]";
const int8_t chardisp_dsp_x16[] PROGMEM = "x16]";
const int8_t chardisp_dsp_x20[] PROGMEM = "x20]";
const int8_t chardisp_dsp_x24[] PROGMEM = "x24]";
const int8_t chardisp_dsp_x40[] PROGMEM = "x40]";
const int8_t chardisp_dsp_row2[] PROGMEM = "[Row  2>";
const int8_t chardisp_dsp_row3[] PROGMEM = "[Row 3";
const int8_t chardisp_dsp_row4[] PROGMEM = "-<Row 4>-";
const int8_t chardisp_dsp_row1_2[] PROGMEM = "[Row 1  ";
const int8_t chardisp_dsp_row2_2[] PROGMEM = "[Row 2 ]";
const int8_t chardisp_dsp_row4_2[] PROGMEM = "[Row 4";

#endif /* CONF_EN_CHARDISP */

#endif /* FLASH_STRINGS_H_ */