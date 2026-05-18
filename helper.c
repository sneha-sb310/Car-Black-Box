/*
 * File:   helper.c
 * Author: Sneha
 *
 */


#include <xc.h>
#include <string.h>

#include "ds1307.h"
#include "CLCD.h"
#include "helper.h"
#include "digital_keypad.h"
#include "timers.h"
#include "eeprom.h"
#include "uart.h"

unsigned char clock_reg[3];
char time[7];
unsigned char log[11]; //time = 6, speed 2, event 2 
unsigned char pos = 0;
signed char log_count = -1;

unsigned char return_time;
unsigned char wait_time;
extern unsigned char newSec, newMin, newHr;

unsigned char saved_log[11];

unsigned char *menu[] = {"View Log", "Clear Log", "Download Log", "Set Time", "Change Password"};

static void get_time() {

    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';

    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';

    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';

    time[6] = '\0';

}

static void display_time() {

    get_time();

    clcd_putch(time[0], LINE2(0));
    clcd_putch(time[1], LINE2(1));
    clcd_putch(':', LINE2(2));

    clcd_putch(time[2], LINE2(3));
    clcd_putch(time[3], LINE2(4));
    clcd_putch(':', LINE2(5));

    clcd_putch(time[4], LINE2(6));
    clcd_putch(time[5], LINE2(7));

}

void display_dashboard(unsigned char event[], unsigned char speed) {
    clcd_print("  TIME    EV  SP", LINE1(0));
    display_time();
    clcd_print(event, LINE2(10));

    clcd_putch((speed / 10 + '0'), LINE2(14)); //65 -> 6
    clcd_putch((speed % 10 + '0'), LINE2(15)); //65 -> 5
}

void log_event(unsigned char event[], unsigned char speed) {
    unsigned char addr;

    addr = pos * 10 + 5; //0 , 10 .......... 90

    //combine time speed event into single string
    get_time();
    strncpy(log, time, 6); //hhmmss
    strncpy(&log[6], event, 2);

    //for storing speed
    log[8] = speed / 10 + '0';
    log[9] = speed % 10 + '0';
    log[10] = '\0';

    ext_eeprom_24c02_str_write(addr, log); //pass addr and string with content

    if (log_count < 9)
        log_count++;

    pos++;
    if (pos == 10) //if 11th event occur overwrite with oldest event
        pos = 0;
}

unsigned char check_password(unsigned char key, unsigned char reset_flag) {
    static unsigned char user_pass[4], saved_pass[4];
    static unsigned char attempts, i;

    if (reset_flag == RESET_PASSWORD) {
        attempts = 3;
        i = 0;

        user_pass[0] = '\0';
        user_pass[1] = '\0';
        user_pass[2] = '\0';
        user_pass[3] = '\0';

        return_time = 5;

        key = 0;
    }

    //read password from user check for timeout return to DB
    if (key == SW4 && i < 4) {
        user_pass[i] = '1';
        i++;
        clcd_putch('*', LINE2(i + 4));
        return_time = 5;
    } else if (key == SW5 && i < 4) {
        user_pass[i] = '0';
        i++;
        clcd_putch('*', LINE2(i + 4));
        return_time = 5;
    }

    if (return_time == 0) {
        return RETURN_BACK;
    }

    //compare stored pass and user pass if true return success
    if (i == 4) {
        
        for(unsigned char j = 0; j < 4; j++)
        {
            saved_pass[j] = ext_eeprom_24C02_read(j);
        }
        
        if ((strncmp(user_pass, saved_pass, 4)) == 0) {
            clear_screen();
            clcd_write(DISPLAY_ON_CUR_OFF, 0);
            clcd_print("LOGIN SUCCESS", LINE1(0));
            __delay_ms(1000);
            TMR2ON = 0;
            return_time = 5;
            return RETURN_SUCCESS;
        } else {
            attempts--;
            if (attempts == 0) //if attempts are over lock the screen for 60 secs
            {
                clear_screen();
                clcd_write(DISPLAY_ON_CUR_OFF, 0);
                clcd_print("You are blocked", LINE1(0));
                clcd_print("wait for ", LINE2(0));
                wait_time = 60;
                while (wait_time != 0) {
                    clcd_putch((wait_time / 10 + '0'), LINE2(10));
                    clcd_putch((wait_time % 10 + '0'), LINE2(11));
                }
                clcd_print("secs", LINE2(13));
                attempts = 3;
            } else {
                clear_screen();
                clcd_write(DISPLAY_ON_CUR_OFF, 0);
                clcd_print("WRONG PASSWORD", LINE1(0));
                clcd_putch(attempts + '0', LINE2(0));
                clcd_print(" Attempts left", LINE2(2));
                __delay_ms(1500);
            }
            clear_screen();
            clcd_print("ENTER PASSWORD", LINE1(0));
            clcd_write(LINE2(4), INS_MODE);
            clcd_write(DISPLAY_ON_CUR_ON, 0);
            i = 0;
            return_time = 5;
        }
    }
}

unsigned char menu_screen(unsigned char reset_flag, unsigned char key) {
    static unsigned char menu_pos;

    if (reset_flag == RESET_MENU) {
        clear_screen();
        return_time = 5;
        menu_pos = 0;
    }

    //based on key scroll up and down
    if (key == SW5 && menu_pos < 4) {
        menu_pos++;
        clear_screen();
        return_time = 5;
    } else if (key == SW4 && menu_pos > 0) {
        menu_pos--;
        clear_screen();
        return_time = 5;
    }

    if (return_time == 0) {
        return RETURN_BACK;
    }

    if (menu_pos < 4) {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos], LINE1(1));
        clcd_print(menu[menu_pos + 1], LINE2(1));
    } else {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos - 1], LINE1(1));
        clcd_print(menu[menu_pos], LINE2(1));
    }


    //return the pos of menu selected
    return menu_pos;
}

void get_log(unsigned char log_pos) {
    unsigned char addr;

    addr = log_pos * 10 + 5;

    for (unsigned int i = 0; i < 10; i++) {
        saved_log[i] = ext_eeprom_24C02_read(addr + i);
    }
    saved_log[10] = '\0';
}

void display_log(unsigned char log_pos) {

    get_log(log_pos);

    clcd_putch((log_pos % 10) + '0', LINE2(0));

    clcd_putch(saved_log[0], LINE2(2));
    clcd_putch(saved_log[1], LINE2(3));

    clcd_putch(':', LINE2(4));

    clcd_putch(saved_log[2], LINE2(5));
    clcd_putch(saved_log[3], LINE2(6));

    clcd_putch(':', LINE2(7));

    clcd_putch(saved_log[4], LINE2(8));
    clcd_putch(saved_log[5], LINE2(9));

    clcd_putch(saved_log[6], LINE2(11));
    clcd_putch(saved_log[7], LINE2(12));

    clcd_putch(saved_log[8], LINE2(14));
    clcd_putch(saved_log[9], LINE2(15));

}

unsigned char view_log(unsigned char reset_flag, unsigned char key) {

    static unsigned char log_pos;

    if (reset_flag == RESET_VIEW_LOG) {
        clear_screen();
        return_time = 5;
        if (log_count == -1) {
            clcd_print("     NO LOGS    ", LINE1(0));
            clcd_print("  ARE AVAILABLE ", LINE2(0));
            __delay_ms(1500);
            return RETURN_BACK;
        } else {
            clcd_print("#  TIME    E  Sp", LINE1(0));
            log_pos = 0;
        }
    }


    //based on key scroll up and down
    if (key == SW5) {
        log_pos++;
        if (log_pos > log_count)
            log_pos = 0;
        return_time = 5;
    } else if (key == SW4) {
        if (log_pos == 0)
            log_pos = log_count;
        else
            log_pos--;
        return_time = 5;
    }

    display_log(log_pos);

    //    if (return_time == 0)
    //        return RETURN_BACK;

    return RETURN_SUCCESS;

}

void clear_log(unsigned char reset_flag) {
    if (reset_flag == RESET_CLEAR_LOG) {
        log_count = -1;
        pos = 0;
        clcd_print("  LOGS CLEARED  ", LINE1(0));
        clcd_print("  SUCCESSFULLY  ", LINE2(0));
        __delay_ms(1000);
    }
}

void display_uart(unsigned char log_pos) {

    putchar('#');
    putchar((log_pos) % 10 + '0');
    putchar(' ');

    putchar(saved_log[0]);
    putchar(saved_log[1]);

    putchar(':');

    putchar(saved_log[2]);
    putchar(saved_log[3]);

    putchar(':');

    putchar(saved_log[4]);
    putchar(saved_log[5]);

    putchar(' ');

    putchar(saved_log[6]);
    putchar(saved_log[7]);

    putchar(' ');

    putchar(saved_log[8]);
    putchar(saved_log[9]);

    putchar('\r');
    putchar('\n');
}

void download_log() {
    unsigned char i = 0;
    if (log_count == -1) {
        clcd_print("   NO LOGS   ", LINE1(0));
        clcd_print("  AVAILABLE  ", LINE2(0));
    } else {
        while (i <= log_count) {
            get_log(i);
            display_uart(i);
            i++;
        }
        clcd_print(" LOG DOWNLOADED ", LINE1(0));
        clcd_print("  SUCCESSFULLY  ", LINE2(0));
    }
    __delay_ms(1500);
}

void set_time(unsigned char reset_flag, unsigned char key) {
    static unsigned char newTime[7];
    static signed char i, j;

    static int blink;
    static int blink_delay;


    if (reset_flag == RESET_SET_TIME) {
        clear_screen();
        return_time = 5;

        clcd_print("Time (HH:MM:SS)", LINE1(0));

        get_time();
        strcpy(newTime, time);

        newSec = (newTime[4] - '0') * 10 + (newTime[5] - '0');
        newMin = (newTime[2] - '0') * 10 + (newTime[3] - '0');
        newHr = (newTime[0] - '0') * 10 + (newTime[1] - '0');

        i = 2; // 0 -> sec 1 -> min 2 -> hr
        j = 0; //hh:mm:ss to display in this order

    }
    if (key == SW4) {
        if (i == 2) {
            if (++newHr > 23)
                newHr = 0;
        } else if (i == 1) {
            if (++newMin > 59)
                newMin = 0;
        } else {
            if (++newSec > 59)
                newSec = 0;
        }
        return_time = 5;
    } else if (key == SW5) {
        if (--i < 0)
            i = 2;

        if (i == 2)
            j = 0;
        else if (i == 1)
            j = 3;
        else
            j = 6;
    }

    //    clcd_write(LINE2(j), 0);
    //    clcd_write(LINE2(j+1), 0);
    //    clcd_write(DISPLAY_ON_CUR_ON, 0);

    if (++blink_delay > 2) // adjust speed
    {
        blink_delay = 0;
        blink = !blink;
    }

    if (!blink) {

        clcd_putch(newHr / 10 + '0', LINE2(0));
        clcd_putch(newHr % 10 + '0', LINE2(1));
        clcd_putch(':', LINE2(2));

        clcd_putch(newMin / 10 + '0', LINE2(3));
        clcd_putch(newMin % 10 + '0', LINE2(4));
        clcd_putch(':', LINE2(5));

        clcd_putch(newSec / 10 + '0', LINE2(6));
        clcd_putch(newSec % 10 + '0', LINE2(7));
    } else {

        clcd_putch(0x5F, LINE2(j));
        clcd_putch(0x5F, LINE2(j + 1));
    }
}

unsigned char change_password(unsigned char reset_flag, unsigned char key) {
    unsigned char pass[4];
    static unsigned char password_1[4];
    static unsigned char password_2[4];
    static unsigned char i, j;

    if (reset_flag == RESET_CHANGE_PASSWORD) {
        clear_screen();
        clcd_print("ENTER NEW PASSWD", LINE1(0));
        clcd_write(LINE2(4), INS_MODE);
        clcd_write(DISPLAY_ON_CUR_ON, 0); 
        i = 0;
        j = 0;
        return_time = 5;

        password_1[0] = '\0';
        password_1[1] = '\0';
        password_1[2] = '\0';
        password_1[3] = '\0';
        
        password_2[0] = '\0';
        password_2[1] = '\0';
        password_2[2] = '\0';
        password_2[3] = '\0';
        
        key = 0;
    }

    if (key == SW4) {
        if (i < 4) {
            password_1[i] = '1';
            i++;
            clcd_putch('*', LINE2(i + 4));
        } else {
            password_2[j] = '1';
            j++;
            clcd_putch('*', LINE2(j + 4));
        }
        return_time = 5;
    }
    else if (key == SW5) {
        if (i < 4) {
            password_1[i] = '0';
            i++;
            clcd_putch('*', LINE2(i + 4));
        } else {
            password_2[j] = '0';
            j++;
            clcd_putch('*', LINE2(j + 4));
        }
        return_time = 5;
    }

    if (i == 4) {
        i++;
        clear_screen();
        clcd_print("RE-ENTER NEW PWD", LINE1(0));
        clcd_write(LINE2(4), INS_MODE);
        clcd_write(DISPLAY_ON_CUR_ON, 0);
    } else if (j == 4) 
    {
        if (!strncmp(password_1, password_2, 4)) {
            clear_screen();
            for (unsigned char i = 0; i < 4; i++) {
                ext_eeprom_24c02_byte_write(i, password_1[i]);
            }
            clcd_print("PASSWORD CHANGED", LINE1(0));
            clcd_print("  SUCCESSFULLY  ", LINE2(0));
            __delay_ms(1000);
            return RETURN_SUCCESS;
        }
        else{
            i = 0;
            j = 0;
            clcd_print("PASSWRD MISMATCH", LINE1(0));
            clcd_print("    TRY AGAIN   ", LINE2(0));
            __delay_ms(1000);
            clear_screen();
    return RETURN_BACK;
        }
    }
}
