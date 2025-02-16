#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "shared.h"
#include "profiles.h"
#include "fatfs.h"
#include "ui_task.h"

const char settings_file_path[] = "dpp_config.txt";
const char default_settings_file[] = "sleep_index 0\nbrightness_index 0\nlast_profile 1\nfw_ver 0.0.0\nserial_number DP20_000000\nkb_layout dpkm_English (US).txt\n";
dp_global_settings dp_settings;

uint8_t current_profile_number;
profile_cache curr_pf_info;
char profile_name_list[MAX_PROFILES][PROFILE_NAME_MAX_LEN];

const char config_sleep_after_index[] = "sleep_index ";
const char config_brightness_index[] = "brightness_index ";
const char config_keyboard_layout[] = "kb_layout ";
const char config_last_used_profile[] = "last_profile ";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS 0";
const char cmd_IS_LANDSCAPE[] = "IS_LANDSCAPE 1";

FRESULT sd_fresult;
FATFS sd_fs;
FIL sd_file;
DIR dir;
FILINFO fno;

uint8_t is_valid_profile_number(uint8_t profile_number)
{
  if(profile_number >= MAX_PROFILES || strlen(profile_name_list[profile_number]) == 0)
    return 0;
  return 1;
}

uint8_t mount_sd(void)
{
  return f_mount(&sd_fs, "", 1);
}

/*
  Reads from profile_info.txt, load profile names into profile_name_list
*/
uint8_t scan_profiles(void)
{
  char *this_profile_name;
  uint8_t this_profile_number;
  uint8_t valid_profile_count = 0;
  
  if(f_open(&sd_file, "/profile_info.txt", FA_READ))
    return PROFILE_SCAN_ERROR_NO_TOC;
  memset(profile_name_list, 0, sizeof(profile_name_list));
  while(f_gets(temp_buf, TEMP_BUFSIZE, &sd_file) != NULL)
  {
    this_profile_number = atoi(temp_buf);
    if(this_profile_number >= MAX_PROFILES)
      continue;
    this_profile_name = goto_next_arg(temp_buf, temp_buf + strlen(temp_buf));
    strip_newline(this_profile_name, PROFILE_NAME_MAX_LEN);
    memset(temp_buf, 0, TEMP_BUFSIZE);
    sprintf(temp_buf, "/profile_%s/config.txt", this_profile_name);
    if(f_stat(temp_buf, &fno)) // profile does not exist
      continue;
    memset(profile_name_list[this_profile_number], 0, PROFILE_NAME_MAX_LEN);
    strncpy(profile_name_list[this_profile_number], this_profile_name, PROFILE_NAME_MAX_LEN);
    valid_profile_count++;
  }
  f_close(&sd_file);
  if(valid_profile_count == 0)
    return PROFILE_SCAN_ERROR_NO_PROFILE;
  return PROFILE_SCAN_OK;
}


const char cmd_sw_name_firstline[] = "z";
void parse_profile_config_line(char* this_line, profile_cache* this_profile)
{
  char* msg_end = this_line + strlen(this_line);

  if(this_line == NULL || strlen(this_line) <= 2)
    return;

  if(strncmp(cmd_sw_name_firstline, this_line, strlen(cmd_sw_name_firstline)) == 0)
  {
    uint8_t this_key_index = atoi(this_line + strlen(cmd_sw_name_firstline));
    if(this_key_index == 0)
      return;
    this_key_index--;
    if(this_key_index >= MECH_OBSW_COUNT)
      return;
    memset(this_profile->sw_name[this_key_index], 0, KEYNAME_SIZE);
    char* kn_start = goto_next_arg(this_line, msg_end);
    if(kn_start == NULL)
      return;
    strncpy(this_profile->sw_name[this_key_index], kn_start, KEYNAME_SIZE);
  }
  else if(strncmp(cmd_BG_COLOR, this_line, strlen(cmd_BG_COLOR)) == 0) // order is important! BG, SW, and KD
  {
    char* curr = goto_next_arg(this_line, msg_end);
    uint8_t rrr = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t ggg = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t bbb = atoi(curr);
    for (size_t i = 0; i < MECH_OBSW_COUNT; i++)
    {
      this_profile->sw_color[i][0] = rrr;
      this_profile->sw_color[i][1] = ggg;
      this_profile->sw_color[i][2] = bbb;
      this_profile->sw_activation_color[i][0] = 255 - rrr;
      this_profile->sw_activation_color[i][1] = 255 - ggg;
      this_profile->sw_activation_color[i][2] = 255 - bbb;
    }
  }
  else if(strncmp(cmd_SWCOLOR, this_line, strlen(cmd_SWCOLOR)) == 0)
  {
    char* curr = this_line + strlen(cmd_SWCOLOR);
    uint8_t sw_index = atoi(curr) - 1;
    if(sw_index >= MECH_OBSW_COUNT)
      return;
    curr = goto_next_arg(curr, msg_end);
    uint8_t rrr = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t ggg = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t bbb = atoi(curr);
    this_profile->sw_color[sw_index][0] = rrr;
    this_profile->sw_color[sw_index][1] = ggg;
    this_profile->sw_color[sw_index][2] = bbb;
    this_profile->sw_activation_color[sw_index][0] = 255 - rrr;
    this_profile->sw_activation_color[sw_index][1] = 255 - ggg;
    this_profile->sw_activation_color[sw_index][2] = 255 - bbb;
  }
  else if(strncmp(cmd_KD_COLOR, this_line, strlen(cmd_KD_COLOR)) == 0)
  {
    char* curr = goto_next_arg(this_line, msg_end);
    uint8_t rrr = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t ggg = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t bbb = atoi(curr);
    for (size_t i = 0; i < MECH_OBSW_COUNT; i++)
    {
      this_profile->sw_activation_color[i][0] = rrr;
      this_profile->sw_activation_color[i][1] = ggg;
      this_profile->sw_activation_color[i][2] = bbb;
    }
  }
  else if(strncmp(cmd_DIM_UNUSED_KEYS, this_line, strlen(cmd_DIM_UNUSED_KEYS)) == 0)
  {
    this_profile->dim_unused_keys = 0;
  }
  else if(strncmp(cmd_IS_LANDSCAPE, this_line, strlen(cmd_IS_LANDSCAPE)) == 0)
  {
    this_profile->is_landscape = 1;
  }
}

// fill up the current profile cache
uint8_t load_profile(uint8_t profile_number)
{
  if(profile_number >= MAX_PROFILES)
    return 1;

  memset(temp_buf, 0, TEMP_BUFSIZE);
  sprintf(temp_buf, "/profile_%s/config.txt", profile_name_list[profile_number]);
  if(f_open(&sd_file, temp_buf, FA_READ))
    return 2;

  memset(&curr_pf_info, 0, sizeof(curr_pf_info));
  curr_pf_info.dim_unused_keys = 1;
  memset(temp_buf, 0, TEMP_BUFSIZE);
  while(f_gets(temp_buf, TEMP_BUFSIZE, &sd_file) != NULL)
  {
    strip_newline(temp_buf, TEMP_BUFSIZE);
    parse_profile_config_line(temp_buf, &curr_pf_info);
    memset(temp_buf, 0, TEMP_BUFSIZE);
  }
  f_close(&sd_file);
  if(curr_pf_info.dim_unused_keys)
  {
    for (size_t i = 0; i < MECH_OBSW_COUNT; i++)
    {
      if(strlen(curr_pf_info.sw_name[i]) > 0)
        continue;
      curr_pf_info.sw_color[i][0] = 0;
      curr_pf_info.sw_color[i][1] = 0;
      curr_pf_info.sw_color[i][2] = 0;
      curr_pf_info.sw_activation_color[i][0] = 0;
      curr_pf_info.sw_activation_color[i][1] = 0;
      curr_pf_info.sw_activation_color[i][2] = 0;
    }
  }
  return 0;
}

void goto_profile(uint8_t profile_number)
{
  if(load_profile(profile_number))
    return;
  current_profile_number = profile_number;
  draw_current_profile();
}

void goto_next_profile(void)
{
  uint8_t new_profile_number = current_profile_number;
  while(1)
  {
    new_profile_number++;
    if(new_profile_number >= MAX_PROFILES)
      new_profile_number = 0;
    if(strlen(profile_name_list[new_profile_number]))
      break;
  }
  goto_profile(new_profile_number);
}

void goto_prev_profile(void)
{
  uint8_t new_profile_number = current_profile_number;
  while(1)
  {
    if(new_profile_number == 0)
      new_profile_number = MAX_PROFILES - 1;
    else
      new_profile_number--;
    if(strlen(profile_name_list[new_profile_number]))
      break;
  }
  goto_profile(new_profile_number);
}
