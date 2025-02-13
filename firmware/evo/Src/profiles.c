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
char profile_name_list[MAX_PROFILES][PROFILE_NAME_MAX_LEN];

const char config_sleep_after_index[] = "sleep_index ";
const char config_brightness_index[] = "brightness_index ";
const char config_keyboard_layout[] = "kb_layout ";
const char config_last_used_profile[] = "last_profile ";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS 0";

uint8_t current_profile_number;
profile_cache curr_pf_info;

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

/*
  Reads from profile_info.txt, load profile names into profile_name_list
*/
uint8_t load_profile_name(void)
{
  char *this_profile_name;
  uint8_t this_profile_number;
  uint8_t has_valid_profile = 0;
  
  if(f_open(&sd_file, "/profile_info.txt", FA_READ) != FR_OK)
    return PROFILE_SCAN_ERROR_NO_TOC;
  memset(profile_name_list, 0, sizeof(profile_name_list));
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    this_profile_number = atoi(read_buffer);
    if(this_profile_number >= MAX_PROFILES)
      continue;
    this_profile_name = goto_next_arg(read_buffer, read_buffer + strlen(read_buffer));
    strip_newline(this_profile_name, PROFILE_NAME_MAX_LEN);
    memset(temp_buf, 0, TEMP_BUFSIZE);
    sprintf(temp_buf, "/profile_%s/config.txt", this_profile_name);
    if(f_stat(temp_buf, &fno)) // profile does not exist
      continue;
    memset(profile_name_list[this_profile_number], 0, PROFILE_NAME_MAX_LEN);
    strncpy(profile_name_list[this_profile_number], this_profile_name, PROFILE_NAME_MAX_LEN);
    has_valid_profile++;
  }
  f_close(&sd_file);
  for (size_t i = 0; i < PROFILE_NAME_MAX_LEN; i++)
  {
    printf("%d %s\n", i, profile_name_list[i]);
  }
  return PROFILE_SCAN_OK;
}

void load_profile_config(uint8_t profile_number)
{
  ;
}


// uint8_t scan_profiles(void)
// {
//   memset(profile_name_list, 0, sizeof(profile_name_list));
//   load_profile_name();
//   uint8_t has_valid_profile = 0;
//   for (size_t i = 0; i < MAX_PROFILES; i++)
//     has_valid_profile += strlen(profile_name_list[i]);
//   if(has_valid_profile == 0)
//     return PSCAN_ERROR_NO_PROFILE;
//   load_profile_config(0);
//   return PSCAN_OK;
// }

