#ifndef __PARSER_H
#define __PARSER_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"
#include "buttons.h"

#define READ_BUF_SIZE 256
#define MAX_PROFILES 32
#define PATH_SIZE 40
#define FILENAME_SIZE 24
#define KEYNAME_SIZE 8
#define PARSE_OK 0
#define PARSE_ERROR 1
#define PARSE_NOT_FOUND 2
#define PARSE_EMPTY_LINE 3
#define NEXT_PROFILE 0
#define PREV_PROFILE 1

extern FRESULT sd_fresult;
extern FATFS sd_fs;
extern FIL sd_file;
extern DIR dir;
extern FILINFO fno;
extern uint8_t mount_result;
extern uint8_t has_valid_profiles;
extern char temp_buf[PATH_SIZE];

typedef struct
{
  uint8_t current_profile;
  uint8_t available_profile[MAX_PROFILES];
  char profile_fn[FILENAME_SIZE];
  char key_fn[MAPPABLE_KEY_COUNT][FILENAME_SIZE];
  uint8_t individual_key_color[MAPPABLE_KEY_COUNT][3];
  uint8_t individual_keydown_color[MAPPABLE_KEY_COUNT][3];
} profile_cache;

typedef struct
{
  uint32_t sleep_after_ms;
} dp_global_settings;

void change_profile(uint8_t dir);
void handle_keypress(uint8_t key_num, but_status* b_status);
void scan_profiles(void);
uint8_t get_last_profile(void);
void restore_profile(uint8_t profile_id);
void keypress_wrap(uint8_t keynum, uint8_t for_release);
void print_legend(int8_t x_offset, int8_t y_offset);
void get_global_settings(void);
void save_last_profile(uint8_t profile_id);

extern profile_cache p_cache;
extern char temp_buf[PATH_SIZE];
extern dp_global_settings dp_settings;
extern uint8_t key_being_held[MAPPABLE_KEY_COUNT];

#ifdef __cplusplus
}
#endif

#endif


