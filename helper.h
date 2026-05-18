/* 
 * File:   helper.h
 * Author: Sneha
 *
 */

#ifndef HELPER_H
#define	HELPER_H

#define DASH_BOARD              0x01
#define MENU_FLAG               0x02
#define PASSWORD_SCREEN         0x03

#define VIEW_LOG                0x04
#define CLEAR_LOG               0x05
#define DOWNLOAD_LOG            0x06
#define CHANGE_PASSWORD         0x07
#define SET_TIME                0x08

#define RESET_PASSWORD          0x12
#define RESET_MENU              0x13

#define RESET_VIEW_LOG          0x14
#define RESET_CLEAR_LOG         0x15
#define RESET_DOWNLOAD_LOG      0x16
#define RESET_CHANGE_PASSWORD   0x17
#define RESET_SET_TIME          0x18


#define RESET_NOTHING           0x00

#define RETURN_SUCCESS          0xA0
#define RETURN_BACK             0xA1


void display_dashboard(unsigned char event[], unsigned char speed);
void log_event(unsigned char event[], unsigned char speed);

unsigned char check_password(unsigned char key, unsigned char reset_flag);
unsigned char menu_screen(unsigned char reset_flag, unsigned char key);

unsigned char view_log(unsigned char reset_flag, unsigned char key);
void clear_log(unsigned char reset_flag);
void download_log(void);
void set_time(unsigned char reset_flag, unsigned char key);
unsigned char change_password(unsigned char reset_flag, unsigned char key);


#endif	/* HELPER_H */

