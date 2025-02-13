#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "shared.h"
#include "profiles.h"
#include "fatfs.h"


const char settings_file_path[] = "dpp_config.txt";
const char default_settings_file[] = "sleep_index 0\nbrightness_index 0\nlast_profile 1\nfw_ver 0.0.0\nserial_number DP20_000000\nkb_layout dpkm_English (US).txt\n";
dp_global_settings dp_settings;

char temp_buf[TEMP_BUFSIZE];
char filename_buf[FILENAME_BUFSIZE];

const char config_sleep_after_index[] = "sleep_index ";
const char config_brightness_index[] = "brightness_index ";
const char config_keyboard_layout[] = "kb_layout ";
const char config_last_used_profile[] = "last_profile ";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS 0";

uint8_t is_profile_load_complete;
uint8_t current_profile_number;
profile_cache p_cache;

FRESULT sd_fresult;
FATFS sd_fs;
FIL sd_file;
DIR dir;
FILINFO fno;
char lfn_buf[FILENAME_BUFSIZE];

#define READ_BUF_SIZE 256
char read_buffer[READ_BUF_SIZE];

uint8_t mount_sd(void)
{
  return f_mount(&sd_fs, "", 1);
}

const char* profile_dir_prefix = "profile";
uint8_t is_profile_dir(char* dirname)
{
  if(strncmp(dirname, profile_dir_prefix, strlen(profile_dir_prefix)))
    return 0;
  if(strlen(dirname) == strlen(profile_dir_prefix))
    return 0;
  if(strlen(dirname) >= FILENAME_BUFSIZE)
    return 0;
  char pnumber = dirname[strlen(profile_dir_prefix)];
  if(pnumber > '9' || pnumber < '0')
    return 0;
  if(strstr(dirname, "_") == 0)
    return 0;
  return 1;
}

void load_profile_info(void)
{
  char *this_profile_name;
  uint8_t this_profile_number;
  if(f_open(&sd_file, "/profile_info.txt", FA_READ) != FR_OK)
    return;

  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    this_profile_number = atoi(read_buffer);
    if(this_profile_number >= MAX_PROFILES)
      continue;
    this_profile_name = goto_next_arg(read_buffer, read_buffer + strlen(read_buffer));
    printf("%d %s\n", this_profile_number, this_profile_name);
    memset(temp_buf, 0, TEMP_BUFSIZE);
    sprintf(temp_buf, "/profile_%s", this_profile_name);
    strip_newline(temp_buf, TEMP_BUFSIZE);
    printf("%d %s\n", this_profile_number, temp_buf);
    // memset(all_profile_info[this_profile_number].dir_path, 0, FILENAME_BUFSIZE);
    // strcpy(all_profile_info[this_profile_number].dir_path, this_profile_name);
  }
  f_close(&sd_file);
}



