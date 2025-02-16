#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shared.h"
#include "keyboard.h"
#include "input_task.h"
#include "ui_task.h"
#include "neopixel.h"
#include "profiles.h"
#include "keypress_task.h"
// #include "ds_vm.h"
#include "hid_task.h"
#include "usb_device.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

#define SHIFT 0x100
#define ALT_GR 0x200

uint16_t duckcode_circumflex;
uint16_t duckcode_diaeresis;
uint16_t duckcode_grave_accent;
uint16_t duckcode_acute_accent;
uint16_t duckcode_tilde;
uint16_t duckcode_cedilla;

my_key deadkey;

uint16_t ascii_map[ASCII_MAP_SIZE] =
{
  0x00,             // NUL
  0x00,             // SOH
  0x00,             // STX
  0x00,             // ETX
  0x00,             // EOT
  0x00,             // ENQ
  0x00,             // ACK  
  0x00,             // BEL
  0x2a,             // BS Backspace
  0x2b,             // TAB  Tab
  0x28,             // LF Enter
  0x00,             // VT 
  0x00,             // FF 
  0x00,             // CR 
  0x00,             // SO 
  0x00,             // SI 
  0x00,             // DEL
  0x00,             // DC1
  0x00,             // DC2
  0x00,             // DC3
  0x00,             // DC4
  0x00,             // NAK
  0x00,             // SYN
  0x00,             // ETB
  0x00,             // CAN
  0x00,             // EM 
  0x00,             // SUB
  0x00,             // ESC
  0x00,             // FS 
  0x00,             // GS 
  0x00,             // RS 
  0x00,             // US 

  0x2c,          //  ' '
  0x1e|SHIFT,    // !
  0x34|SHIFT,    // "
  0x20|SHIFT,    // #
  0x21|SHIFT,    // $
  0x22|SHIFT,    // %
  0x24|SHIFT,    // &
  0x34,          // '
  0x26|SHIFT,    // (
  0x27|SHIFT,    // )
  0x25|SHIFT,    // *
  0x2e|SHIFT,    // +
  0x36,          // ,
  0x2d,          // -
  0x37,          // .
  0x38,          // /
  0x27,          // 0
  0x1e,          // 1
  0x1f,          // 2
  0x20,          // 3
  0x21,          // 4
  0x22,          // 5
  0x23,          // 6
  0x24,          // 7
  0x25,          // 8
  0x26,          // 9
  0x33|SHIFT,      // :
  0x33,          // ;
  0x36|SHIFT,      // <
  0x2e,          // =
  0x37|SHIFT,      // >
  0x38|SHIFT,      // ?
  0x1f|SHIFT,      // @
  0x04|SHIFT,      // A
  0x05|SHIFT,      // B
  0x06|SHIFT,      // C
  0x07|SHIFT,      // D
  0x08|SHIFT,      // E
  0x09|SHIFT,      // F
  0x0a|SHIFT,      // G
  0x0b|SHIFT,      // H
  0x0c|SHIFT,      // I
  0x0d|SHIFT,      // J
  0x0e|SHIFT,      // K
  0x0f|SHIFT,      // L
  0x10|SHIFT,      // M
  0x11|SHIFT,      // N
  0x12|SHIFT,      // O
  0x13|SHIFT,      // P
  0x14|SHIFT,      // Q
  0x15|SHIFT,      // R
  0x16|SHIFT,      // S
  0x17|SHIFT,      // T
  0x18|SHIFT,      // U
  0x19|SHIFT,      // V
  0x1a|SHIFT,      // W
  0x1b|SHIFT,      // X
  0x1c|SHIFT,      // Y
  0x1d|SHIFT,      // Z
  0x2f,          // [
  0x31,          // bslash
  0x30,          // ]
  0x23|SHIFT,    // ^
  0x2d|SHIFT,    // _
  0x35,          // `
  0x04,          // a
  0x05,          // b
  0x06,          // c
  0x07,          // d
  0x08,          // e
  0x09,          // f
  0x0a,          // g
  0x0b,          // h
  0x0c,          // i
  0x0d,          // j
  0x0e,          // k
  0x0f,          // l
  0x10,          // m
  0x11,          // n
  0x12,          // o
  0x13,          // p
  0x14,          // q
  0x15,          // r
  0x16,          // s
  0x17,          // t
  0x18,          // u
  0x19,          // v
  0x1a,          // w
  0x1b,          // x
  0x1c,          // y
  0x1d,          // z
  0x2f|SHIFT,    // {
  0x31|SHIFT,    // |
  0x30|SHIFT,    // }
  0x35|SHIFT,    // ~
  0x00,          // DEL
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,// ¡
  0x00,// ¢
  0x00,// £
  0x00,// ¤
  0x00,// ¥
  0x00,// ¦
  0x00,// §
  0x00,// ¨
  0x00,// ©
  0x00,// ª
  0x00,// «
  0x00,// ¬
  0x00,
  0x00,// ®
  0x00,// ¯
  0x00,// °
  0x00,// ±
  0x00,// ²
  0x00,// ³
  0x00,// ´
  0x00,// µ
  0x00,// ¶
  0x00,// ·
  0x00,// ¸
  0x00,// ¹
  0x00,// º
  0x00,// »
  0x00,// ¼
  0x00,// ½
  0x00,// ¾
  0x00,// ¿
  0x00,// À
  0x00,// Á
  0x00,// Â
  0x00,// Ã
  0x00,// Ä
  0x00,// Å
  0x00,// Æ
  0x00,// Ç
  0x00,// È
  0x00,// É
  0x00,// Ê
  0x00,// Ë
  0x00,// Ì
  0x00,// Í
  0x00,// Î
  0x00,// Ï
  0x00,// Ð
  0x00,// Ñ
  0x00,// Ò
  0x00,// Ó
  0x00,// Ô
  0x00,// Õ
  0x00,// Ö
  0x00,// ×
  0x00,// Ø
  0x00,// Ù
  0x00,// Ú
  0x00,// Û
  0x00,// Ü
  0x00,// Ý
  0x00,// Þ
  0x00,// ß
  0x00,// à
  0x00,// á
  0x00,// â
  0x00,// ã
  0x00,// ä
  0x00,// å
  0x00,// æ
  0x00,// ç
  0x00,// è
  0x00,// é
  0x00,// ê
  0x00,// ë
  0x00,// ì
  0x00,// í
  0x00,// î
  0x00,// ï
  0x00,// ð
  0x00,// ñ
  0x00,// ò
  0x00,// ó
  0x00,// ô
  0x00,// õ
  0x00,// ö
  0x00,// ÷
  0x00,// ø
  0x00,// ù
  0x00,// ú
  0x00,// û
  0x00,// ü
  0x00,// ý
  0x00,// þ
  0x00,// ÿ
};

void delay_wrapper(int32_t amount, int32_t fuzz)
{
  delay_ms(fuzz == 0 ? amount : amount + rand() % fuzz);
}

uint8_t kb_buf[DP_HID_MSG_SIZE];

void keyboard_release_all(void)
{
  memset(kb_buf, 0, DP_HID_MSG_SIZE);
  kb_buf[0] = HID_USAGE_ID_KEYBOARD;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

uint8_t is_mouse_type(my_key* this_key)
{
  return this_key->type >= KEY_TYPE_MOUSE_BUTTON && this_key->type <= KEY_TYPE_MOUSE_MOVEMENT;
}

void media_key_release(void)
{
  memset(kb_buf, 0, DP_HID_MSG_SIZE);
  kb_buf[0] = HID_USAGE_ID_MEDIA_KEY;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

void media_key_press(my_key* this_key)
{
  memset(kb_buf, 0, DP_HID_MSG_SIZE);
  kb_buf[0] = HID_USAGE_ID_MEDIA_KEY;
  kb_buf[1] = this_key->code;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

uint8_t should_use_mod(uint8_t ttt)
{
  switch(ttt)
  {
    case KEY_TYPE_CHAR: return 1;
    case KEY_TYPE_DEAD_GRAVE_ACCENT: return 1;
    case KEY_TYPE_DEAD_ACUTE_ACCENT: return 1;
    case KEY_TYPE_DEAD_CIRCUMFLEX: return 1;
    case KEY_TYPE_DEAD_TILDE: return 1;
    case KEY_TYPE_DEAD_DIAERESIS: return 1;
    case KEY_TYPE_DEAD_CEDILLA: return 1;
  }
  return 0;
}

void mouse_press(my_key* this_key)
{
  memset(kb_buf, 0, DP_HID_MSG_SIZE);
  kb_buf[0] = HID_USAGE_ID_MOUSE;

  if(this_key->type == KEY_TYPE_MOUSE_BUTTON)
  {
    kb_buf[1] = this_key->code;
  }
  else if(this_key->type == KEY_TYPE_MOUSE_MOVEMENT)
  {
    kb_buf[2] = this_key->code;
    kb_buf[3] = ~(this_key->code2) + 1;
  }
  else if(this_key->type == KEY_TYPE_MOUSE_WHEEL)
  {
    kb_buf[4] = this_key->code;
  }
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

void mouse_release(my_key* this_key)
{
  kb_buf[0] = HID_USAGE_ID_MOUSE;
  if(this_key->type == KEY_TYPE_MOUSE_BUTTON)
  {
    kb_buf[1] = 0;
  }
  else if(this_key->type == KEY_TYPE_MOUSE_MOVEMENT)
  {
    kb_buf[2] = 0;
    kb_buf[3] = 0;
  }
  else if(this_key->type == KEY_TYPE_MOUSE_WHEEL)
  {
    kb_buf[4] = 0;
  }
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

void mouse_release_all(void)
{
  memset(kb_buf, 0, DP_HID_MSG_SIZE);
  kb_buf[0] = HID_USAGE_ID_MOUSE;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

void action_press(my_key* this_key, uint8_t use_mod)
{
  uint16_t duckcode;
  if(this_key->type == KEY_TYPE_MEDIA)
  {
    media_key_press(this_key);
    return;
  }
  else if(is_mouse_type(this_key))
  {
    mouse_press(this_key);
    return;
  }
  else if(this_key->type == KEY_TYPE_MODIFIER)
  {
    kb_buf[1] |= this_key->code;
    duckcode = 0;
  }
  else if(this_key->type == KEY_TYPE_SPECIAL)
    duckcode = this_key->code;
  else if(this_key->type == KEY_TYPE_CHAR)
    duckcode = ascii_map[this_key->code];
  else if(this_key->type == KEY_TYPE_DEAD_GRAVE_ACCENT)
    duckcode = duckcode_grave_accent;
  else if(this_key->type == KEY_TYPE_DEAD_ACUTE_ACCENT)
    duckcode = duckcode_acute_accent;
  else if(this_key->type == KEY_TYPE_DEAD_CIRCUMFLEX)
    duckcode = duckcode_circumflex;
  else if(this_key->type == KEY_TYPE_DEAD_TILDE)
    duckcode = duckcode_tilde;
  else if(this_key->type == KEY_TYPE_DEAD_DIAERESIS)
    duckcode = duckcode_diaeresis;
  else if(this_key->type == KEY_TYPE_DEAD_CEDILLA)
    duckcode = duckcode_cedilla;
  else
    return;

  if(use_mod && should_use_mod(this_key->type))
  {
    if(duckcode & SHIFT)
      kb_buf[1] |= KEY_LEFT_SHIFT;
    if(duckcode & ALT_GR)
      kb_buf[1] |= KEY_RIGHT_ALT;
  }
  duckcode = duckcode & 0xff;
  if(kb_buf[2] != duckcode && kb_buf[3] != duckcode && kb_buf[4] != duckcode && kb_buf[5] != duckcode && kb_buf[6] != duckcode && kb_buf[7] != duckcode)
  {
    for (int i = 2; i < DP_HID_MSG_SIZE; ++i)
      if(kb_buf[i] == 0)
      {
        kb_buf[i] = (uint8_t)duckcode;
        break;
      }
  }
  kb_buf[0] = HID_USAGE_ID_KEYBOARD;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

void action_release(my_key* this_key)
{
  uint16_t duckcode;
  if(this_key->type == KEY_TYPE_MEDIA)
  {
    media_key_release();
    return;
  }
  else if(is_mouse_type(this_key))
  {
    mouse_release(this_key);
    return;
  }
  else if(this_key->type == KEY_TYPE_MODIFIER)
  {
    kb_buf[1] &= ~(this_key->code);
    duckcode = 0;
  }
  else if(this_key->type == KEY_TYPE_SPECIAL)
    duckcode = this_key->code;
  else if(this_key->type == KEY_TYPE_CHAR)
    duckcode = ascii_map[this_key->code];
  else if(this_key->type == KEY_TYPE_DEAD_GRAVE_ACCENT)
    duckcode = duckcode_grave_accent;
  else if(this_key->type == KEY_TYPE_DEAD_ACUTE_ACCENT)
    duckcode = duckcode_acute_accent;
  else if(this_key->type == KEY_TYPE_DEAD_CIRCUMFLEX)
    duckcode = duckcode_circumflex;
  else if(this_key->type == KEY_TYPE_DEAD_TILDE)
    duckcode = duckcode_tilde;
  else if(this_key->type == KEY_TYPE_DEAD_DIAERESIS)
    duckcode = duckcode_diaeresis;
  else if(this_key->type == KEY_TYPE_DEAD_CEDILLA)
    duckcode = duckcode_cedilla;
  else
    return;

  if(should_use_mod(this_key->type))
  {
    if(duckcode & SHIFT)
      kb_buf[1] &= ~(KEY_LEFT_SHIFT);
    if(duckcode & ALT_GR)
      kb_buf[1] &= ~(KEY_RIGHT_ALT);
  }
  duckcode = duckcode & 0xff;
  for (int i = 2; i < DP_HID_MSG_SIZE; ++i)
    if(kb_buf[i] == (uint8_t)duckcode)
      kb_buf[i] = 0;
  kb_buf[0] = HID_USAGE_ID_KEYBOARD;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, DP_HID_MSG_SIZE);
}

static uint8_t c1;
uint8_t utf8ascii(uint8_t ascii)
{
  if(ascii<128) // Standard ASCII-set 0..0x7F handling  
  {   
    c1=0;
    return ascii;
  }
  // get previous input
  uint8_t last = c1; // get last char
  c1=ascii; // remember actual character

  switch (last) // conversion depending on first UTF8-character
  {   
    case 0xC2: return  (ascii);  break;
    case 0xC3: return  (ascii | 0xC0);  break;
    case 0x82: if(ascii==0xAC) return(0x80); // special case Euro-symbol
  }
  return 0; // otherwise: return zero, if character has to be ignored
}

/*
  kk.code is the ASCII character
  look it up in the keymap, result is uint16_t
  top 4 bits map to dead key 
  if no dead key, press as normal
  if has dead key, press it first

  dead key first, dead key itself might contain MOD keys
  */
my_key temp_shift_key;
my_key temp_altgr_key;
void kb_print_char(my_key *kk, int32_t chardelay, int32_t charjitter)
{
  uint16_t duckcode = ascii_map[kk->code];
  if(duckcode == 0)
  	return;
  if(duckcode & 0xf000) // deadkey
  {
    switch(duckcode >> 12)
    {
      case 1: deadkey.type = KEY_TYPE_DEAD_GRAVE_ACCENT; break;
      case 2: deadkey.type = KEY_TYPE_DEAD_ACUTE_ACCENT; break;
      case 3: deadkey.type = KEY_TYPE_DEAD_CIRCUMFLEX; break;
      case 4: deadkey.type = KEY_TYPE_DEAD_TILDE; break;
      case 5: deadkey.type = KEY_TYPE_DEAD_DIAERESIS; break;
      case 6: deadkey.type = KEY_TYPE_DEAD_CEDILLA; break;
      default: deadkey.type = KEY_TYPE_UNKNOWN; deadkey.code = 0;
    }
    action_press(&deadkey, 1);
    delay_wrapper(chardelay, charjitter);
    action_release(&deadkey);
    delay_wrapper(chardelay, charjitter);
  }
  if(duckcode & SHIFT)
  {
    temp_shift_key.type = KEY_TYPE_MODIFIER;
    temp_shift_key.code = KEY_LEFT_SHIFT;
    action_press(&temp_shift_key, 1);
    delay_wrapper(chardelay, charjitter);
  }
  if(duckcode & ALT_GR)
  {
    temp_altgr_key.type = KEY_TYPE_MODIFIER;
    temp_altgr_key.code = KEY_RIGHT_ALT;
    action_press(&temp_altgr_key, 1);
    delay_wrapper(chardelay, charjitter);
  }
  action_press(kk, 1);
  delay_wrapper(chardelay, charjitter);
  action_release(kk);
  delay_wrapper(chardelay, charjitter);
  if(duckcode & ALT_GR)
  {
    action_release(&temp_altgr_key);
    delay_wrapper(chardelay, charjitter);
  }
  if(duckcode & SHIFT)
  {
    action_release(&temp_shift_key);
    delay_wrapper(chardelay, charjitter);
  }
}

uint8_t kb_print(char* msg, int32_t chardelay, int32_t charjitter)
{
  my_key kk;
  for (int i = 0; i < strlen(msg); ++i)
  {
    if(allow_abort && sw_queue_has_keydown_event())
      return 1;
    kk.type = KEY_TYPE_CHAR;
    kk.code = utf8ascii(msg[i]);
    if(kk.code == 0)
      continue;
    kb_print_char(&kk, chardelay, charjitter);
  }
  return 0;
}

void init_my_key(my_key* kk)
{
  if(kk == NULL)
    return;
  kk->type = KEY_TYPE_UNKNOWN;
  kk->code = 0;
}

void press_key(uint8_t code, uint8_t type)
{
  my_key kk;
  kk.code = code;
  kk.type = type;
  action_press(&kk, 0);
}

void release_key(uint8_t code, uint8_t type)
{
  my_key kk;
  kk.code = code;
  kk.type = type;
  action_release(&kk);
}