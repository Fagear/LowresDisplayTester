# LowresDisplayTester

## What is this?

**LowresDisplayTester** is a small device with AVR MCUs that enables creation of low-part-count and low-cost video generator (sync generator/black burst generator/genlock) device.
It also can be used as a rudimentary pattern generator, but not very stable one. Pattern generation can be disabled via a jumper on the PCB. This device also performs tests on character displays.

## Features

- Minimalistic and cheap circuitry
- Switchable video generation standard (see list below)
- Sync generation (composite/H+V)
- Selectable solid fill (or black level)
- Selectable bar pattern generation
- Additional outputs for VFD/LED/LCD/OLED character displays
- Compile-time configuration (via [config.h] file) to select features/firmware size

## Video outputs

- Composite RCA 525i/29.97 (NTSC no color, black level fill + white vertical bars)
- Composite RCA 625i/25 (PAL/SECAM no color, black level fill + white vertical bars)
- VGA 640x480 60Hz (blue fill + orange vertical bars)
- CGA/EGA 640x200 60Hz (blue fill + cyan/magenta vertical bars)

Only one video standard can be selected at any time.

## Display ports
- HD44780-compatible (HD44780/KS0066/КБ1013ВГ6/PT6314/SPLC780/US2066/etc.) character display port with 8-bit paraller data bus (used in 4-bit mode)

## Firmware

Device utilizes **Atmel AVR** MCU for all timing, video generation and data protocols. Firmware project is done in **AtmelStudio 7**.

Target MCU is **ATmega 168** (either variant). But firmware should be compilable and working on **ATmega 328** variants as well. **ATmega 48** and **ATmega 88** are not supported due to low ROM capacity. **ATmega 16** and **ATmega 32** are also supported (but require a different PCB).

MCU must be clocked from external **16.0 MHz Xtal** for precise timing.

Due to high clock and dependance of output analog levels on logic levels MCU should be run on voltages from **4.1 V** to **5.0 V**. Thus BOD (Brown-out Detector) is enabled by MCU fuses.

<details>
<summary>AVR fuses for ATmega328</summary>

- **SUT1** = 0
- **CKSEL3** = 0
- **SPIEN** = 0
- **BODLEVEL1** = 0
- **BODLEVEL0** = 0
- all other at "1"

In hex form:
- low byte: **0xD7**
- high byte: **0xDF**
- extended byte: **0xFC**
</details>

<details>
<summary>AVR fuses for ATmega168</summary>

- **SUT1** = 0
- **CKSEL3** = 0
- **SPIEN** = 0
- **BODLEVEL1** = 0
- **BODLEVEL0** = 0
- all other at "1"

In hex form:
- low byte: **0xD7**
- high byte: **0xDC**
- extended byte: **0xFF**
</details>

<details>
<summary>AVR fuses for ATmega16/ATmega32</summary>

- **BODLEVEL** = 0
- **BODEN** = 0
- **SUT1** = 0
- **SPIEN** = 0
- **CKOPT** = 0
- all other at "1"

In hex form:
- low byte: **0x1F**
- high byte: **0xCF**
</details>

## Hardware

Sorry, no electrical schematics at the moment.

Adapter fits onto single PCB. PCB project is done in **Sprint Layout 6**.
Sizes of the boards:
- **LRDT.328SMD.xx** (88x41 mm) for ATmega168/ATmega328 in TQFP package
- **LRDT.328DIP.xx** (78x53 mm) for ATmega168/ATmega328 in DIP package
- **LRDT.32DIP.xx** (78x62 mm) for ATmega16/ATmega32 in DIP package

"*xx*" at the end denotes revision of each PCB.

<details>
<summary>Pinout for ATmega 168/328 MCU in SMD packages</summary>

Power supply:
- **pin 4** *(VCC)*: +5 V supply
- **pin 6** *(VCC)*: +5 V supply
- **pin 18** *(AVCC)*: +5 V supply (BOD)
- **pin 20** *(AREF)*: +5 V reference
- **pin 3** *(GND)*: 0 V (common)
- **pin 5** *(GND)*: 0 V (common)
- **pin 21** *(GND)*: 0 V (common)

Clock input:
- **pin 7** *(PB6)*: 16.0 MHz Xtal
- **pin 8** *(PB7)*: 16.0 MHz Xtal

User input:
- **pin 12** *(PB0)*: (input) ***video select*** switch, bit 0
- **pin 16** *(PB4)*: (input) ***video select*** switch, bit 1

Video output:
- **pin 13** *(PB1)*: (output) composite/horizontal ***sync output***
- **pin 32** *(PD2)*: (input) sync input (loopback from *pin 15*) for additional timing
- **pin 10** *(PD6)*: (output) ***vertical sync*** output
- **pin 14** *(PB2)*: (output) line ***active part*** signal output
- **pin 17** *(PB5)*: (output) ***bar pattern*** output

HD44780-compatible character display parallel port:
- **pin 1** *(PD3)*: (output) ***HD44780 E*** (operation enable)
- **pin 9** *(PD5)*: (output) ***HD44780 R/W*** (read/write select)
- **pin 11** *(PD7)*: (output) ***HD44780 RS*** (command/data select)
- **pin 23** *(PC0)*: (input/output) ***HD44780 D4*** (data bit 4)
- **pin 24** *(PC1)*: (input/output) ***HD44780 D5*** (data bit 5)
- **pin 25** *(PC2)*: (input/output) ***HD44780 D6*** (data bit 6)
- **pin 26** *(PC3)*: (input/output) ***HD44780 D7*** (data bit 7)

I2C displays serial port:
- **pin 27** *(PC4)*: (input/output) ***I2C data***
- **pin 28** *(PC5)*: (output) ***I2C clock***

UART displays serial port:
- **pin 30** *(PD0)*: (input) ***UART data RX*** from display
- **pin 31** *(PD1)*: (output) ***UART data TX*** to display
- **pin 2** *(PD4)*: (input) ***Busy*** signal from display

</details>

<details>
<summary>Pinout for ATmega 168/328 MCU in DIP packages</summary>

Power supply:
- **pin 7** *(VCC)*: +5 V supply
- **pin 20** *(AVCC)*: +5 V supply (BOD)
- **pin 21** *(AREF)*: +5 V reference
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
- **pin 12** *(PD6)*: (output) ***vertical sync*** output
- **pin 16** *(PB2)*: (output) line ***active part*** signal output
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
- **pin 6** *(PD4)*: (input) ***Busy*** signal from display

</details>

<details>
<summary>Pinout for ATmega 16/32 MCU in DIP packages</summary>

Power supply:
- **pin 10** *(VCC)*: +5 V supply
- **pin 30** *(AVCC)*: +5 V supply (BOD)
- **pin 32** *(AREF)*: +5 V reference
- **pin 11** *(GND)*: 0 V (common)
- **pin 31** *(GND)*: 0 V (common)

Clock input:
- **pin 12** *(XTAL2)*: 16.0 MHz Xtal
- **pin 13** *(XTAL1)*: 16.0 MHz Xtal

User input:
- **pin 29** *(PC7)*: (input) ***video select*** switch, bit 0
- **pin 28** *(PC6)*: (input) ***video select*** switch, bit 1

Video output:
- **pin 19** *(PD5)*: (output) composite/horizontal ***sync output***
- **pin 16** *(PD2)*: (input) sync input (loopback from *pin 19*) for additional timing
- **pin 18** *(PD4)*: (output) line ***active part*** signal output
- **pin 20** *(PD6)*: (output) ***vertical sync*** output
- **pin 8** *(PB7)*: (output) ***bar pattern*** output

HD44780-compatible character display parallel port:
- **pin 35** *(PA5)*: (output) ***HD44780 E*** (operation enable)
- **pin 34** *(PA6)*: (output) ***HD44780 R/W*** (read/write select)
- **pin 33** *(PA7)*: (output) ***HD44780 RS*** (command/data select)
- **pin 37** *(PA3)*: (input/output) ***HD44780 D4*** (data bit 4)
- **pin 38** *(PA2)*: (input/output) ***HD44780 D5*** (data bit 5)
- **pin 39** *(PA1)*: (input/output) ***HD44780 D6*** (data bit 6)
- **pin 40** *(PA0)*: (input/output) ***HD44780 D7*** (data bit 7)

I2C displays serial port:
- **pin 23** *(PC1)*: (input/output) ***I2C data***
- **pin 22** *(PC0)*: (output) ***I2C clock***

UART displays serial port:
- **pin 14** *(PD0)*: (input) ***UART data RX*** from display
- **pin 15** *(PD1)*: (output) ***UART data TX*** to display
- **pin 17** *(PD3)*: (input) ***Busy*** signal from display

</details>

## Usage

Power supply connects to the default (center-positive) DC barrel jack on the PCB. The board is equiped with reverse-polarity protection. Supplied voltage should in range of **8...20 Volts**. Onboard linear volate regulator provides 5V for MCU and connected character display.

On power-on MCU starts generating video signal for selected video standard. In parallel MCUs tries to communicate with character display through *HD44780* connector.

> [!NOTE]
> Only one of the display output connectors should be used at any time.

Video standard is selected through "MODE" switches.

## Current state
- Two-switch video selector works
- PAL and NTSC frame generation works ok
- VGA, EGA and CGA frame generation works ok
- Bar pattern for PAL/NTSC/CGA/EGA/VGA has horizontal jitter due to AVR interrupt jitter
- HD44780 parallel (4-bit) driver works fine

## Plans
- Update I2C driver to working state
- Write driver for character displays with I2C bus
- Write driver for UART Futaba character displays
- Write driver for UART terminal (?)

## License
Program copyright 2022-2024.

This program is free software.
Licensed under the Apache License, Version 2.0 (the "License");