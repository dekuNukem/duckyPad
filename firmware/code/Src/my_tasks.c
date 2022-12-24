#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_tasks.h"
#include "shared.h"
#include "usbd_customhid.h"
#include "ssd1306.h"
#include "fonts.h"
#include "neopixel.h"
#include "buttons.h"
#include "keyboard.h"
#include "parser.h"
#include "animations.h"
#include "usb_device.h"
#include "usbd_desc.h"

#define LONG_PRESS_MS 500
#define MAX_KEYMAP_SIZE 8

uint8_t init_complete;
uint32_t last_keypress;
uint32_t next_pixel_shift = 30000;
uint8_t is_sleeping, is_busy;
uint32_t button_hold_start, button_hold_duration;
char default_str[] = "default";

void oled_full_brightness()
{
  last_keypress = HAL_GetTick();
  ssd1306_dim(0);
}

void draw_brightness_value()
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, " %d%% ", brightness_values[brightness_index]);
  ssd1306_SetCursor((128 - strlen(temp_buf)*6)/2, 30);
  ssd1306_WriteString(temp_buf,Font_6x10,White);
  ssd1306_UpdateScreen();
}

void service_all(void)
{
  for (int i = 0; i < KEY_COUNT; ++i)
    service_press(&button_status[i]);
}

void set_brightness(void)
{
    draw_brightness_value();
    redraw_bg();
    osDelay(30);
    service_all();
    save_last_profile(p_cache.current_profile);
}

void change_brightness()
{   
    redraw_bg();
    osDelay(30);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(34, 8);
    ssd1306_WriteString("Brightness",Font_6x10,White);
    ssd1306_SetCursor(25, 30);
    ssd1306_WriteString("-",Font_6x10,White);
    ssd1306_SetCursor(98, 30);
    ssd1306_WriteString("+",Font_6x10,White);
    ssd1306_SetCursor(1, 53);
    ssd1306_WriteString("Press any key to exit",Font_6x10,White);
    ssd1306_UpdateScreen();
    draw_brightness_value();
    service_all();
    while(1)
    {
      HAL_IWDG_Refresh(&hiwdg);
      keyboard_update();

      for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
          if(is_pressed(&button_status[i]))
              return;

      if(is_pressed(&button_status[KEY_BUTTON1])) // -
      {
          brightness_index--;
          if(brightness_index < 0)
              brightness_index = 0;
          set_brightness();
      }
      if(is_pressed(&button_status[KEY_BUTTON2])) // +
      {
          brightness_index++;
          if(brightness_index >= BRIGHTNESS_LEVELS)
              brightness_index = BRIGHTNESS_LEVELS - 1;
          set_brightness();
      }
      osDelay(50);
    }
}

void handle_tactile_button_press(uint8_t button_num)
{
    button_hold_start = HAL_GetTick();
    while(1)
    {
        HAL_IWDG_Refresh(&hiwdg);
        keyboard_update();
        button_hold_duration = HAL_GetTick() - button_hold_start;
        if(button_status[button_num].button_state == BUTTON_RELEASED)
            break;
        if(button_hold_duration > LONG_PRESS_MS)
            break;
    }
    keyboard_release_all();
    media_key_release();
    if(button_hold_duration < LONG_PRESS_MS) // short press
    {
        if(button_num == KEY_BUTTON1) // -
          change_profile(PREV_PROFILE);
        else if(button_num == KEY_BUTTON2) // +
          change_profile(NEXT_PROFILE);
    }
    else // long press
    {
      if(button_num == KEY_BUTTON1) // -
      {
        is_busy = 1;
        change_brightness();
        save_settings();
        is_busy = 0;
        print_legend(0, 0);
        service_all();
      }
    }
}

const char str_circumflex[] = "dk_circumflex";
const char str_diaeresis[] = "dk_diaeresis";
const char str_grave_accent[] = "dk_grave_accent";
const char str_acute_accent[] = "dk_acute_accent";
const char str_tilde[] = "dk_tilde";
const char str_cedilla[] = "dk_cedilla";

uint8_t load_keymap_by_name(char* name)
{
  char* next;
  uint8_t result = 0;
  uint8_t ascii_index;
  uint16_t keycode;
  if(name == NULL)
    return 1;
  if(strcmp(name, default_str) == 0)
    return 0;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/keymaps/%s", name);

  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    result = 2;
    goto load_keymap_end;
  }

  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
  	if(strncmp(read_buffer, "//", 2) == 0)
    {
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_circumflex, strlen(str_circumflex)) == 0)
    {
      circumflex = strtoul(read_buffer + strlen(str_circumflex), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_diaeresis, strlen(str_diaeresis)) == 0)
    {
      diaeresis = strtoul(read_buffer + strlen(str_diaeresis), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_grave_accent, strlen(str_grave_accent)) == 0)
    {
      grave_accent = strtoul(read_buffer + strlen(str_grave_accent), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_acute_accent, strlen(str_acute_accent)) == 0)
    {
      acute_accent = strtoul(read_buffer + strlen(str_acute_accent), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_tilde, strlen(str_tilde)) == 0)
    {
      tilde = strtoul(read_buffer + strlen(str_tilde), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_cedilla, strlen(str_cedilla)) == 0)
    {
      cedilla = strtoul(read_buffer + strlen(str_cedilla), NULL, 0);
      goto read_keymap_loop_end;
    }

    ascii_index = strtoul(read_buffer, &next, 0);
    keycode = strtoul(next, NULL, 0);
    _asciimap[ascii_index] = keycode;

    read_keymap_loop_end:
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  strcpy(curr_kb_layout, name);
  load_keymap_end:
  f_close(&sd_file);
  _asciimap[0] = 0;
  return result;
}

void print_keymap(char* msg)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(15, 5);
  ssd1306_WriteString("Keyboard Layout:", Font_6x10,White);
  ssd1306_SetCursor(10, 20);
  ssd1306_WriteString(msg+5, Font_6x10,White);
  ssd1306_SetCursor(20, 35);
  ssd1306_WriteString("+/- to select", Font_6x10,White);
  ssd1306_SetCursor(15, 50);
  ssd1306_WriteString("Any key to exit", Font_6x10,White);
  ssd1306_UpdateScreen();
}

void select_keymap(void)
{
  memset(temp_buf, 0, PATH_SIZE);
  print_keymap(temp_buf);
  char* keymap_filename;
  fno.lfname = lfn_buf;
  fno.lfsize = FILENAME_SIZE - 1;
  if (f_opendir(&dir, "/keymaps") != FR_OK)
    return;
  
  sprintf(temp_buf, "dpkm_");
  while(1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    keyboard_update();
    if(is_pressed(&button_status[KEY_BUTTON1]) || is_pressed(&button_status[KEY_BUTTON2])) // -
    {
      memset(lfn_buf, 0, FILENAME_SIZE);
      if(f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
        f_readdir(&dir, 0);
      if(fno.fattrib & AM_DIR)
        continue;
      keymap_filename = fno.lfname[0] ? fno.lfname : fno.fname;
      if(strncmp(temp_buf, keymap_filename, strlen(temp_buf)) != 0)
        continue;
      if(strncmp(keymap_filename + strlen(keymap_filename) - 4, ".txt", 4) != 0)
        continue;
      print_keymap(keymap_filename);
      service_all();
    }

    for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
      if(is_pressed(&button_status[i]))
        break;

    osDelay(50);
  }

  service_all();
  f_closedir(&dir);
}

uint8_t command_type, seq_number;
uint8_t hid_tx_buf[HID_TX_BUF_SIZE];

#define HID_COMMAND_GET_INFO 0
#define HID_COMMAND_GOTO_PROFILE 1
#define HID_COMMAND_PREV_PROFILE 2
#define HID_COMMAND_NEXT_PROFILE 3
#define HID_COMMAND_RELOAD_CURRENT_PROFILE 4
#define HID_COMMAND_SW_COLOR 5
#define HID_COMMAND_PRINT_TEXT 6
#define HID_COMMAND_PRINT_BITMAP 7
#define HID_COMMAND_CLEAR_SCREEN 8
#define HID_COMMAND_UPDATE_SCREEN 9
#define HID_COMMAND_LIST_FILES 10
#define HID_COMMAND_READ_FILE 11
#define HID_COMMAND_OP_RESUME 12
#define HID_COMMAND_OP_ABORT 13
#define HID_COMMAND_OPEN_FILE_FOR_WRITING 14
#define HID_COMMAND_WRITE_FILE 15
#define HID_COMMAND_CLOSE_FILE 16
#define HID_COMMAND_DELETE_FILE 17
#define HID_COMMAND_CREATE_DIR 18
#define HID_COMMAND_DELETE_DIR 19
#define HID_COMMAND_SW_RESET 20
#define HID_COMMAND_SLEEP 21


#define HID_RESPONSE_OK 0
#define HID_RESPONSE_ERROR 1
#define HID_RESPONSE_BUSY 2
#define HID_RESPONSE_EOF 3

#define HID_FILE_READ_BUF_SIZE 60
#define HID_TX_DELAY 10

/*
  HID OP RESUME
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 12
  -----------
  duckyPad to PC
  next line/blob of file
  */
uint8_t check_resume(void)
{
  uint32_t start = HAL_GetTick();
  while(hid_rx_has_unprocessed_data == 0)
  {
    if(HAL_GetTick() - start > 2500)
      return 0;
  }
  return hid_rx_buf[2] == HID_COMMAND_OP_RESUME;
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


void handle_hid_command(void)
{
  // hid_rx_buf HID_RX_BUF_SIZE
  // hid_tx_buf HID_TX_BUF_SIZE

  // printf("new data!\n");
  // for (int i = 0; i < HID_RX_BUF_SIZE; ++i)
  //   printf("%c, ", hid_rx_buf[i]);
  // printf("\ndone\n");

  seq_number = hid_rx_buf[1];
  command_type = hid_rx_buf[2];

  memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
  hid_tx_buf[0] = 4;
  hid_tx_buf[1] = seq_number;
  hid_tx_buf[2] = HID_RESPONSE_OK;

  /*
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  if(is_busy)
  {
    hid_tx_buf[2] = HID_RESPONSE_BUSY;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    return;
  }

  /*
  HID GET INFO
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 0
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK
  [3]   firmware version major
  [4]   firmware version minor
  [5]   firmware version patch
  [6]   hardware revision
  [7 - 10]   UUID (uint32_t)
  [11]   current profile
  */
  if(command_type == HID_COMMAND_GET_INFO)
  {
    hid_tx_buf[3] = fw_version_major;
    hid_tx_buf[4] = fw_version_minor;
    hid_tx_buf[5] = fw_version_patch;
    hid_tx_buf[6] = 20;
    uint32_t uuid = get_uuid();
    memcpy(hid_tx_buf + 7, &uuid, 4);
    hid_tx_buf[11] = p_cache.current_profile;
    hid_tx_buf[12] = is_sleeping;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID GOTO PROFILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 1
  [3]   profile number to switch to
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR
  */
  else if(command_type == HID_COMMAND_GOTO_PROFILE)
  {
    if(p_cache.available_profile[hid_rx_buf[3]])
    {
      USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
      is_sleeping = 0;
      oled_full_brightness();
      restore_profile(hid_rx_buf[3]);
    }
    else
    {
      hid_tx_buf[2] = HID_RESPONSE_ERROR;
      USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    }
  }
  /*
  HID PREV PROFILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 2
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK
  */
  else if(command_type == HID_COMMAND_PREV_PROFILE)
  {
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    oled_full_brightness();
    change_profile(PREV_PROFILE);
  }
  /*
  HID NEXT PROFILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 3
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK
  */
  else if(command_type == HID_COMMAND_NEXT_PROFILE)
  {
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    oled_full_brightness();
    change_profile(NEXT_PROFILE);
  }

  /*
  HID LIST FILES
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 10
  [3 ... 63]   starting directory, zero-terminated string, all 0 for root
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY, 3 = EOF
  [3]   file type, 0 = file, 1 = directory
  [4 ... 63] zero-terminated string of file name
  */
  else if(command_type == HID_COMMAND_LIST_FILES)
  {
    char* this_filename;
    fno.lfname = lfn_buf; 
    fno.lfsize = FILENAME_SIZE - 1;
    if (f_opendir(&dir, hid_rx_buf+3) != FR_OK)
      goto list_file_end;
    memset(temp_buf, 0, PATH_SIZE);
    while(1)
    {
      hid_rx_has_unprocessed_data = 0;
      memset(lfn_buf, 0, FILENAME_SIZE);
      memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
      hid_tx_buf[0] = 4;
      hid_tx_buf[1] = seq_number;
      hid_tx_buf[2] = HID_RESPONSE_OK;
      
      if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
        break;
      if (fno.fattrib & AM_DIR)
        hid_tx_buf[3] = 1;
      this_filename = fno.lfname[0] ? fno.lfname : fno.fname;
      strncpy(hid_tx_buf+4, this_filename, FILENAME_SIZE);
      USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
      
      if(check_resume() == 0)
        goto hid_read_file_end;

    }
    list_file_end:
    memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
    hid_tx_buf[0] = 4;
    hid_tx_buf[1] = seq_number;
    hid_tx_buf[2] = HID_RESPONSE_EOF;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    // osDelay(HID_TX_DELAY);
    f_closedir(&dir);
    hid_rx_has_unprocessed_data = 0;
  }
  /*
  HID READ FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 10
  [3 ... 63]   file path, zero-terminated string
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number, xxxincrementing
  [2]   0 = OK, 1 = ERROR, 2 = BUSY, 3 = EOF
  [3 ... 60] file content
  */
  else if(command_type == HID_COMMAND_READ_FILE)
  {
    if(f_open(&sd_file, hid_rx_buf+3, FA_READ) != 0)
      goto hid_read_file_end;

    uint8_t count = 0;
    while(1)
    {
      hid_rx_has_unprocessed_data = 0;
      memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
      hid_tx_buf[0] = 4;
      hid_tx_buf[1] = seq_number + count;
      hid_tx_buf[2] = HID_RESPONSE_OK;
      f_read(&sd_file, read_buffer, HID_FILE_READ_BUF_SIZE, &bytes_read);
      strncpy(hid_tx_buf+3, read_buffer, bytes_read);
      USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);

      if(check_resume() == 0)
        goto hid_read_file_end;

      memset(read_buffer, 0, READ_BUF_SIZE);
      if(bytes_read < HID_FILE_READ_BUF_SIZE)
        break;
      count++;
    }
    hid_read_file_end:
    f_close(&sd_file);
    memset(hid_tx_buf, 0, HID_TX_BUF_SIZE);
    hid_tx_buf[0] = 4;
    hid_tx_buf[1] = seq_number + count;
    hid_tx_buf[2] = HID_RESPONSE_EOF;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    // osDelay(HID_TX_DELAY);
    hid_rx_has_unprocessed_data = 0;
  }
  /*
  HID OPEN FILE FOR WRITING
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 14
  [3 ... 63]   file path, zero-terminated string
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  [3 ... 60] file content
  */
  else if(command_type == HID_COMMAND_OPEN_FILE_FOR_WRITING)
  {
    if(f_open(&sd_file, hid_rx_buf+3, FA_CREATE_ALWAYS | FA_WRITE) != 0)
    {
      hid_tx_buf[2] = HID_RESPONSE_ERROR;
      goto hid_open_for_write_end;
    }

    hid_tx_buf[2] = HID_RESPONSE_OK;
    hid_open_for_write_end:
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID WRITE TO FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 15
  [3 ... 63]   content
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_WRITE_FILE)
  {
    // printf("to write: %s\n", hid_rx_buf+3);
    if(f_write(&sd_file, hid_rx_buf+3, strlen(hid_rx_buf+3), &bytes_read) != 0)
      hid_tx_buf[2] = HID_RESPONSE_ERROR;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID CLOSE FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 16
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_CLOSE_FILE)
  {
    f_close(&sd_file);
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID DELETE FILE
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command
  [3 ... 63]   file name string, zero terminated
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_DELETE_FILE)
  {
    f_close(&sd_file);
    if(f_unlink(hid_rx_buf+3) != 0)
      hid_tx_buf[2] = HID_RESPONSE_ERROR;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID create DIR
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command
  [3 ... 63]   directory name string, zero terminated
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_CREATE_DIR)
  {
    if(f_mkdir(hid_rx_buf+3) != 0)
      hid_tx_buf[2] = HID_RESPONSE_ERROR;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID DELETE DIR
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command
  [3 ... 63]   dir name string, zero terminated
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK, 1 = ERROR, 2 = BUSY
  */
  else if(command_type == HID_COMMAND_DELETE_DIR)
  {
    if(delete_node(hid_rx_buf+3, HID_RX_BUF_SIZE - 3, &fno) != 0)
      hid_tx_buf[2] = HID_RESPONSE_ERROR;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
  /*
  HID SOFTWARE RESET
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK
  */
  else if(command_type == HID_COMMAND_SW_RESET)
  {
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    NVIC_SystemReset();
  }
  /*
  HID SLEEP
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK
  */
  else if(command_type == HID_COMMAND_SLEEP)
  {
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
    start_sleeping();
  }
  /*
    unknown command
    -----------
    duckyPad to PC
    [0]   report_id: always 4
    [1]   seq number
    [2]   1 = ERROR
    */
  else
  {
    hid_tx_buf[2] = HID_RESPONSE_ERROR;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  }
}

void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(16);
  is_busy = 1;
  select_keymap();
  is_busy = 0;
  change_bg();
  service_all();
  keyboard_release_all();
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
    {
      if(is_pressed(&button_status[i]))
      {
        oled_full_brightness(); // OLED back to full brightness

        if(is_sleeping) // wake up from sleep
        {
          restore_profile(p_cache.current_profile);
          is_sleeping = 0;
          goto key_task_end;
        }
        if(i <= KEY_14)
        {
          keydown_anime_start(i);
          if(hold_cache[i].key_type != KEY_TYPE_UNKNOWN && hold_cache[i].code != 0)
          {
            keyboard_press(&hold_cache[i], 0);
            osDelay(DEFAULT_CHAR_DELAY_MS);
            if(hold_cache2[i].key_type != KEY_TYPE_UNKNOWN && hold_cache2[i].code != 0)
            {
              keyboard_press(&hold_cache2[i], 0);
              osDelay(DEFAULT_CHAR_DELAY_MS);
            }
          }
          else
          {
            is_busy = 1;
            handle_keypress(i, &button_status[i]); // handle the button state inside here for repeats
            keydown_anime_end(i);
            if(my_dpc.type == DPC_SLEEP)
            {
              start_sleeping();
              dpc_init(&my_dpc);
            }
            if(my_dpc.type == DPC_PREV_PROFILE)
            {
              change_profile(PREV_PROFILE);
              dpc_init(&my_dpc);
            }
            if(my_dpc.type == DPC_NEXT_PROFILE)
            {
              change_profile(NEXT_PROFILE);
              dpc_init(&my_dpc);
            }
            if(my_dpc.type == DPC_GOTO_PROFILE)
            {
              if(p_cache.available_profile[my_dpc.data])
                restore_profile(my_dpc.data);
              dpc_init(&my_dpc);
            }
            is_busy = 0;
          }
        }
        else if(i == KEY_BUTTON1 || i == KEY_BUTTON2)
          handle_tactile_button_press(i);
      }
      if(is_released_but_not_serviced(&button_status[i]) && hold_cache[i].key_type != KEY_TYPE_UNKNOWN && hold_cache[i].code != 0)
      {
        last_keypress = HAL_GetTick();
        keyboard_release(&hold_cache[i]);
        osDelay(DEFAULT_CHAR_DELAY_MS);
        if(hold_cache2[i].key_type != KEY_TYPE_UNKNOWN && hold_cache2[i].code != 0)
        {
          keyboard_release(&hold_cache2[i]);
          osDelay(DEFAULT_CHAR_DELAY_MS);
        }
        keydown_anime_end(i);
      }
      key_task_end:
      service_press(&button_status[i]);
    } 
    osDelay(16);
  }
}

void start_sleeping(void)
{
  neopixel_off();
  osDelay(66);
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  ssd1306_UpdateScreen();
  ssd1306_UpdateScreen();
  is_sleeping = 1;
}

void animation_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(20);
  anime_init();
  for(;;)
  {
    osDelay(20);
    led_animation_handler();

    if(hid_rx_has_unprocessed_data)
    {
      handle_hid_command();
      hid_rx_has_unprocessed_data = 0;
      memset(hid_rx_buf, 0, HID_RX_BUF_SIZE);
    }

    if(is_sleeping)
      continue;

    if(dp_settings.sleep_after_ms != 0 && HAL_GetTick() - last_keypress > dp_settings.sleep_after_ms)
      start_sleeping();
    // dim OLED screen after 5 minutes of idle to prevent burn-in
    if(HAL_GetTick() - last_keypress > 300000)
      ssd1306_dim(1);
    // shift pixels around every 2 minutes to prevent burn-in
    if(is_busy == 0 && HAL_GetTick() > next_pixel_shift)
    {
      if(has_valid_profiles)
        print_legend(rand()%3-1, rand()%3-1); // -1 to 1
      next_pixel_shift = HAL_GetTick() + 120000;
    }
    
  }
}
