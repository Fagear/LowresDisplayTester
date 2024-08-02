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

Created: 2024-08

Part of the [LowresDisplayTester] project.
Strings and custom symbols for testing Cyrillic support.
This file is intentionally saved in CP1251 encoding for ROM strings to be correct!

**************************************************************************************************************************************************************/

#ifndef CP1251_H_
#define CP1251_H_

#ifdef CYR_TESTS
#include <avr/pgmspace.h>

const int8_t cp1251_lc1[] PROGMEM = "àáâãäå¸æçèéêëìíî";
const int8_t cp1251_lc2[] PROGMEM = "ïðñòóôõö÷øùúûüýþÿ";
const int8_t cp1251_uc1[] PROGMEM = "ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎ";
const int8_t cp1251_uc2[] PROGMEM = "ÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß";
const int8_t cp1251_symbols1[] PROGMEM = "§«Ðàçíûå»‹çíàêè›";
const int8_t cp1251_symbols2[] PROGMEM = "–¸é‘¤’“®”•—„©”—¶ ";

// Letter "á".
const int8_t usr_char_b_lc[8] PROGMEM =
{
	0b00000010,
	0b00001100,
	0b00010000,
	0b00011110,
	0b00010001,
	0b00010001,
	0b00001110,
	0b00000000,
};

// Letter "Á".
const int8_t usr_char_b_uc[8] PROGMEM =
{
	0b00011111,
	0b00010000,
	0b00011110,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011110,
	0b00000000,
};

// Letter "â".
const int8_t usr_char_v_lc[8] PROGMEM =
{
	0b00001100,
	0b00010010,
	0b00010100,
	0b00011110,
	0b00010001,
	0b00010001,
	0b00001110,
	0b00000000,
};

// Letter "ã".
const int8_t usr_char_g_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00011110,
	0b00010010,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00000000,
};

// Letter "Ã".
const int8_t usr_char_g_uc[8] PROGMEM =
{
	0b00011111,
	0b00010001,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00000000,
};

// Letter "æ".
const int8_t usr_char_j_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00010101,
	0b00010101,
	0b00001110,
	0b00010101,
	0b00010101,
	0b00000000,
};

// Letter "Æ".
const int8_t usr_char_j_uc[8] PROGMEM =
{
	0b00010101,
	0b00010101,
	0b00010101,
	0b00001110,
	0b00010101,
	0b00010101,
	0b00010101,
	0b00000000,
};

// Letter "ë".
const int8_t usr_char_l_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000111,
	0b00000101,
	0b00000101,
	0b00010101,
	0b00001001,
	0b00000000,
};

// Letter "Ë".
const int8_t usr_char_l_uc[8] PROGMEM =
{
	0b00000111,
	0b00001001,
	0b00001001,
	0b00001001,
	0b00001001,
	0b00001001,
	0b00010001,
	0b00000000,
};

// Letter "ô".
const int8_t usr_char_f_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00000100,
	0b00001110,
	0b00010101,
	0b00010101,
	0b00001110,
	0b00000100,
};

// Letter "Ô".
const int8_t usr_char_f_uc[8] PROGMEM =
{
	0b00000100,
	0b00001110,
	0b00010101,
	0b00010101,
	0b00001110,
	0b00000100,
	0b00000100,
	0b00000000,
};

// Letter "û".
const int8_t usr_char_yi_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00010001,
	0b00010001,
	0b00011001,
	0b00010101,
	0b00011001,
	0b00000000,
};

// Letter "Û".
const int8_t usr_char_yi_uc[8] PROGMEM =
{
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011001,
	0b00010101,
	0b00010101,
	0b00011001,
	0b00000000,
};

// Letter "ý".
const int8_t usr_char_e_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00001110,
	0b00010001,
	0b00000111,
	0b00010001,
	0b00001110,
	0b00000000,
};

// Letter "Ý".
const int8_t usr_char_e_uc[8] PROGMEM =
{
	0b00001110,
	0b00010001,
	0b00000001,
	0b00000111,
	0b00000001,
	0b00010001,
	0b00001110,
	0b00000000,
};

// Letter "þ".
const int8_t usr_char_ju_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00010010,
	0b00010101,
	0b00011101,
	0b00010101,
	0b00010010,
	0b00000000,
};

// Letter "Þ".
const int8_t usr_char_ju_uc[8] PROGMEM =
{
	0b00010010,
	0b00010101,
	0b00010101,
	0b00011101,
	0b00010101,
	0b00010101,
	0b00010010,
	0b00000000,
};

// Letter "ÿ".
const int8_t usr_char_ja_lc[8] PROGMEM =
{
	0b00000000,
	0b00000000,
	0b00001111,
	0b00010001,
	0b00001111,
	0b00000101,
	0b00010001,
	0b00000000,
};

// Letter "ß".
const int8_t usr_char_ja_uc[8] PROGMEM =
{
	0b00001111,
	0b00010001,
	0b00010001,
	0b00001111,
	0b00000101,
	0b00001001,
	0b00010001,
	0b00000000,
};

#endif /* CYR_TESTS */

#endif /* CP1251_H_ */