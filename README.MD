# LowresDisplayTester

## What is this?

**LowresDisplayTester** is firmware for AVR MCUs that enables creation of low-part-count and low-cost video generator (sync generator/black burst generator/genlock) device.

## Features

- Minimalistic and cheap circuitry
- Switchable video generation standard (see list below)
- Selectable sync only or containing video output
- Additional outputs for VFD/LED/LCD/OLED displays

## Video outputs

- 525i/29.97 NTSC (no color)
- 625i/25 PAL/SECAM (no color)
- 640x480 60Hz VGA
- 640x200 60Hz CGA/EGA

Only one video standard can be selected at any time.

## Display ports
- HD44780-compatible (HD44780/KS0066/КБ1013ВГ6/PT6314/SPLC780/US2066) character display port with 4-bit data bus

## Usage

Sorry, no electrical schematics at the moment...

MCU must be clocked from external 16.0 MHz Xtal for precise timing.

<details>
<summary>AVR fuse information</summary>

Fuses for **ATmega328P** with *16 MHz Xtal*:
- **SUT0** = 0
- **CKSEL3** = 0
- **SPIEN** = 0
- **EESAVE** = 0
- **BODLEVEL1** = 0
- **BODLEVEL0** = 0
- all other at "1"

In hex form:
- low byte: **0xE7**
- high byte: **0xD7**
- extended byte: **0xFC**
</details>

Due to high clock and dependance of output analog levels on logic levels MCU should be run on voltages from **4.1 V** to **5.0 V**.

<details>
<summary>Pinout for ATmega 48/88/168/328 MCU</summary>

Power supply:
- **pin 7** *(VCC)*: +5 V supply
- **pin 20** *(AVCC)*: +5 V supply
- **pin 8** *(GND)*: 0 V (common)
- **pin 22** *(GND)*: 0 V (common)

Clock input:
- **pin 9** *(PB6)*: 16.0 MHz Xtal
- **pin 10** *(PB7)*: 16.0 MHz Xtal

User input:
- **pin 14** *(PB0)*: (input) ***video select*** switch, bit 0
- **pin 18** *(PB4)*: (input) ***video select*** switch, bit 1

Video output:
- **pin 15** *(PB1)*: (output) composite/horizontal ***sync output***
- **pin 4** *(PD2)*: (input) sync input (loopback from *pin 15*) for additional timing
- **pin 12** *(PD6)*: (output) line ***active part*** signal output
- **pin 16** *(PB2)*: (output) ***vertical sync*** output
- **pin 19** *(PB5)*: (output) ***bar pattern*** output

HD44780-compatible character display parallel port:
- **pin 5** *(PD3)*: (output) ***HD44780 E*** (operation enable)
- **pin 11** *(PD5)*: (output) ***HD44780 R/W*** (read/write select)
- **pin 13** *(PD7)*: (output) ***HD44780 RS*** (command/data select)
- **pin 23** *(PC0)*: (input/output) ***HD44780 D4*** (data bit 4)
- **pin 24** *(PC1)*: (input/output) ***HD44780 D5*** (data bit 5)
- **pin 25** *(PC2)*: (input/output) ***HD44780 D6*** (data bit 6)
- **pin 26** *(PC3)*: (input/output) ***HD44780 D7*** (data bit 7)

I2C displays serial port:
- **pin 27** *(PC4)*: (input/output) ***I2C data***
- **pin 28** *(PC5)*: (output) ***I2C clock***

UART displays serial port:
- **pin 2** *(PD0)*: (input) ***UART data RX*** from display
- **pin 3** *(PD1)*: (output) ***UART data TX*** to display

</details>

> [!NOTE]
> "*Capstan motor drive*" is optional and can be left disconnected.

## Demo


## Current state
- Two-switch video selector works
- PAL and NTSC frame generation works ok, having a little horizontal jitter
- VGA, EGA and CGA frame generation works ok
- HD44780 driver works fine

## Plans
- Update I2C driver to working state
- Write driver for character displays with I2C bus
- Write driver for UART Futaba character displays
- Maybe add support for graphical displays

## License
Program copyright 2022-2024.

This program is free software.
Licensed under the Apache License, Version 2.0 (the "License");