#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"
#include "usbd_customhid.h"

#define KEY_LEFT_CTRL  0x01
#define KEY_LEFT_SHIFT 0x02
#define KEY_LEFT_ALT   0x04
#define KEY_LEFT_GUI  0x08
#define KEY_RIGHT_CTRL  0x10
#define KEY_RIGHT_SHIFT 0x20
#define KEY_RIGHT_ALT   0x40
#define KEY_RIGHT_GUI  0x80

#define KEY_RETURN 0X28
#define KEY_ESC 0X29
#define KEY_BACKSPACE 0X2A
#define KEY_TAB 0X2B
#define KEY_CAPS_LOCK 0X39
#define KEY_PRINT_SCREEN 0X46
#define KEY_SCROLL_LOCK 0X47
#define KEY_PAUSE 0X48
#define KEY_INSERT 0X49
#define KEY_HOME 0X4A
#define KEY_PAGE_UP 0X4B
#define KEY_DELETE 0X4C
#define KEY_END 0X4D
#define KEY_PAGE_DOWN 0X4E
#define KEY_RIGHT_ARROW 0X4F
#define KEY_LEFT_ARROW 0X50
#define KEY_DOWN_ARROW 0X51
#define KEY_UP_ARROW 0X52

#define KEY_NUMLOCK 0x53 // Keyboard Num Lock and Clear
#define KEY_KPSLASH 0x54 // Keypad /
#define KEY_KPASTERISK 0x55 // Keypad *
#define KEY_KPMINUS 0x56 // Keypad -
#define KEY_KPPLUS 0x57 // Keypad +
#define KEY_KPENTER 0x58 // Keypad ENTER
#define KEY_KP1 0x59 // Keypad 1 and End
#define KEY_KP2 0x5a // Keypad 2 and Down Arrow
#define KEY_KP3 0x5b // Keypad 3 and PageDn
#define KEY_KP4 0x5c // Keypad 4 and Left Arrow
#define KEY_KP5 0x5d // Keypad 5
#define KEY_KP6 0x5e // Keypad 6 and Right Arrow
#define KEY_KP7 0x5f // Keypad 7 and Home
#define KEY_KP8 0x60 // Keypad 8 and Up Arrow
#define KEY_KP9 0x61 // Keypad 9 and Page Up
#define KEY_KP0 0x62 // Keypad 0 and Insert
#define KEY_KPDOT 0x63 // Keypad . and Delete
#define KEY_KPEQUAL 0x67 // Keypad =

#define KEY_MENU 0x65
#define KEY_POWER 0x66 // Keyboard Power

#define KEY_MK_VOLDOWN 0x80
#define KEY_MK_VOLUP 0x40
#define KEY_MK_VOLMUTE 0x20
#define KEY_MK_PLAYPAUSE 0x10
#define KEY_MK_STOP 0x4
#define KEY_MK_PREV 0x2
#define KEY_MK_NEXT 0x1

#define HID_TX_BUF_SIZE (CUSTOM_HID_EPIN_SIZE+1)
#define MOUSE_BUF_SIZE 5
#define MEDIA_KEY_BUF_SIZE 2
#define KB_BUF_SIZE 8

#define KB_LAYOUT_WQERTY 0
#define KB_LAYOUT_AZERTY_FR 1
#define KB_LAYOUT_AZERTY_BE 2
#define KB_LAYOUT_DVORAK 3

#define KEY_TYPE_UNKNOWN 0
#define KEY_TYPE_CHAR 1
#define KEY_TYPE_MODIFIER 2
#define KEY_TYPE_SPECIAL 3
#define KEY_TYPE_MEDIA 4
#define KEY_TYPE_DEAD_GRAVE_ACCENT 5
#define KEY_TYPE_DEAD_ACUTE_ACCENT 6
#define KEY_TYPE_DEAD_CIRCUMFLEX 7
#define KEY_TYPE_DEAD_TILDE 8
#define KEY_TYPE_DEAD_DIAERESIS 9
#define KEY_TYPE_DEAD_CEDILLA 10
#define KEY_TYPE_MOUSE_BUTTON 11
#define KEY_TYPE_MOUSE_WHEEL 12
#define KEY_TYPE_MOUSE_MOVEMENT 13

#define KEY_KATAKANAHIRAGANA 0x88
// #define KEY_YEN 0x89
#define KEY_HENKAN 0x8a
#define KEY_MUHENKAN 0x8b

// #define KEY_HANGEUL 0x90 // korea
// #define KEY_HANJA 0x91 // korea
#define KEY_KATAKANA 0x92
#define KEY_HIRAGANA 0x93
#define KEY_ZENKAKUHANKAKU 0x94

#define ASCII_MAP_SIZE 256

typedef struct
{
  uint8_t key_type;
  uint8_t code;
  uint8_t code2;
} my_key;

void kb_print(char* msg, int32_t chardelay, int32_t char_delay_fuzz);
void keyboard_release(my_key* this_key);
void keyboard_press(my_key* this_key, uint8_t use_shift);
void keyboard_release_all(void);
void init_my_key(my_key* kk);
void media_key_release(void);
uint8_t is_mouse_type(my_key* this_key);
void mouse_test(void);
void kb_print_char(my_key *kk, int32_t chardelay, int32_t char_delay_fuzz);

extern uint16_t circumflex;
extern uint16_t diaeresis;
extern uint16_t grave_accent;
extern uint16_t acute_accent;
extern uint16_t tilde;
extern uint16_t cedilla;

extern uint16_t _asciimap[ASCII_MAP_SIZE];

#ifdef __cplusplus
}
#endif

#endif


