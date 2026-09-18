# Smart Door Lock System using LPC1768 (ARM Cortex-M3)

## Overview
This project implements a smart door lock system using the LPC1768 microcontroller. It uses UART for password input, a 16x2 LCD for user interaction, and an LED for system status. The system is implemented in a compact Keil uVision 5 RTX-style RTOS design.

## Features
- Password-based door lock logic
- UART serial input from a PC/terminal
- 16x2 LCD display
- LED indication for access and lock state
- Retry limit after incorrect password attempts
- Reset flow using the word "forgot"
- Keil RTX task-based design for LPC1768

## Hardware Used
- LPC1768 development board
- 16x2 LCD module
- LED
- USB-to-UART interface
- Power supply

## Software Used
- Keil uVision 5
- LPC17xx CMSIS drivers
- RTX RTOS library
- UART and LCD peripheral drivers

## Working Principle
1. The system displays `ENTER PASSWORD` on the LCD.
2. The user enters a 4-digit password through UART.
3. The entered digits are shown as `*` on the LCD.
4. The firmware compares the entered password with the predefined password `1234`.
5. If the password matches:
   - `ACCESS GRANTED` is displayed
   - LED is turned ON
6. If the password is wrong:
   - `WRONG PASSWORD` is displayed
   - the attempt counter increases
7. After 3 failed attempts:
   - the system enters `SYSTEM LOCKED`
   - the LED blinks continuously
8. The word `forgot` resets the access state and allows re-entry.

## File Structure
- `Code/Rtos_lock_sys.c` - main RTOS-based firmware
- `Code/UART0.c` - UART0 driver
- `Code/UART0.h` - UART0 declarations
- `Code/Lcd.c` - LCD driver
- `Code/Lcd.h` - LCD declarations

## Build Notes
This project is intended for Keil uVision 5 with:
- LPC1768 device support
- CMSIS core files
- RTX RTOS library enabled
- UART and LCD drivers linked into the project

## Important
This is a learning and embedded project for hardware interfacing and RTOS task design. It is suitable for academic and mini-project use.

## Author
Aditya Vijay Patil
