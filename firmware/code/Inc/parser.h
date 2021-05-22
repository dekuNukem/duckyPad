#ifndef __PARSER_H
#define __PARSER_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"
#include "buttons.h"
#include "keyboard.h"
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
#define DEFAULT_CMD_DELAY_MS 18
#define DEFAULT_CHAR_DELAY_MS 18
#define PF_CACHE_FILENAME_MAXLEN 7
extern FRESULT sd_fresult;
extern FATFS sd_fs;
extern FIL sd_file;
extern DIR dir;
extern FILINFO fno;
extern uint8_t mount_result;
extern uint8_t has_valid_profiles;
extern char temp_buf[PATH_SIZE];
extern unsigned int bytes_read;

typedef struct
{
  uint8_t current_profile;
  uint8_t available_profile[MAX_PROFILES];
  char profile_fn[FILENAME_SIZE];
  char key_fn[MAPPABLE_KEY_COUNT][FILENAME_SIZE];
  uint8_t individual_key_color[MAPPABLE_KEY_COUNT][3];
  uint8_t individual_keydown_color[MAPPABLE_KEY_COUNT][3];
} profile_cache;
extern profile_cache p_cache;

typedef struct
{
  uint8_t is_in_use;
  char name[FILENAME_SIZE];
} keymap_cache;

typedef struct
{
  uint32_t sleep_after_ms;
} dp_global_settings;

#define DPC_NONE 0
#define DPC_SLEEP 1
#define DPC_PREV_PROFILE 2
#define DPC_NEXT_PROFILE 3
#define DPC_GOTO_PROFILE 4

typedef struct
{
  uint8_t type;
  uint8_t data;
} duckypad_parsed_command;

void dpc_init(duckypad_parsed_command* dpc);
void change_profile(uint8_t dir);
void handle_keypress(uint8_t key_num, but_status* b_status);
void scan_profiles(void);
uint8_t get_last_profile(void);
void restore_profile(uint8_t profile_id, uint8_t reset_loop_count, uint8_t reload_colors);
void keypress_wrap(uint8_t keynum);
void print_legend(int8_t x_offset, int8_t y_offset);
void save_settings(void);
void load_settings(void);
void save_last_profile(uint8_t profile_id);
void list_profiles(uint8_t page);
void print_keyname(char* keyname, uint8_t keynum, int8_t x_offset, int8_t y_offset);
char* goto_next_arg(char* buf, char* buf_end);
void testest(void);

extern profile_cache p_cache;
extern char temp_buf[PATH_SIZE];
extern dp_global_settings dp_settings;
extern my_key hold_cache[MAPPABLE_KEY_COUNT];
extern my_key hold_cach2[MAPPABLE_KEY_COUNT];

extern char lfn_buf[FILENAME_SIZE];
extern char read_buffer[READ_BUF_SIZE];
extern char curr_kb_layout[FILENAME_SIZE];
extern char project_url[];
extern duckypad_parsed_command my_dpc;

#ifdef __cplusplus
}
#endif

#endif


