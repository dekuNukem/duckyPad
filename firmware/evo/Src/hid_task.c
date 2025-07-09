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

volatile uint8_t needs_gv_save;
volatile uint8_t is_in_file_access_mode;

void enter_file_access_mode(void)
{
  if(is_in_file_access_mode)
    return;
  is_in_file_access_mode = 1;
  neopixel_fill(127, 127, 127);
  oled_say("File Access Mode");
}

uint8_t delete_node (
    char* path,    /* Path name buffer with the sub-directory to delete */
    uint8_t sz_buff,   /* Size of path name buffer (items) */
    FILINFO* my_fno    /* Name read buffer */
)
{
    uint16_t i, j;
    uint16_t fr;
    DIR dir;

    fr = f_opendir(&dir, path); /* Open the sub-directory to make it empty */
    if (fr != FR_OK) return fr;

    for (i = 0; path[i]; i++) ; /* Get current path length */
    path[i++] = _T('/');

    for (;;) {
        fr = f_readdir(&dir, my_fno);  /* Get a directory item */
        if (fr != FR_OK || !my_fno->fname[0]) break;   /* End of directory? */
        j = 0;
        do {    /* Make a path name */
            if (i + j >= sz_buff) { /* Buffer over flow? */
                fr = 100; break;    /* Fails with 100 when buffer overflow */
            }
            path[i + j] = my_fno->fname[j];
        } while (my_fno->fname[j++]);
        if (my_fno->fattrib & AM_DIR) {    /* Item is a sub-directory */
            fr = delete_node(path, sz_buff, my_fno);
        } else {                        /* Item is a file */
            fr = f_unlink(path);
        }
        if (fr != FR_OK) break;
    }

    path[--i] = 0;  /* Restore the path name */
    f_closedir(&dir);

    if (fr == FR_OK) fr = f_unlink(path);  /* Delete the empty sub-directory */
    return fr;
}

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

void split_uint16(uint16_t input, uint8_t* high_byte, uint8_t* low_byte)
{
  if (high_byte == NULL || low_byte == NULL)
    return;
  *high_byte = (input >> 8) & 0xFF;
  *low_byte = input & 0xFF;
}

uint16_t combine_uint16(uint8_t high_byte, uint8_t low_byte)
{
  return ((uint16_t)high_byte << 8) | low_byte;
}

void parse_hid_msg(uint8_t* this_msg)
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
  DUMP GV
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
  [3-4] GV0
  [5-6] GV1
  .....
  [61-62] GV29
  */
  if(command_type == HID_COMMAND_DUMP_GV)
  {
    for (size_t i=3; i < HID_TX_BUF_SIZE; i+=2)
    {
      uint8_t this_gv = (i-3)/2;
      if(this_gv >= GLOBAL_VARIABLE_COUNT)
        continue;
      uint8_t* upper_byte = &hid_tx_buf[i];
      uint8_t* lower_byte = &hid_tx_buf[i+1];
      split_uint16(gv_buf[this_gv], upper_byte, lower_byte);
    }
    send_hid_cmd_response(hid_tx_buf);
    return;
  }
  /*
    Write GV
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command

    [3] 127 + GV index (0 indexed)
    [4] Upper Byte
    [5] Lower Byte

    [6-8] next chunk (if needed)
    etc
    -----------
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status, 0 = OK
  */
  else if(command_type == HID_COMMAND_WRITE_GV)
  {
    for (size_t i = 3; i < HID_TX_BUF_SIZE; i+=3)
    {
      if((this_msg[i] & 0x80) == 0)
        continue;
      uint8_t this_gv_index = this_msg[i] & 0x7f;
      if(this_gv_index >= GLOBAL_VARIABLE_COUNT)
        continue;
      gv_buf[this_gv_index] = combine_uint16(this_msg[i+1], this_msg[i+2]);
      needs_gv_save = 1;
    }
    send_hid_cmd_response(hid_tx_buf);
    return;
  }
  /*
    duckyPad to PC
    [0]   Usage ID, always 4
    [1]   Unused
    [2]   Status
  */
  else if(is_busy)
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
    [0]   not used
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
    [0]   not used
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
    -----------
    duckyPad to PC
    See excel file

  */
  else if(command_type == HID_COMMAND_DUMP_SD)
  {
    enter_file_access_mode();
    sd_walk(hid_tx_buf);
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
    Open file for reading
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command: open file for reading
    -----------
    duckyPad to PC
    See excel file
  */
  else if(command_type == HID_COMMAND_OPEN_FILE_FOR_READING)
  {
    enter_file_access_mode();
    CLEAR_TEMP_BUF();
    strncpy(temp_buf, this_msg+3, HID_READ_FILE_PATH_SIZE_MAX);
    // printf("%s\n", temp_buf);
    f_close(&sd_file);
    hid_tx_buf[2] = f_open(&sd_file, temp_buf, FA_READ);
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
    Read File
    -----------
    PC to duckyPad:
    [0]   Usage ID, always 5
    [1]   Unused
    [2]   Command: Read file
    -----------
    duckyPad to PC
    See excel file
  */
  else if(command_type == HID_COMMAND_READ_FILE)
  {
    enter_file_access_mode();
    hid_tx_buf[1] = f_read(&sd_file, hid_tx_buf+3, HID_FILE_READ_PAYLOAD_SIZE, &bytes_read);
    hid_tx_buf[2] = (uint8_t)bytes_read;
    if(bytes_read == 0)
      f_close(&sd_file);
    send_hid_cmd_response(hid_tx_buf);
  }
  /*
  HID OPEN FILE FOR WRITING
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   not used
  [2]   command: 14
  [3 ... 63]   file path, zero-terminated string
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   unused
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_OPEN_FILE_FOR_WRITING)
  {
    enter_file_access_mode();
    if(f_open(&sd_file, this_msg+3, FA_CREATE_ALWAYS | FA_WRITE) != 0)
    {
      hid_tx_buf[2] = HID_RESPONSE_GENERIC_ERROR;
      goto hid_open_for_write_end;
    }
    hid_tx_buf[2] = HID_RESPONSE_OK;
    hid_open_for_write_end:
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
  HID WRITE TO FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   data length in bytes
  [2]   command: 15
  [3 ... 63]   content
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   reserved
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_WRITE_FILE)
  {
    enter_file_access_mode();
    if(f_write(&sd_file, this_msg+3, this_msg[1], &bytes_read) != 0)
      hid_tx_buf[2] = HID_RESPONSE_GENERIC_ERROR;
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
  HID CLOSE CURRENTLY OPENED FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   not used
  [2]   command: 16
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   not used
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_CLOSE_FILE)
  {
    enter_file_access_mode();
    f_close(&sd_file);
    send_hid_cmd_response(hid_tx_buf);
  }

  /*
  HID DELETE FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   unused
  [2]   command
  [3 ... 63]   file name string, zero terminated
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   unused
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_DELETE_FILE)
  {
    enter_file_access_mode();
    f_close(&sd_file);
    f_unlink(this_msg+3);
    send_hid_cmd_response(hid_tx_buf);
  }
  /*
  HID create DIR
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   unused
  [2]   command
  [3 ... 63]   directory name string, zero terminated
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   unused
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_CREATE_DIR)
  {
    enter_file_access_mode();
    if(f_mkdir(this_msg+3) != 0)
      hid_tx_buf[2] = HID_RESPONSE_GENERIC_ERROR;
    send_hid_cmd_response(hid_tx_buf);
  }
  /*
  HID DELETE DIR
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   unused
  [2]   command
  [3 ... 63]   dir name string, zero terminated
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   unused
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_DELETE_DIR)
  {
    enter_file_access_mode();
    if(delete_node(this_msg+3, HID_TX_BUF_SIZE - 3, &fno) != FR_OK)
      hid_tx_buf[2] = HID_RESPONSE_GENERIC_ERROR;
    send_hid_cmd_response(hid_tx_buf);
  }
  else // not a valid HID command
  {
    hid_tx_buf[2] = HID_RESPONSE_UNKNOWN_CMD;
    send_hid_cmd_response(hid_tx_buf);
  }
}

void handle_hid_command(uint8_t* hid_rx_buf)
{
  uint32_t ke_start = millis();
  if(hid_rx_buf[0] == 1) // LED
    kb_led_status = hid_rx_buf[1];
  else if(hid_rx_buf[0] == 5) // PC data
    parse_hid_msg(hid_rx_buf);
  // printf("HID %ldms\n", millis() - ke_start);
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
#define MAX_FILENAME_LEN_IN_HID_PAYLOAD 45
uint8_t sd_walk_state;
uint8_t sd_walk_current_profile_number;
char* sd_walk_current_file_path;
char* this_file_name;
uint8_t md5_buf[MD5_BUF_SIZE];

void sd_walk(uint8_t* res_buf)
{
  memset(res_buf, 0, HID_TX_BUF_SIZE);
  res_buf[0] = 4; // usage ID, always 4
  if(sd_walk_state == SD_WALK_STATE_IDLE)
  {
    sd_walk_state = SD_WALK_STATE_NEW_PROFILE_DIR;
    sd_walk_current_profile_number = find_first_profile();
    if(sd_walk_current_profile_number == PROFILE_OVERFLOW)
      draw_fatal_error(10);
    // HID Response: Ack
    res_buf[1] = 0;
    return;
  }

  if(sd_walk_state == SD_WALK_STATE_NEW_PROFILE_DIR)
  {
    if(sd_walk_current_profile_number == PROFILE_OVERFLOW)
    {
      printf("all done!");
      sd_walk_state = SD_WALK_STATE_IDLE;
      // HID response: End of Transmission
      res_buf[1] = 4;
      return;
    }
    CLEAR_TEMP_BUF();
    sprintf(temp_buf, "/profile_%s", profile_name_list[sd_walk_current_profile_number]);
    printf("In dir: %s\n", temp_buf);
    if(f_opendir(&dir, temp_buf))
      draw_fatal_error(20);
    fno.lfname = lfn_buf; 
    fno.lfsize = FILENAME_BUFSIZE - 1;
    sd_walk_state = SD_WALK_STATE_NEW_FILE;
    // HID response: New DIR
    res_buf[1] = 1;
    strncpy(res_buf+2, temp_buf, MAX_FILENAME_LEN_IN_HID_PAYLOAD);
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
        // HID Response: Ack
        res_buf[1] = 0;
        return;
      }
      break;
    }
    // we found the next file
    this_file_name = fno.lfname[0] ? fno.lfname : fno.fname;
    make_file_walk_hid_packet(this_file_name, profile_name_list[sd_walk_current_profile_number], res_buf);
    return;
  }
}
#define HID_WALK_FILE_PAYLOAD_SIZE 59
#define HID_MD5_PAYLOAD_FILENAME_START 18
uint8_t make_file_walk_hid_packet(char* file_name, char* profile_name, uint8_t* tx_buf)
{
  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/profile_%s/%s", profile_name, this_file_name);
  if(f_open(&sd_file, temp_buf, FA_READ))
    draw_fatal_error(30);
  uint32_t this_file_size = f_size(&sd_file);
  uint32_t packet_len = strlen(file_name) + 1 + this_file_size + 1;
  
  // printf("current file: %s %d %d\n", temp_buf, this_file_size, packet_len);
  memset(tx_buf, 0, HID_TX_BUF_SIZE);
  tx_buf[0] = 4; // usage ID, always 4
  if(packet_len <= HID_WALK_FILE_PAYLOAD_SIZE)
  {
    tx_buf[1] = 2; // operation type, payload is file content 
    tx_buf[2] = 4 + strlen(file_name); // file name end, byte at this index should be 0
    tx_buf[3] = tx_buf[2] + this_file_size; // file content end, inclusive

    strncpy(tx_buf+4, file_name, strlen(file_name)); // copy file name into tx buf
    f_read(&sd_file, tx_buf + tx_buf[2] + 1, this_file_size, &bytes_read);
    if(bytes_read != this_file_size)
      draw_fatal_error(40);
  }
  else
  {
    tx_buf[1] = 3; // operation type, payload is MD5
    memset(md5_buf, 0, MD5_BUF_SIZE);
    md5File(&sd_file, md5_buf);
    memcpy(tx_buf+2, md5_buf, MD5_BUF_SIZE);
    strncpy(tx_buf+HID_MD5_PAYLOAD_FILENAME_START, file_name, MAX_FILENAME_LEN_IN_HID_PAYLOAD);
    print_hash(md5_buf);
  }
  f_close(&sd_file);
  return 0;
}
