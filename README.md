# Vehicle Black Box System using PIC16F877A

## Description

This project implements a **Vehicle Black Box System** inspired by the flight data recorder concept,
built around the **PIC16F877A microcontroller**. The system continuously monitors vehicle speed via ADC,
logs gear change and clutch events with timestamps into an **external EEPROM (24C02)**, and displays
live data on a **16x2 LCD**. Logged data can be viewed on LCD, downloaded to a PC via UART, or cleared
through a password protected menu. Time is accurately maintained using a **DS1307 RTC module**.

---

## Features

- Real time clock display on dashboard (HH:MM:SS)
- Event logging — Clutch press, Gear Up, Gear Down with speed and timestamp
- Stores up to 10 events in external EEPROM (circular buffer, overwrites oldest)
- Password protected menu (default password: `1111` using SW4=1, SW5=0)
- 3 incorrect attempts trigger a 60 second lockout
- Long press detection using Timer2 ISR (no blocking delays)
- Switch debounce handled inside Timer2 ISR
- UART log download to PC terminal at 9600 baud
- Change password and set time options in menu

---

## Menu Options

| Option            | Description                              |
|-------------------|------------------------------------------|
| View Log          | Scroll through stored events on LCD      |
| Clear Log         | Erase all stored events from EEPROM      |
| Download Log      | Send all logs to PC via UART             |
| Set Time          | Adjust RTC time (HH:MM:SS)              |
| Change Password   | Set a new 4 digit password               |

---

## Hardware Components

| Component                  | Description                              |
|----------------------------|------------------------------------------|
| PIC16F877A                 | Main microcontroller                     |
| 16x2 Character LCD (CLCD)  | Display for dashboard and menu           |
| DS1307 RTC Module          | Real time clock over I2C                 |
| 24C02 External EEPROM      | Event log storage over I2C               |
| 6 Tactile Push Buttons     | SW1 to SW6 on PORTB (RB0-RB5)           |
| Potentiometer              | Simulates vehicle speed via ADC (RA0)    |
| UART to USB Module         | Log download to PC terminal              |
| 20MHz Crystal Oscillator   | System clock                             |

---

## Switch Functions

| Switch | Short Press                        | Long Press               |
|--------|------------------------------------|--------------------------|
| SW1    | Log clutch event                   | —                        |
| SW2    | Gear up                            | —                        |
| SW3    | Gear down                          | —                        |
| SW4    | Navigate up / Enter 1 in password  | Select menu item         |
| SW5    | Navigate down / Enter 0 in password| Return to dashboard      |
| SW6    | Select / Confirm                   | —                        |

---

## Project Structure

```
.
├── main.c            # Entry point — screen state machine and key routing
├── helper.c          # Display, logging, menu, password, set time functions
├── helper.h          # Header for helper.c
├── digital_keypad.c  # Key read with long press and debounce logic
├── digital_keypad.h  # Header for digital_keypad.c
├── isr.c             # Timer2 ISR — debounce, long press, return/wait timers
├── timers.c          # Timer2 initialization
├── timers.h          # Header for timers.c
├── ds1307.c          # DS1307 RTC driver
├── ds1307.h          # Header for ds1307.c
├── i2c.c             # I2C driver
├── i2c.h             # Header for i2c.c
├── eeprom.c          # 24C02 EEPROM read/write driver
├── eeprom.h          # Header for eeprom.c
├── adc.c             # ADC driver for speed reading
├── adc.h             # Header for adc.c
├── uart.c            # UART driver for log download
├── uart.h            # Header for uart.c
└── CLCD.c / CLCD.h   # 16x2 LCD driver
```

---

## How to Run

### Prerequisites
- MPLAB X IDE installed
- XC8 Compiler installed
- PICkit 3 or compatible programmer
- Serial terminal (RealTerm or PuTTY) for UART log download

### Steps

**1. Clone or download the project**

**2. Open in MPLAB X IDE**
- File → Open Project → select the project folder

**3. Build the project**
- Click Clean and Build
- Verify 0 errors in output window

**4. Flash to PIC16F877A**
- Connect PICkit 3 to the board
- Click Make and Program Device in MPLAB X

**5. Open serial terminal for UART**
- Baud rate : 9600
- Data bits : 8
- Parity    : None
- Stop bits : 1

---

## Operating the System

1. On power up the dashboard displays live time, current event and speed
2. Press **SW1** to log a clutch event
3. Press **SW2** to log gear up, **SW3** to log gear down
4. Press **SW4 or SW5** from dashboard to go to password entry screen
5. Enter 4 digit password using **SW4 (= 1)** and **SW5 (= 0)**, default is `1111`
6. After successful login the menu appears
7. Use **SW4 / SW5** to scroll through menu options
8. **Long press SW4** or press **SW6** to select the highlighted option
9. In **View Log** — SW4/SW5 to scroll, long press SW4/SW6 to go back to menu
10. In **Download Log** — logs are sent automatically to PC terminal via UART
11. In **Set Time** — SW4 to increment value, SW5 to switch between HH/MM/SS fields, long press SW4/SW6 to save
12. In **Change Password** — enter new password twice to confirm
13. **Long press SW5** from View Log or Set Time to return directly to dashboard
14. Menu auto returns to dashboard after 5 seconds of inactivity

---

## Key Implementation Details

- **Long press detection** — Timer2 ISR increments a hold counter every tick.
  When it reaches `LONG_PRESS_THRESHOLD`, `long_press` flag is set
- **Debounce** — Key must remain stable for `DEBOUNCE_THRESHOLD` consecutive
  ISR ticks before being accepted as a valid press
- **Buffer on press, return on release** — SW4 and SW5 are buffered on press
  and returned only on release to distinguish short press from long press.
  SW1, SW2, SW3, SW6 are returned immediately on press
- **Circular buffer** — EEPROM stores up to 10 events. On the 11th event
  the oldest entry is overwritten
- **Return timer** — `return_time` is decremented every 20000 ISR ticks.
  Screens auto exit to dashboard after 5 seconds of no key activity

---

## Author

Sneha Bujurke  
