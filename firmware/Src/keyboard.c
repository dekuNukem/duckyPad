#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usb_device.h"
#include "usbd_hid.h"
#include "shared.h"
#include "keyboard.h"
#if USE_GERMAN_ASCIIMAP
#include "keyboard/asciimap_german.h"
#endif

#if USE_ENGLISH_ASCIIMAP
#error IMPLEMENT - PORT FROM QMK
#endif

uint8_t kb_buf[KB_BUF_SIZE] = {1, 0, 0, 0, 0};

void print_kb_buf(void)
{
  for (int i = 0; i < KB_BUF_SIZE; ++i)
    printf("0x%x ", kb_buf[i]);
  printf("\n");
}

void keyboard_release_all(void)
{
  for (int i = 1; i < KB_BUF_SIZE; ++i)
    kb_buf[i] = 0;
  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, KB_BUF_SIZE);
}

uint8_t is_media_key(uint8_t k)
{
  return k == KEY_VOL_UP || k == KEY_VOL_DOWN || k == KEY_VOL_MUTE;
}

void media_key_press(uint8_t k)
{
  uint8_t usage_id = 0;
  if(k == KEY_VOL_UP)
    usage_id = 0x80;
  else if(k == KEY_VOL_DOWN)
    usage_id = 0x81;
  else if(k == KEY_VOL_MUTE)
    usage_id = 0x7f;

  for (int i = 1; i < KB_BUF_SIZE; ++i)
    kb_buf[i] = 0;
  kb_buf[2] = usage_id;
  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, KB_BUF_SIZE);
}

// adopted from arduino keyboard.c
// can be found in resources folder
void keyboard_press(uint8_t k, uint8_t use_mod)
{
  uint8_t usage_id;
  if(is_media_key(k))
  {
    media_key_press(k);
    return;
  }
  if(k >= 0x88)
    usage_id = k - 0x88;
  else if(k >= 0x80)
  {
    kb_buf[1] |= (1<<(k-0x80));
    usage_id = 0;
  }
  else {
    usage_id = _asciimap[k];

    if(_asciimap_is_shift[k]) {
      kb_buf[1] |= (1 << 1);
    } 
    if(_asciimap_is_altgr[k]) {
      kb_buf[1] |= (1 << 6);
    }
  }
  
  

  if(kb_buf[2] != usage_id && kb_buf[3] != usage_id && kb_buf[4] != usage_id)
  {
    for (int i = 2; i < KB_BUF_SIZE; ++i)
      if(kb_buf[i] == 0)
      {
        kb_buf[i] = usage_id;
        break;
      }
  }
  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, KB_BUF_SIZE);
}

void keyboard_release(uint8_t k)
{
  uint8_t usage_id;

  if(is_media_key(k))
  {
    keyboard_release_all();
    return;
  }

  if(k >= 0x88)
    usage_id = k - 0x88;
  else if(k >= 0x80)
  {
    kb_buf[1] &= ~(1<<(k-0x80));
    usage_id = 0;
  } else {
    usage_id = _asciimap[k];

    if(_asciimap_is_shift[k]) {
      kb_buf[1] &= ~(1 << 1);
    } 
    if(_asciimap_is_altgr[k]) {
      kb_buf[1] &= ~(1 << 6);
    }
  }

  for (int i = 2; i < KB_BUF_SIZE; ++i)
    if(kb_buf[i] == usage_id)
      kb_buf[i] = 0;

  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, KB_BUF_SIZE);
}

void kb_print(char* msg, uint16_t chardelay)
{
  for (int i = 0; i < strlen(msg); ++i)
  {
    keyboard_press(msg[i], 1);
    osDelay(chardelay);
    keyboard_release(msg[i]);
    osDelay(chardelay);
  }
}

void kb_test(void)
{
  ;
}
