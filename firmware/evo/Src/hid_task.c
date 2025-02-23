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
#include "main.h"

#define HID_DP_TO_PC_USAGE_ID 4

#define HID_TX_BUF_SIZE (CUSTOM_HID_EPIN_SIZE+1)
uint8_t hid_tx_buf[HID_TX_BUF_SIZE];

void send_hid_cmd_response(uint8_t* hid_cmdbuf)
{
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_cmdbuf, HID_TX_BUF_SIZE);
}

uint8_t parse_hid_goto_profile_by_name(const uint8_t* this_buf)
{
  if(this_buf == NULL)
    return 255;
  char* pf_name_start = (char*)this_buf + 2;
  for (uint8_t i = 0; i < MAX_PROFILES; i++)
  {
    if(strlen(profile_name_list[i]) == 0)
      continue;
    if(strcmp(profile_name_list[i], pf_name_start) == 0)
      return i;
  }
  return 255;
}

void parse_hid_msg(const uint8_t* this_msg)
{
  printf("%ld HID MSG:\n", millis());
  for (size_t i = 0; i < USBD_CUSTOMHID_OUTREPORT_BUF_SIZE-1; i++)
    printf("%02x ", this_msg[i]);
  printf("\n--------\n");

  uint8_t command_type = this_msg[1];
  memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
  hid_tx_buf[0] = HID_DP_TO_PC_USAGE_ID; // HID usage ID
  hid_tx_buf[1] = HID_RESPONSE_OK;

  /*
    duckyPad to PC
    [0]   seq number (not used)
    [1]   Status
  */
  if(is_busy)
  {
    hid_tx_buf[1] = HID_RESPONSE_BUSY;
    send_hid_cmd_response(hid_tx_buf);
    return;
  }

  /*
    GET INFO
    -----------
    PC to duckyPad:
    [0]   seq number (not used)
    [1]   command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Status, 0 = OK
    [2]   firmware version major
    [3]   firmware version minor
    [4]   firmware version patch
    [5]   hardware revision
    [6 - 9]   UUID (uint32_t)
    [10]   current profile
    [11] is_sleeping
  */
  if(command_type == HID_COMMAND_GET_INFO)
  {
    hid_tx_buf[2] = fw_version_major;
    hid_tx_buf[3] = fw_version_minor;
    hid_tx_buf[4] = fw_version_patch;
    hid_tx_buf[5] = 20;
    hid_tx_buf[6] = (uint8_t)get_uuid();
    hid_tx_buf[7] = (uint8_t)(get_uuid() >> 8);
    hid_tx_buf[8] = (uint8_t)(get_uuid() >> 16);
    hid_tx_buf[9] = (uint8_t)(get_uuid() >> 24);
    hid_tx_buf[10] = current_profile_number;
    hid_tx_buf[11] = is_sleeping;
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
    GOTO PROFILE BY NUMBER
    -----------
    PC to duckyPad:
    [0]   seq number (not used)
    [1]   command
    [2]   profile number
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Status
  */
  else if(command_type == HID_COMMAND_GOTO_PROFILE_BY_NUMBER)
  {
    uint8_t target_profile = this_msg[2];
    if(target_profile >= MAX_PROFILES || strlen(profile_name_list[target_profile]) == 0)
    {
      hid_tx_buf[1] = HID_RESPONSE_INVALID_ARG;
      send_hid_cmd_response(hid_tx_buf);
    }
    else
    {
      wakeup_from_sleep_and_load_profile(target_profile);
      send_hid_cmd_response(hid_tx_buf);
    }
  }
  /*
    GOTO PROFILE BY NAME
    -----------
    PC to duckyPad:
    [0]   seq number (not used)
    [1]   command
    [2]   profile name string, 0 terminated
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Status
  */
  else if(command_type == HID_COMMAND_GOTO_PROFILE_BY_NAME)
  {
    uint8_t target_profile = parse_hid_goto_profile_by_name(this_msg);
    if(target_profile >= MAX_PROFILES)
    {
      hid_tx_buf[1] = HID_RESPONSE_INVALID_ARG;
      send_hid_cmd_response(hid_tx_buf);
    }
    else
    {
      wakeup_from_sleep_and_load_profile(target_profile);
      send_hid_cmd_response(hid_tx_buf);
    }
  }

  /*
    PREV PROFILE
    -----------
    PC to duckyPad:
    [0]   seq number
    [1]   command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   0 = OK
  */
  else if(command_type == HID_COMMAND_PREV_PROFILE)
  {
    wakeup_from_sleep_no_load();
    goto_prev_profile();
    send_hid_cmd_response(hid_tx_buf);
  }
  /*
    NEXT PROFILE
    -----------
    PC to duckyPad:
    [0]   seq number
    [1]   command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   0 = OK
  */
  else if(command_type == HID_COMMAND_NEXT_PROFILE)
  {
    wakeup_from_sleep_no_load();
    goto_next_profile();
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
    SOFTWARE RESET
    -----------
    PC to duckyPad:
    [0]   seq number (not used)
    [1]   command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   0 = OK
  */
  else if(command_type == HID_COMMAND_SW_RESET)
  {
    send_hid_cmd_response(hid_tx_buf);
    delay_ms(250);
    NVIC_SystemReset();
  }

  /*
    SLEEP
    -----------
    PC to duckyPad:
    [0]   seq number (not used)
    [1]   command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   0 = OK
  */
  else if(command_type == HID_COMMAND_SLEEP)
  {
    send_hid_cmd_response(hid_tx_buf);
    start_sleeping();
  }

  /*
    WAKE UP
    -----------
    PC to duckyPad:
    [0]   seq number (not used)
    [1]   command
    -----------
    duckyPad to PC
    [0]   seq number (not used)
    [1]   0 = OK
  */
  else if(command_type == HID_COMMAND_WAKEUP)
  {
    wakeup_from_sleep_and_load_profile(current_profile_number);
    send_hid_cmd_response(hid_tx_buf);
  }
  else // not a valid HID command
  {
    hid_tx_buf[1] = HID_RESPONSE_UNKNOWN_CMD;
    send_hid_cmd_response(hid_tx_buf);
  }

// ---------------

}

void handle_hid_command(const uint8_t* hid_rx_buf)
{
  if(hid_rx_buf[0] == 1) // LED
    kb_led_status = hid_rx_buf[1];
  else if(hid_rx_buf[0] == 5) // PC data
    parse_hid_msg(hid_rx_buf+1);
}

