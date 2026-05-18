/*
 * File:   main.c
 * Author: Sneha
 *
 */
/*
    *PASSWORD = SW4 SW4 SW4 SW4 
    please hold swith for at least 1 ms to smooth working of switches
 
    Car Black Box System using PIC16F877A
    This project implements a Car Black Box System , designed to log and store car events in real time. 
    Built around the PIC16F877A microcontroller, the system continuously monitors car speed (via ADC), 
    logs gear change and clutch events with timestamps, and stores them in an external EEPROM. 
    All logged data can be viewed on a 16x2 LCD, downloaded via UART to a PC terminal, or cleared through a password-protected menu system. 
    Time is maintained accurately using a DS1307 RTC module.
    
    At 1st dashboard displays live time, event and speed
    Press SW1 to log a clutch event
    Press SW2 to log gear up, SW3 to log gear down
    Press SW4 or SW5 from dashboard to enter password screen
    Enter password using SW4 and SW5  default is 1111
    After login, use SW4/SW5 to navigate menu, long press SW4 or SW6 to select
    In View Log ? SW4/SW5 to scroll through events
    In Download Log ? logs are sent automatically via UART to PC terminal
    In Set Time ? SW4 to increment, SW5 to switch between HH/MM/SS, long press SW4/SW6 to save
    In Change Password ? enter new 4-digit password twice to confirm
    Long press SW5 from View Log or Set Time to return directly to dashboard
*/

#include <xc.h>
#include <string.h>
#include "CLCD.h"
#include "ds1307.h"
#include "i2c.h"
#include "digital_keypad.h"
#include "helper.h"
#include "adc.h"
#include "timers.h"
#include "uart.h"
#include "eeprom.h"

//turn of WDT
#pragma config WDTE = OFF

unsigned char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4"};
unsigned char newSec, newMin, newHr;

static void init_config() {
    init_i2c(100000); //100k
    init_ds1307();
    init_digital_keypad();
    init_clcd();
    init_adc();
    init_timer2();
    init_uart(9600);

    puts("UART TEST CODE\n");

    PEIE = 1;
    GIE = 1;

}

void main(void) {

    init_config();

    unsigned char screen_flag = DASH_BOARD;
    unsigned char reset_flag = RESET_NOTHING;

    unsigned char speed = 0;
    unsigned char event[3] = "ON";
    unsigned char key;

    unsigned char gr = 0;
    unsigned char delay = 0;

    unsigned char menu_pos;
    unsigned char view_return;
    unsigned char pass_return;

    //store the password here as it is required to change password
    ext_eeprom_24c02_str_write(0x00, "1111");

    while (1) {
        //read speed
        speed = (unsigned char) (read_adc(CHANNEL0) / 10);
        if (speed > 99)
            speed = 99;

        //detect e vents on key press
        key = read_digital_keypad(STATE);
        for(unsigned short wait = 5000;wait--;);
        
        if (key == SW1) {
            strcpy(event, " C");
            //store event , speed , time in external EEPROM
            log_event(event, speed);
        } else if (key == SW2) {
            if (gr < 5)
                gr++;
            strcpy(event, gear[gr]);
            log_event(event, speed);
        } else if (key == SW3) {
            if (gr > 0)
                gr--;
            strcpy(event, gear[gr]);
            log_event(event, speed);
        } else if ((key == SW4 || key == SW5) && screen_flag == DASH_BOARD) {
            screen_flag = PASSWORD_SCREEN;
            reset_flag = RESET_PASSWORD;
            clear_screen();
            clcd_print("ENTER PASSWORD", LINE1(0));
            clcd_write(LINE2(4), INS_MODE);
            clcd_write(DISPLAY_ON_CUR_ON, 0);
            TMR2ON = 1;
        }

        if ((key == L_SW4 || key == SW6) && (screen_flag == MENU_FLAG)) {
            //if menu screen, long press of the switch update screen

            switch (menu_pos) {
                case 0:
                    clear_screen();
                    screen_flag = VIEW_LOG;
                    reset_flag = RESET_VIEW_LOG;
                    break;
                case 1:
                    clear_screen();
                    screen_flag = CLEAR_LOG;
                    reset_flag = RESET_CLEAR_LOG;
                    break;
                case 2:
                    clear_screen();
                    screen_flag = DOWNLOAD_LOG;
                    reset_flag = RESET_DOWNLOAD_LOG;
                    break;
                case 3:
                    clear_screen();
                    screen_flag = SET_TIME;
                    reset_flag = RESET_SET_TIME;
                    break;
                case 4:
                    clear_screen();
                    screen_flag = CHANGE_PASSWORD;
                    reset_flag = RESET_CHANGE_PASSWORD;
                    break;
            }
        } else if ((key == L_SW4 || key == SW6) && (screen_flag == VIEW_LOG)) {
            screen_flag = MENU_FLAG;
            reset_flag = RESET_MENU;
            clcd_write(DISPLAY_ON_CUR_OFF, 0);
        } else if ((key == L_SW4 || key == SW6) && screen_flag == SET_TIME) {
            clcd_write(DISPLAY_ON_CUR_OFF, 0);
            clcd_print("  Time changed  ", LINE1(0));
            clcd_print("  Successfully  ", LINE2(0));
            __delay_ms(1000);
            clear_screen();
            screen_flag = DASH_BOARD;
            //            reset_flag = RESET_MENU;
            write_ds1307(SEC_ADDR, (newSec / 10 << 4) | (newSec % 10));
            write_ds1307(MIN_ADDR, (newMin / 10 << 4) | (newMin % 10));
            write_ds1307(HOUR_ADDR, (newHr / 10 << 4) | (newHr % 10));
        }
        else if ((key == L_SW5) && (screen_flag == VIEW_LOG || screen_flag == SET_TIME)) {
            screen_flag = DASH_BOARD;
            clear_screen();
            clcd_write(DISPLAY_ON_CUR_OFF, 0);
        }

        //based on screen_flag display particular screen
        switch (screen_flag) {
            case DASH_BOARD:
                display_dashboard(event, speed);
                break;
            case PASSWORD_SCREEN:
                switch (check_password(key, reset_flag)) {
                    case RETURN_BACK:
                        screen_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISPLAY_ON_CUR_OFF, 0);
                        TMR2ON = 0;
                        break;
                    case RETURN_SUCCESS:
                        screen_flag = MENU_FLAG;
                        reset_flag = RESET_MENU;
                        clear_screen();
                        clcd_write(DISPLAY_ON_CUR_OFF, 0);
                        TMR2ON = 1;
                        break;
                }
                break;
            case MENU_FLAG:
                menu_pos = menu_screen(reset_flag, key);
                if (menu_pos == RETURN_BACK) {
                    screen_flag = DASH_BOARD;
                    clear_screen();
                    clcd_write(DISPLAY_ON_CUR_OFF, 0);
                    TMR2ON = 1;
                }
                break;

            case VIEW_LOG:
                view_return = view_log(reset_flag, key);
                if (view_return == RETURN_BACK) {
                    screen_flag = MENU_FLAG;
                    reset_flag = RESET_MENU;
                    clear_screen();
                }
                break;
            case CLEAR_LOG:
                clear_log(reset_flag);
                screen_flag = MENU_FLAG;
                reset_flag = RESET_MENU;
                clear_screen();
                break;
            case DOWNLOAD_LOG:
                download_log();
                screen_flag = MENU_FLAG;
                reset_flag = RESET_MENU;
                clear_screen();
                break;
            case CHANGE_PASSWORD:
                switch (change_password(reset_flag, key)) {
                    case RETURN_BACK:
                        screen_flag = MENU_FLAG;
                        reset_flag = RESET_MENU;
                        clear_screen();
                        clcd_write(DISPLAY_ON_CUR_OFF, 0);
                        TMR2ON = 1;
                        break;
                    case RETURN_SUCCESS:
                        screen_flag = MENU_FLAG;
                        reset_flag = RESET_MENU;
                        clear_screen();
                        clcd_write(DISPLAY_ON_CUR_OFF, 0);
                        TMR2ON = 1;
                        break;
                }
                break;
            case SET_TIME:
                set_time(reset_flag, key);
                break;
        }
        reset_flag = RESET_NOTHING;
    }

    return;
} 