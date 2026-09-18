# Smart Door Lock System using LPC1768 (ARM Cortex-M3) with RTX RTOS

## Overview
This project implements a smart door lock system using the LPC1768 microcontroller. The system accepts a password from a UART terminal, displays the status on a 16x2 LCD, and indicates the door state using an LED. The design is implemented as an RTOS-based embedded application using Keil RTX, where multiple tasks run concurrently.

## RTOS Design
The firmware is structured as a simple RTOS application for the LPC1768:
- `DoorLock_Task`: handles UART password input, validation, lock logic, and reset flow
- `LED_Task`: controls LED blinking and lock indication
- Main task: initializes peripherals and starts the RTX scheduler

This architecture makes the application modular and suitable for task-based embedded design.

## Features
- Password-protected access control
- UART-based password input
- LCD display for messages and password masking
- LED indication for access granted and system lock
- Retry counter for wrong password attempts
- Lockout after maximum failed attempts
- Reset mechanism using the word `forgot`
- Keil RTX task-based implementation

## Hardware Requirements
- LPC1768 ARM Cortex-M3 development board
- 16x2 LCD module
- LED
- USB-to-UART serial interface
- Power supply

## Software Requirements
- Keil uVision 5
- ARM CMSIS device files for LPC1768
- Keil RTX RTOS library
- LPC17xx peripheral support files

## Block Diagram Concept
- LPC1768 microcontroller: control unit
- UART interface: serial password input
- LCD: user interface
- LED: access/lock indication
- Firmware logic: password verification and lock management

## Working Principle
1. The system displays `ENTER PASSWORD` on the LCD.
2. The user enters a 4-digit password through UART.
3. The entered digits are masked as `*` on the LCD.
4. The password is compared with the predefined value `1234`.
5. If the password is correct:
   - `ACCESS GRANTED` is shown
   - the LED is turned ON
6. If the password is wrong:
   - `WRONG PASSWORD` is shown
   - the attempt counter increases
7. After 3 failed attempts:
   - the system enters `SYSTEM LOCKED`
   - the LED blinks continuously
8. Entering `forgot` resets the state and prompts the user to enter a new password sequence.

## RTOS Task Flow
- UART task waits for received characters
- Password validation task checks the entered digits
- State logic decides whether access is granted or denied
- LED task handles the visual locking and access indication

## File Structure
- `Code/Rtos_lock_sys.c` - main RTX-based application
- `Code/UART0.c` - UART0 driver
- `Code/UART0.h` - UART0 declarations
- `Code/Lcd.c` - LCD driver
- `Code/Lcd.h` - LCD declarations
- `block diagram/` - project block diagram image
- `READme.md` - project documentation

## Build Notes
This project should be built in Keil uVision 5 with:
- LPC1768 device family selected
- CMSIS Core included
- RTX RTOS enabled
- UART and LCD source files added to the project
- `RTL.h` included in the main firmware

## Important Note
This implementation is intended for embedded learning and academic use. It demonstrates RTOS task design, GPIO interfacing, UART communication, and display control on the LPC1768 board.

## Author
Aditya Vijay Patil
