#ifndef __PROFILES_H
#define __PROFILES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#include "input_task.h"
#include "neopixel.h"

#define TEMP_BUFSIZE 128
#define FILENAME_BUFSIZE 64
#define MAX_PROFILES 32
#define KEYNAME_SIZE 8

typedef struct
{
  uint8_t brightness_index;
  uint8_t sleep_index;
  uint8_t last_used_profile;
  char current_kb_layout[FILENAME_BUFSIZE];
} dp_global_settings;

/*
  this caches profile name, and printable keynames
*/
typedef struct
{
  uint8_t is_loaded;
  char dir_path[FILENAME_BUFSIZE];
  char* pf_name;
  char sw_name_firstline[TOTAL_OBSW_COUNT][KEYNAME_SIZE];
  uint8_t sw_color[MECH_OBSW_COUNT][THREE]; // only mechanical keys have RGB LEDs
  uint8_t sw_activation_color[MECH_OBSW_COUNT][THREE];
  uint8_t keypress_count[MAX_TOTAL_SW_COUNT];
  uint8_t dim_unused_keys;
} profile_info;

extern profile_info all_profile_info[MAX_PROFILES];
extern dp_global_settings dp_settings;
extern uint8_t current_profile_number;
extern char temp_buf[TEMP_BUFSIZE];
extern char filename_buf[FILENAME_BUFSIZE];

uint8_t mount_sd(void);

void load_profile_info(void);

#ifdef __cplusplus
}
#endif

#endif


