#ifndef __PROFILES_H
#define __PROFILES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#include "input_task.h"
#include "neopixel.h"

#define TEMP_BUFSIZE 128
#define FILENAME_BUFSIZE 32
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
  uint8_t is_valid;
  char pf_name[FILENAME_BUFSIZE];
  char sw_name[MECH_OBSW_COUNT][KEYNAME_SIZE];
  uint8_t sw_color[MECH_OBSW_COUNT][THREE]; // only mechanical keys have RGB LEDs
  uint8_t sw_activation_color[MECH_OBSW_COUNT][THREE];
  uint8_t keypress_count[MAX_TOTAL_SW_COUNT];
  uint8_t dim_unused_keys;
} profile_cache;
extern profile_cache p_cache;

extern dp_global_settings dp_settings;
extern uint8_t current_profile_number;
extern char temp_buf[TEMP_BUFSIZE];
extern char filename_buf[FILENAME_BUFSIZE];

uint8_t mount_sd(void);
uint8_t load_profile_name(void);

#define PROFILE_SCAN_OK 0
#define PROFILE_SCAN_ERROR_NO_TOC 1
#define PROFILE_SCAN_ERROR_NO_PROFILE 2

#ifdef __cplusplus
}
#endif

#endif


