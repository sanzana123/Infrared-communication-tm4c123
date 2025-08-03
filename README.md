# TM4C123 UART Command-Based LED Controller

## Description
This project implements a UART-based command interface to control RGB LEDs on the TM4C123 microcontroller. Users can send text commands over UART to toggle red, green, and blue LEDs and get feedback via UART communication.

---

## Features
- UART communication for command input and output
- Parses and processes user commands
- Controls red, green, and blue LEDs connected to GPIO port F
- Uses SysTick timer for periodic green LED blinking
- Basic command validation and feedback over UART

---

## Hardware
- TM4C123 microcontroller (Tiva C LaunchPad)
- Red LED: PF1
- Blue LED: PF2
- Green LED: PF3

---

## How to Build and Run

### Prerequisites
- TM4C123 development environment (e.g., Code Composer Studio, Keil, or similar)
- TM4C123 board with LEDs connected to port F

### Steps
1. Include the provided source files (`main.c`, `clock.h`, `uart0.h`, etc.) in your project.
2. Configure the system clock to 40 MHz.
3. Initialize UART0 for serial communication.
4. Initialize GPIO Port F pins for LED control.
5. Build and flash the firmware to your TM4C123 board.
6. Open a serial terminal (e.g., PuTTY, TeraTerm) at the configured baud rate.
7. Send commands like `red on`, `red off`, `set <num1> <num2>`, or `alert on/off`.
8. Observe LED state changes and UART feedback.

---

## Example Commands

- `red on`  — Turns the red LED on
- `red off` — Turns the red LED off
- `set 10 5` — Computes 10 - 5 and returns `pass` if result > 0 else `fail`
- `alert on` or `alert off` — Placeholder for alert command processing
