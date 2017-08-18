#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usb_device.h"
#include "usbd_hid.h"
#include "helpers.h"
#include "shared.h"
#include "keyboard.h"

uint8_t keyboard_buf[KB_BUF_SIZE] = {1, 0, 0, 0, 0};
uint8_t kb_release_all[KB_BUF_SIZE] = {1, 0, 0, 0, 0};

void release_all(void)
{
  USBD_HID_SendReport(&hUsbDeviceFS, kb_release_all, KB_BUF_SIZE);
}

void kb_test(void)
{
  keyboard_buf[2] = 44;
  USBD_HID_SendReport(&hUsbDeviceFS, keyboard_buf, 5);
  HAL_Delay(50);
  release_all();
}

