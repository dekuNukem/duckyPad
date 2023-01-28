#ifndef __PARSER_H
#define __PARSER_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"
#include "buttons.h"
#include "keyboard.h"
#include "shared.h"
#include "ds3_vm.h"


#define READ_BUF_SIZE 256
#define MAX_PROFILES 32
#define PATH_SIZE 40
#define FILENAME_SIZE 24
#define KEYNAME_SIZE 10
#define PARSE_OK 0
#define PARSE_ERROR 1

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
extern char temp_buf[PATH_SIZE];
extern unsigned int bytes_read;
extern char read_buffer[READ_BUF_SIZE];

typedef struct
{
  uint8_t current_profile;
  uint8_t available_profile[MAX_PROFILES];
  char profile_fn[FILENAME_SIZE];
  char key_fn[MAPPABLE_KEY_COUNT][KEYNAME_SIZE];
  uint8_t individual_key_color[MAPPABLE_KEY_COUNT][3];
  uint8_t individual_keydown_color[MAPPABLE_KEY_COUNT][3];
} profile_cache;
extern profile_cache p_cache;

typedef struct
{
  uint32_t sleep_after_ms;
} dp_global_settings;

void der_init(ds3_exe_result* der);
void change_profile(uint8_t dir);
void handle_keypress(uint8_t keynum, but_status* b_status, ds3_exe_result* exe);
void scan_profiles(void);
uint8_t get_last_profile(void);
void restore_profile(uint8_t profile_id);
void keypress_wrapper(uint8_t keynum, ds3_exe_result* exe);
void print_legend(void);
void save_settings(void);
void load_settings(void);
void save_last_profile(uint8_t profile_id);
void print_keyname(char* keyname, uint8_t keynum);
char* goto_next_arg(char* buf, char* buf_end);
void delay_wrapper(int32_t amount, int32_t fuzz);
uint16_t get_hash_at_first_nonspace_word(char* msg);

extern profile_cache p_cache;
extern char temp_buf[PATH_SIZE];
extern dp_global_settings dp_settings;
extern my_key hold_cache[MAPPABLE_KEY_COUNT];

extern char lfn_buf[FILENAME_SIZE];
extern char read_buffer[READ_BUF_SIZE];
extern char curr_kb_layout[FILENAME_SIZE];
extern char project_url[];
extern ds3_exe_result my_der;

#ifdef __cplusplus
}
#endif

#endif
