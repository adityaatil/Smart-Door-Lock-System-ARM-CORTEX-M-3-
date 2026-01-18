# RTOS-Based Smart Door Lock System using LPC1768

## Overview
This project implements an **RTOS-based smart door lock system** using the **LPC1768 (ARM Cortex-M3)** microcontroller.  
The system provides secure access control using a password entered through **UART (serial input)**, displays status on a **16×2 LCD**, and indicates lock/unlock states using an **LED**.

The project demonstrates **embedded system design**, **peripheral interfacing**, and **task-based control logic** suitable for academic and learning purposes.

---

## Features
- Password-protected door locking system  
- UART-based password input (via PC or mobile serial terminal)  
- Password masking on LCD (`****`)  
- Maximum attempt limit with system lock  
- LED indication for:
  - Access granted
  - System locked
- Timer-based delays and RTOS-style task control  
- “Forgot password” reset mechanism  

---

## Hardware Requirements
- LPC1768 ARM Cortex-M3 Development Board  
- 16×2 LCD (4-bit mode)  
- LED  
- USB-to-Serial interface / UART terminal  
- Power supply  

---

## Software Requirements
- Keil µVision IDE  
- ARM Compiler  
- LPC17xx CMSIS libraries  
- Serial terminal 
---

## Block Diagram
The system consists of the following main blocks:
- LPC1768 Microcontroller (Central Control)
- UART Interface (Password Input)
- LCD Display (User Interface)
- Timer (Delay & RTOS timing control)
- Password Verification & Access Control (Software Task)
- LED Indicator (System Status)

---

## Working Principle
1. User enters a password via UART.
2. Input is masked and displayed on the LCD.
3. Password is verified by a software task running on the LPC1768.
4. If the password is correct:
   - Access is granted
   - LED blinks and then stays ON
5. If the password is wrong:
   - Attempt counter increments
   - System locks after maximum attempts
6. Timer controls delays and task timing.

---

## Applications
- Smart door locking systems  
- Embedded security systems  
- RTOS learning projects  
- Academic mini-projects  

---

## Author
**Aditya Vijay Patil**  
Department of Electrical and Electronics Engineering  

