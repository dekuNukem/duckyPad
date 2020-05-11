#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx_hal.h"

#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT    0x81
#define KEY_LEFT_ALT    0x82
#define KEY_LEFT_GUI    0x83
#define KEY_RIGHT_CTRL    0x84
#define KEY_RIGHT_SHIFT   0x85
#define KEY_RIGHT_ALT   0x86
#define KEY_RIGHT_GUI   0x87

#define KEY_RETURN (0x28+0x88)
#define KEY_ESC (0x29+0x88)
#define KEY_BACKSPACE (0x2A+0x88)
#define KEY_TAB (0x2B+0x88)
#define KEY_CAPS_LOCK (0x39+0x88)
#define KEY_F1 (0x3A+0x88)
#define KEY_F2 (0x3B+0x88)
#define KEY_F3 (0x3C+0x88)
#define KEY_F4 (0x3D+0x88)
#define KEY_F5 ((0x3E)+0x88)
#define KEY_F6 (0x3F+0x88)
#define KEY_F7 (0x40+0x88)
#define KEY_F8 (0x41+0x88)
#define KEY_F9 (0x42+0x88)
#define KEY_F10 (0x43+0x88)
#define KEY_F11 (0x44+0x88)
#define KEY_F12 (0x45+0x88)
#define KEY_PRINT_SCREEN (0x46+0x88)
#define KEY_SCROLL_LOCK (0x47+0x88)
#define KEY_PAUSE (0x48+0x88)
#define KEY_INSERT (0x49+0x88)
#define KEY_HOME (0x4A+0x88)
#define KEY_PAGE_UP (0x4B+0x88)
#define KEY_DELETE (0x4C+0x88)
#define KEY_END (0x4D+0x88)
#define KEY_PAGE_DOWN ((0x4E)+0x88)
#define KEY_RIGHT_ARROW (0x4F+0x88)
#define KEY_LEFT_ARROW (0x50+0x88)
#define KEY_DOWN_ARROW (0x51+0x88)
#define KEY_UP_ARROW (0x52+0x88)

#define KEY_VOL_UP 0xff
#define KEY_VOL_DOWN 0xfe
#define KEY_VOL_MUTE 0xfd

#define KB_BUF_SIZE 5

void kb_test(void);
void kb_print(char* msg, uint16_t chardelay);
void keyboard_release(uint8_t k);
void keyboard_press(uint8_t k, uint8_t use_mod);
void keyboard_release_all(void);

#ifdef __cplusplus
}
#endif

#endif


