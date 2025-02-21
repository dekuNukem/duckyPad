#ifndef __PROFILES_H
#define __PROFILES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#include "input_task.h"
#include "neopixel.h"
#include "fatfs.h"

#define FILENAME_BUFSIZE 64
#define MAX_PROFILES 32
#define KEYNAME_SIZE 8
#define PROFILE_NAME_MAX_LEN 16

typedef struct
{
  uint8_t brightness_index;
  uint8_t sleep_index;
  uint8_t last_used_profile;
  char current_kb_layout[FILENAME_BUFSIZE];
} dp_global_settings;

typedef struct
{
  char sw_name[MECH_OBSW_COUNT][KEYNAME_SIZE];
  uint8_t sw_color[MECH_OBSW_COUNT][THREE]; // only mechanical keys have RGB LEDs
  uint8_t sw_activation_color[MECH_OBSW_COUNT][THREE];
  uint8_t keypress_count[MAX_TOTAL_SW_COUNT];
  uint8_t dim_unused_keys;
  uint8_t is_landscape;
} profile_cache;

extern profile_cache curr_pf_info;

extern dp_global_settings dp_settings;
extern uint8_t current_profile_number;
extern char filename_buf[FILENAME_BUFSIZE];
extern char profile_name_list[MAX_PROFILES][PROFILE_NAME_MAX_LEN];

uint8_t mount_sd(void);
uint8_t scan_profiles(void);
uint8_t load_profile(uint8_t profile_number);
void goto_profile(uint8_t profile_number);
void goto_prev_profile(void);
void goto_next_profile(void);

uint8_t load_settings(dp_global_settings* dps);
uint8_t save_settings(dp_global_settings* dps);


#define PROFILE_SCAN_OK 0
#define PROFILE_SCAN_ERROR_NO_TOC 1
#define PROFILE_SCAN_ERROR_NO_PROFILE 2

extern FRESULT sd_fresult;
extern FATFS sd_fs;
extern FIL sd_file;
extern DIR dir;
extern FILINFO fno;

#ifdef __cplusplus
}
#endif

#endif


