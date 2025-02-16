#include <stdlib.h>
#include "input_task.h"
#include "sd_util.h"
#include "ui_task.h"
#include "neopixel.h"
#include "shared.h"
#include "profiles.h"
#include "ssd1306.h"
#include "keypress_task.h"
#include "keyboard.h"
#include "ds_vm.h"
#include "hid_task.h"
#include "usb_device.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

#define HID_TX_BUF_SIZE (CUSTOM_HID_EPIN_SIZE+1)
uint8_t hid_tx_buf[HID_TX_BUF_SIZE];

void handle_hid_command(const uint8_t* hid_rx_buf)
{
  printf("%ld HID MSG:\n", millis());
  for (size_t i = 0; i < USBD_CUSTOMHID_OUTREPORT_BUF_SIZE; i++)
    printf("%02x ", hid_rx_buf[i]);
  printf("\n--------\n");

  if(hid_rx_buf[0] == 1) //LED
  {
    return;
  }
  else if(hid_rx_buf[0] == 5) // PC data
  {
    hid_tx_buf[0] = 4;
    uint8_t result = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    printf("hid tx: %d\n", result);
    return;
  }
}
