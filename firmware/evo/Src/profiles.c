#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "shared.h"
#include "profiles.h"
#include "fatfs.h"
#include "ui_task.h"
#include "ds_vm.h"
#include "keyboard.h"

const char settings_file_path[] = "/dpp_config.txt";
const char default_settings_file[] = "sleep_index 0\nbrightness_index 0\nlast_profile 1\nfw_ver 0.0.0\nkb_layout dpkm_English (US).txt\n";
dp_global_settings dp_settings;

uint8_t current_profile_number;
profile_cache curr_pf_info;
char profile_name_list[MAX_PROFILES][PROFILE_NAME_MAX_LEN];
char lfn_buf[FILENAME_BUFSIZE];

const char config_sleep_after_index[] = "sleep_index ";
const char config_brightness_index[] = "brightness_index ";
const char config_keyboard_layout[] = "kb_layout ";
const char config_last_used_profile[] = "last_profile ";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS 0";

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

  CLEAR_TEMP_BUF();
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

const char* profile_info_file_path = "/profile_info.txt";

/*
  Reads from profile_info.txt, load profile names into profile_name_list
*/
uint8_t scan_profiles(void)
{
  char *this_profile_name;
  uint8_t this_profile_number;
  uint8_t valid_profile_count = 0;
  
  if(f_open(&sd_file, profile_info_file_path, FA_READ))
    return PROFILE_SCAN_ERROR_NO_TOC;
  memset(profile_name_list, 0, sizeof(profile_name_list));
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    this_profile_number = atoi(read_buffer);
    if(this_profile_number >= MAX_PROFILES)
      continue;
    this_profile_name = goto_next_arg(read_buffer, read_buffer + strlen(read_buffer));
    strip_newline(this_profile_name, PROFILE_NAME_MAX_LEN);
    CLEAR_TEMP_BUF();
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
  if(this_line == NULL || strlen(this_line) <= 2)
    return;

  char* msg_end = this_line + strlen(this_line);

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
}

const char* on_release_dsb_suffix = "-release.dsb";
const char* dsb_extension = ".dsb";

void load_dsb_exists_cache(char* profile_dir_path)
{
  if(f_opendir(&dir, profile_dir_path))
    return;
  
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_BUFSIZE - 1;
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    sd_fresult = f_readdir(&dir, &fno);
    if (sd_fresult != FR_OK || fno.fname[0] == 0)
      break;
    char* file_name = fno.lfname[0] ? fno.lfname : fno.fname;
  
    // file name starts with "key"
    if(strncmp(file_name, "key", 3))
      continue;
    char* dash_start = strrchr(file_name, '-');
    char* dot_start = strrchr(file_name, '.');
    // key4-release.dsb
    if(dash_start != NULL && strncmp(dash_start, on_release_dsb_suffix, strlen(on_release_dsb_suffix)) == 0)
    {
      uint8_t this_key_number = atoi(file_name+3) - 1;
      if(this_key_number <= MAX_TOTAL_SW_COUNT)
        curr_pf_info.dsb_exists[this_key_number] |= DSB_ON_RELEASE_EXISTS;
    }
    // key4.dsb
    else if(dot_start != NULL && strncmp(dot_start, dsb_extension, strlen(dsb_extension)) == 0)
    {
      uint8_t this_key_number = atoi(file_name+3) - 1;
      if(this_key_number <= MAX_TOTAL_SW_COUNT)
        curr_pf_info.dsb_exists[this_key_number] |= DSB_ON_PRESS_EXISTS;
    }
  }
  f_closedir(&dir);
}

// fill up the current profile cache
uint8_t load_profile(uint8_t profile_number)
{
  if(profile_number >= MAX_PROFILES)
    return 1;

  memset(&curr_pf_info, 0, sizeof(curr_pf_info));

  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/profile_%s", profile_name_list[profile_number]);
  load_dsb_exists_cache(temp_buf);
  
  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/profile_%s/config.txt", profile_name_list[profile_number]);
  if(f_open(&sd_file, temp_buf, FA_READ))
    return 2;

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
  current_profile_number = profile_number;
  draw_current_profile();
  save_settings(&dp_settings);
  return 0;
}

void goto_profile(uint8_t profile_number)
{
  if(goto_profile_without_updating_rgb_LED(profile_number))
    return;
  if(load_persistent_state())
    redraw_bg();
  else
    neopixel_draw_current_buffer();
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
  
  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/profile_%s/state_dpp.sps", profile_name_list[current_profile_number]);

  f_open(&sd_file, temp_buf, FA_WRITE | FA_CREATE_ALWAYS);
  f_write(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_written);
  f_close(&sd_file);
}

uint8_t load_persistent_state(void)
{
  CLEAR_TEMP_BUF();
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
  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/gv.sps");
  f_open(&sd_file, temp_buf, FA_WRITE | FA_CREATE_ALWAYS);
  f_write(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_written);
  f_close(&sd_file);
}

void load_gv(void)
{
  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/gv.sps");
  f_open(&sd_file, temp_buf, FA_READ);
  memset(sps_bin_buf, 0, SPS_BIN_SIZE);
  f_read(&sd_file, sps_bin_buf, SPS_BIN_SIZE, &bytes_read);
  f_close(&sd_file);
  memcpy(gv_buf, sps_bin_buf, GLOBAL_VARIABLE_COUNT * sizeof(uint16_t));
}

const char* dk_circumflex = "dk_circumflex";
const char* dk_diaeresis = "dk_diaeresis";
const char* dk_grave_accent = "dk_grave_accent";
const char* dk_acute_accent = "dk_acute_accent";
const char* dk_tilde = "dk_tilde";
const char* dk_cedilla = "dk_cedilla";

uint8_t load_keymap_by_name(char* km_name)
{
  char* next;
  if(km_name == NULL)
    return 1;
  CLEAR_TEMP_BUF();
  sprintf(temp_buf, "/keymaps/%s", km_name);

  if(f_open(&sd_file, temp_buf, FA_READ))
    return ERROR_KEYMAP_NOT_FOUND;
  
  while(f_gets(temp_buf, TEMP_BUFSIZE, &sd_file))
  {
    if(strncmp(temp_buf, dk_circumflex, strlen(dk_circumflex)) == 0)
      duckcode_circumflex = strtoul(temp_buf + strlen(dk_circumflex), NULL, 0);
    else if(strncmp(temp_buf, dk_diaeresis, strlen(dk_diaeresis)) == 0)
      duckcode_diaeresis = strtoul(temp_buf + strlen(dk_diaeresis), NULL, 0);
    else if(strncmp(temp_buf, dk_grave_accent, strlen(dk_grave_accent)) == 0)
      duckcode_grave_accent = strtoul(temp_buf + strlen(dk_grave_accent), NULL, 0);
    else if(strncmp(temp_buf, dk_acute_accent, strlen(dk_acute_accent)) == 0)
      duckcode_acute_accent = strtoul(temp_buf + strlen(dk_acute_accent), NULL, 0);
    else if(strncmp(temp_buf, dk_tilde, strlen(dk_tilde)) == 0)
      duckcode_tilde = strtoul(temp_buf + strlen(dk_tilde), NULL, 0);
    else if(strncmp(temp_buf, dk_cedilla, strlen(dk_cedilla)) == 0)
      duckcode_cedilla = strtoul(temp_buf + strlen(dk_cedilla), NULL, 0);

    uint8_t ascii_index = strtoul(temp_buf, &next, 0);
    uint16_t keycode = strtoul(next, NULL, 0);
    ascii_map[ascii_index] = keycode;
  }
  f_close(&sd_file);
  ascii_map[0] = 0;
  return 0;
}

uint8_t get_next_keymap(const char* current_keymap_filename, char* next_keymap_filename)
{
  if(f_opendir(&dir, "/keymaps"))
    return ERROR_NO_KEYMAP_FOLDER;
  uint8_t found = 0;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_BUFSIZE - 1;
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    sd_fresult = f_readdir(&dir, &fno);
    if (sd_fresult != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
      continue;
    char* file_name = fno.lfname[0] ? fno.lfname : fno.fname;
    if(!(strncmp(file_name, "dpkm_", 5) == 0 && strstr(file_name, ".txt") != NULL))
      continue;
    if(found)
    {
      strcpy(next_keymap_filename, file_name);
      f_closedir(&dir);
      return 0;
    }
    if(strcmp(file_name, current_keymap_filename) == 0)
      found = 1;
  }
  f_closedir(&dir);
  return ERROR_KEYMAP_NOT_FOUND;
}

uint8_t get_first_keymap(char* keymap_filename)
{
  if(f_opendir(&dir, "/keymaps"))
    return ERROR_NO_KEYMAP_FOLDER;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_BUFSIZE - 1;
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    sd_fresult = f_readdir(&dir, &fno);
    if (sd_fresult != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
      continue;
    char* file_name = fno.lfname[0] ? fno.lfname : fno.fname;
    if(!(strncmp(file_name, "dpkm_", 5) == 0 && strstr(file_name, ".txt") != NULL))
      continue;
    strcpy(keymap_filename, file_name);
    f_closedir(&dir);
    return 0;
  }
  f_closedir(&dir);
  return ERROR_KEYMAP_NOT_FOUND;
}

void profile_init(void)
{
  if(is_valid_profile_number(dp_settings.last_used_profile))
    goto_profile(dp_settings.last_used_profile);
  else
    goto_next_profile();
}

const char* profile_str = "profile";

uint8_t ensure_new_profile_format(void)
{
  char* this_profile_name;
  uint8_t this_profile_number;

  if(f_stat(profile_info_file_path, NULL) == 0)
    return 1;
  if(f_opendir(&dir, "/"))
    return 2;
  oled_say("Converting...");
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_BUFSIZE - 1;
  while (1)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    sd_fresult = f_readdir(&dir, &fno);
    if (sd_fresult != FR_OK || fno.fname[0] == 0)
      break;
    if(!(fno.fattrib & AM_DIR))
     continue;
    char* dir_name = fno.lfname[0] ? fno.lfname : fno.fname;
    if(strncmp(dir_name, profile_str, strlen(profile_str)) != 0)
      continue;
    
    this_profile_number = atoi(dir_name + strlen(profile_str));
    if(this_profile_number >= MAX_PROFILES)
      continue;

    this_profile_name = strstr(dir_name, "_");
    if(this_profile_name == NULL)
      continue;
    this_profile_name++;
    memset(profile_name_list[this_profile_number], 0, PROFILE_NAME_MAX_LEN);
    strncpy(profile_name_list[this_profile_number], this_profile_name, PROFILE_NAME_MAX_LEN);
  }
  f_closedir(&dir);

  if(f_open(&sd_file, profile_info_file_path, FA_WRITE | FA_CREATE_ALWAYS))
    return 4;

  for (size_t i = 0; i < MAX_PROFILES; i++)
  {
    if(strlen(profile_name_list[i]) == 0)
      continue;
    f_printf(&sd_file, "%d %s\n", i, profile_name_list[i]);
  }
  f_close(&sd_file);

  for (size_t i = 0; i < MAX_PROFILES; i++)
  {
    if(strlen(profile_name_list[i]) == 0)
      continue;
    CLEAR_TEMP_BUF();
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    sprintf(temp_buf, "/profile%d_%s\n", i, profile_name_list[i]);
    sprintf(lfn_buf, "/profile_%s\n", profile_name_list[i]);
    f_rename(temp_buf, lfn_buf);
  }
  oled_say("Done!");
  return 0;
}

