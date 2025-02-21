#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "shared.h"
#include "profiles.h"
#include "fatfs.h"
#include "ui_task.h"
#include "ds_vm.h"

const char settings_file_path[] = "dpp_config.txt";
const char default_settings_file[] = "sleep_index 0\nbrightness_index 0\nlast_profile 1\nfw_ver 0.0.0\nkb_layout dpkm_English (US).txt\n";
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
UINT bytes_written, bytes_read;

uint8_t load_settings(dp_global_settings* dps)
{
  if(dps == NULL)
    return 1;

  memset(dps, 0, sizeof(*dps));

  if(f_stat(settings_file_path, &fno))
  {
    if(f_open(&sd_file, settings_file_path, FA_CREATE_ALWAYS | FA_WRITE))
      return 2;
    f_printf(&sd_file, "%s", default_settings_file);
    f_close(&sd_file);
  }
  
  if(f_open(&sd_file, settings_file_path, FA_READ))
    return 3;

  memset(temp_buf, 0, TEMP_BUFSIZE);
  memset(dps->current_kb_layout, 0, FILENAME_BUFSIZE);

  while(f_gets(temp_buf, TEMP_BUFSIZE, &sd_file))
  {
    if(strncmp(temp_buf, config_sleep_after_index, strlen(config_sleep_after_index)) == 0)
      dps->sleep_index = atoi(temp_buf + strlen(config_sleep_after_index));
    if(strncmp(temp_buf, config_brightness_index, strlen(config_brightness_index)) == 0)
      dps->brightness_index = atoi(temp_buf + strlen(config_brightness_index));
    if(strncmp(temp_buf, config_last_used_profile, strlen(config_last_used_profile)) == 0)
      dps->last_used_profile = atoi(temp_buf + strlen(config_last_used_profile));
    if(dps->brightness_index >= BRIGHTNESS_LEVEL_SIZE)
      dps->brightness_index = BRIGHTNESS_LEVEL_SIZE - 1;
    if(strncmp(temp_buf, config_keyboard_layout, strlen(config_keyboard_layout)) == 0)
    {
      strcpy(dps->current_kb_layout, temp_buf + strlen(config_keyboard_layout));
      strip_newline(dps->current_kb_layout, FILENAME_BUFSIZE);
    }
  }
  f_close(&sd_file);
  return 0;
}

uint8_t save_settings(dp_global_settings* dps)
{
  if(dps == NULL)
    return 1;
  if(f_open(&sd_file, settings_file_path, FA_WRITE | FA_CREATE_ALWAYS))
    return 2;
  f_printf(
    &sd_file,
    "%s%d\n"
    "%s%d\n"
    "%s%d\n"
    "fw_ver %d.%d.%d\n"
    "%s%s\n",
    config_sleep_after_index, dps->sleep_index,
    config_brightness_index, dps->brightness_index,
    config_last_used_profile, current_profile_number,
    fw_version_major, fw_version_minor, fw_version_patch,
    config_keyboard_layout, dps->current_kb_layout
  );
  f_close(&sd_file);
  return 0;
}

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
  memset(read_buffer, 0, READ_BUF_SIZE);
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    strip_newline(read_buffer, READ_BUF_SIZE);
    parse_profile_config_line(read_buffer, &curr_pf_info);
    memset(read_buffer, 0, READ_BUF_SIZE);
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

uint8_t goto_profile_without_updating_rgb_LED(uint8_t profile_number)
{
  if(load_profile(profile_number))
    return 1;
  draw_current_profile();
  current_profile_number = profile_number;
  save_settings(&dp_settings);
  return 0;
}

void goto_profile(uint8_t profile_number)
{
  if(goto_profile_without_updating_rgb_LED(profile_number))
    return;
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

#define COLOR_START_ADDR MAX_TOTAL_SW_COUNT
#define SPS_BIN_SIZE 256
uint8_t sps_bin_buf[SPS_BIN_SIZE];

void save_persistent_state(uint8_t epilogue_value, uint8_t swid)
{
  memset(sps_bin_buf, 0, SPS_BIN_SIZE);
  memcpy(sps_bin_buf, curr_pf_info.keypress_count, MAX_TOTAL_SW_COUNT);
  for (uint8_t i = 0; i < NEOPIXEL_COUNT; i++)
  {
    uint8_t r_addr = i*3 + COLOR_START_ADDR;
    uint8_t g_addr = r_addr + 1;
    uint8_t b_addr = g_addr + 1;
    uint8_t red, green, blue;
    get_current_color(i, &red, &green, &blue);
    // if not asking to save color state, save the current key color with assigned switch color, instead of keydown color
    if((epilogue_value & EPILOGUE_SAVE_COLOR_STATE) == 0 && i == swid)
    {
      red = curr_pf_info.sw_color[i][0];
      green = curr_pf_info.sw_color[i][1];
      blue = curr_pf_info.sw_color[i][2];
    }
    sps_bin_buf[r_addr] = red;
    sps_bin_buf[g_addr] = green;
    sps_bin_buf[b_addr] = blue;
  }
  
  memset(temp_buf, 0, TEMP_BUFSIZE);
  sprintf(temp_buf, "/profile_%s/state_dpp.sps", profile_name_list[current_profile_number]);

  f_open(&sd_file, temp_buf, FA_WRITE | FA_CREATE_ALWAYS);
  f_write(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_written);
  f_close(&sd_file);
}

uint8_t load_persistent_state(void)
{
  memset(temp_buf, 0, TEMP_BUFSIZE);
  sprintf(temp_buf, "/profile_%s/state_dpp.sps", profile_name_list[current_profile_number]);

  if(f_open(&sd_file, temp_buf, FA_READ))
    return 1;
  memset(sps_bin_buf, 0, SPS_BIN_SIZE);
  if(f_read(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_read))
    return 2;
  f_close(&sd_file);
  memcpy(curr_pf_info.keypress_count, sps_bin_buf, MAX_TOTAL_SW_COUNT);

  for (uint8_t i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    uint8_t r_addr = i*3 + COLOR_START_ADDR;
    uint8_t red = sps_bin_buf[r_addr];
    uint8_t green = sps_bin_buf[r_addr+1];
    uint8_t blue = sps_bin_buf[r_addr+2];
    set_pixel_3color_update_buffer(i, red, green, blue);
  }
  return 0;
}

void save_gv(void)
{
  memset(sps_bin_buf, 0, SPS_BIN_SIZE);
  memcpy(sps_bin_buf, gv_buf, GLOBAL_VARIABLE_COUNT*sizeof(uint16_t));
  memset(temp_buf, 0, TEMP_BUFSIZE);
  sprintf(temp_buf, "/gv.sps");
  f_open(&sd_file, temp_buf, FA_WRITE | FA_CREATE_ALWAYS);
  f_write(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_written);
  f_close(&sd_file);
}

void load_gv(void)
{
  memset(temp_buf, 0, TEMP_BUFSIZE);
  sprintf(temp_buf, "/gv.sps");
  f_open(&sd_file, temp_buf, FA_READ);
  memset(sps_bin_buf, 0, SPS_BIN_SIZE);
  f_read(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_read);
  f_close(&sd_file);
  memcpy(gv_buf, sps_bin_buf, GLOBAL_VARIABLE_COUNT * sizeof(uint16_t));
}
