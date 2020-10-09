#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

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
#define KEY_F1 0X3A
#define KEY_F2 0X3B
#define KEY_F3 0X3C
#define KEY_F4 0X3D
#define KEY_F5 0X3E
#define KEY_F6 0X3F
#define KEY_F7 0X40
#define KEY_F8 0X41
#define KEY_F9 0X42
#define KEY_F10 0X43
#define KEY_F11 0X44
#define KEY_F12 0X45
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

#define KEY_MENU 0x65

#define KEY_MK_VOLDOWN 0x80
#define KEY_MK_VOLUP 0x40
#define KEY_MK_VOLMUTE 0x20
#define KEY_MK_PLAYPAUSE 0x10
#define KEY_MK_STOP 0x4
#define KEY_MK_PREV 0x2
#define KEY_MK_NEXT 0x1

#define KB_BUF_SIZE 5

#define KB_LAYOUT_WQERTY 0
#define KB_LAYOUT_AZERTY_FR 1
#define KB_LAYOUT_AZERTY_BE 2
#define KB_LAYOUT_DVORAK 3

#define KEY_TYPE_UNKNOWN 0
#define KEY_TYPE_CHAR 1
#define KEY_TYPE_MODIFIER 2
#define KEY_TYPE_SPECIAL 3
#define KEY_TYPE_MEDIA 4

typedef struct
{
  uint8_t key_type;
  uint8_t code;
} my_key;

void kb_print(char* msg, uint16_t chardelay);
void keyboard_release(my_key* this_key);
void keyboard_press(my_key* this_key, uint8_t use_shift);
void keyboard_release_all(void);
void init_my_key(my_key* kk);

extern uint8_t curr_kb_layout;

#ifdef __cplusplus
}
#endif

#endif


