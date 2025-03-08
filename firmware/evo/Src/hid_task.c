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
#include "ui_task.h"
#include "md5.h"

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
  char* pf_name_start = (char*)this_buf + 3;
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
  // printf("%ld HID MSG:\n", millis());
  // for (size_t i = 0; i < USBD_CUSTOMHID_OUTREPORT_BUF_SIZE; i++)
  //   printf("%02x ", this_msg[i]);
  // printf("\n--------\n");

  uint8_t command_type = this_msg[2];
  memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
  hid_tx_buf[0] = HID_DP_TO_PC_USAGE_ID; // HID usage ID
  hid_tx_buf[2] = HID_RESPONSE_OK;

  /*
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status
  */
  if(is_busy)
  {
    hid_tx_buf[2] = HID_RESPONSE_BUSY;
    send_hid_cmd_response(hid_tx_buf);
    return;
  }

  /*
    GET INFO
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
    [3]   firmware version major
    [4]   firmware version minor
    [5]   firmware version patch
    [6]   hardware revision
    [7 - 10]   UUID (uint32_t)
    [12]   current profile
    [13] is_sleeping
  */
  if(command_type == HID_COMMAND_GET_INFO)
  {
    hid_tx_buf[3] = fw_version_major;
    hid_tx_buf[4] = fw_version_minor;
    hid_tx_buf[5] = fw_version_patch;
    hid_tx_buf[6] = 20;
    hid_tx_buf[7] = (uint8_t)get_uuid();
    hid_tx_buf[8] = (uint8_t)(get_uuid() >> 8);
    hid_tx_buf[9] = (uint8_t)(get_uuid() >> 16);
    hid_tx_buf[10] = (uint8_t)(get_uuid() >> 24);
    hid_tx_buf[11] = current_profile_number;
    hid_tx_buf[12] = is_sleeping;
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
    GOTO PROFILE BY NUMBER
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    [3]   Profile number
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
  */
  else if(command_type == HID_COMMAND_GOTO_PROFILE_BY_NUMBER)
  {
    uint8_t target_profile = this_msg[3];
    if(target_profile >= MAX_PROFILES || strlen(profile_name_list[target_profile]) == 0)
    {
      hid_tx_buf[2] = HID_RESPONSE_INVALID_ARG;
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
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    [3]   profile name string, 0 terminated
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
  */
  else if(command_type == HID_COMMAND_GOTO_PROFILE_BY_NAME)
  {
    uint8_t target_profile = parse_hid_goto_profile_by_name(this_msg);
    if(target_profile >= MAX_PROFILES)
    {
      hid_tx_buf[2] = HID_RESPONSE_INVALID_ARG;
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
    [1]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
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
    [1]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
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
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
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
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
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
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
  */
  else if(command_type == HID_COMMAND_WAKEUP)
  {
    wakeup_from_sleep_and_load_profile(current_profile_number);
    send_hid_cmd_response(hid_tx_buf);
  }
  /*
    DUMP SD
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command: Dump SD
    [3]   Action: 1: Start/resume, 2: abort
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status

    Status:
    1: Encountered new file
    2: Data Chunk
    3: End of file
    4: End of transmission
  */
  else if(command_type == HID_COMMAND_DUMP_SD)
  {
    // if(this_msg[3] == 2)
    // {
    //   printf("Aborted\n");
    //   temp_buf[0] = 0;
    // }
    ;
  }

  /*
    Enter exclusive file access mode
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   0 = OK
  */
  else if(command_type == HID_COMMAND_ENTER_EXCLUSIVE_MODE)
  {
    is_in_file_access_mode = 1;
    neopixel_fill(127, 127, 127);
    oled_say("File Access Mode");
    send_hid_cmd_response(hid_tx_buf);
  }

  else // not a valid HID command
  {
    hid_tx_buf[2] = HID_RESPONSE_UNKNOWN_CMD;
    send_hid_cmd_response(hid_tx_buf);
  }

// ---------------

}

void handle_hid_command(const uint8_t* hid_rx_buf)
{
  uint32_t ke_start = millis();
  if(hid_rx_buf[0] == 1) // LED
    kb_led_status = hid_rx_buf[1];
  else if(hid_rx_buf[0] == 5) // PC data
    parse_hid_msg(hid_rx_buf);
  printf("HID %ldms\n", millis() - ke_start);
}

#define PROFILE_OVERFLOW 255
uint8_t find_first_profile(void)
{
  for (uint8_t i = 0; i < MAX_PROFILES; i++)
    if(strlen(profile_name_list[i]))
      return i;
  return PROFILE_OVERFLOW;
}

uint8_t find_next_profile(uint8_t current_pf)
{
  current_pf++;
  for (uint8_t i = current_pf; i < MAX_PROFILES; i++)
    if(strlen(profile_name_list[i]))
      return i;
  return PROFILE_OVERFLOW;
}

#define SD_WALK_STATE_IDLE 0
#define SD_WALK_STATE_NEW_PROFILE_DIR 1
#define SD_WALK_STATE_NEW_FILE 2
uint8_t sd_walk_state;
uint8_t sd_walk_current_profile_number;
char* sd_walk_current_file_path;
char* this_file_name;
volatile uint8_t is_in_file_access_mode;
uint8_t md5_buf[MD5_BUF_SIZE];

void sd_walk(void)
{
  if(sd_walk_state == SD_WALK_STATE_IDLE)
  {
    sd_walk_state = SD_WALK_STATE_NEW_PROFILE_DIR;
    // hid reply: ack
    sd_walk_current_profile_number = find_first_profile();
    if(sd_walk_current_profile_number == PROFILE_OVERFLOW)
      draw_fatal_error(10);
    return;
  }

  if(sd_walk_state == SD_WALK_STATE_NEW_PROFILE_DIR)
  {
    if(sd_walk_current_profile_number == PROFILE_OVERFLOW)
    {
      printf("all done!"); //  HID send EOT
      sd_walk_state = SD_WALK_STATE_IDLE;
      return;
    }
    CLEAR_TEMP_BUF();
    sprintf(temp_buf, "/profile_%s", profile_name_list[sd_walk_current_profile_number]);
    printf("In dir: %s\n", temp_buf);
    // open dir, go to next state
    if(f_opendir(&dir, temp_buf))
      draw_fatal_error(20);
    fno.lfname = lfn_buf; 
    fno.lfsize = FILENAME_BUFSIZE - 1;
    sd_walk_state = SD_WALK_STATE_NEW_FILE;
    return;
  }

  if(sd_walk_state == SD_WALK_STATE_NEW_FILE)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    while(1)
    {
      memset(lfn_buf, 0, FILENAME_BUFSIZE);
      sd_fresult = f_readdir(&dir, &fno);
      if (fno.fattrib & AM_DIR)
        continue;
      if (sd_fresult != FR_OK || fno.fname[0] == 0)
      {
        // done with this dir, time for next
        sd_walk_state = SD_WALK_STATE_NEW_PROFILE_DIR;
        sd_walk_current_profile_number = find_next_profile(sd_walk_current_profile_number);
        f_closedir(&dir);
        printf("this profile done\n");
        return;
      }
      break;
    }
    // we found the next file
    this_file_name = fno.lfname[0] ? fno.lfname : fno.fname;
    CLEAR_TEMP_BUF();
    sprintf(temp_buf, "/profile_%s/%s", profile_name_list[sd_walk_current_profile_number], this_file_name);
    printf("current file: %s ", temp_buf);

    memset(md5_buf, 0, MD5_BUF_SIZE);
    md5File(temp_buf, md5_buf);
    print_hash(md5_buf);
    return;
  }
}

uint8_t make_file_walk_hid_packet(char* filename)
{

}