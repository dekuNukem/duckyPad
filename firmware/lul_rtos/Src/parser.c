#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "buttons.h"
#include "parser.h"
#include "ssd1306.h"
#include "keyboard.h"

#define READ_BUF_SIZE 256
#define DEFAULT_CMD_DELAY_MS 15
#define DEFAULT_CHAR_DELAY_MS 15

uint8_t col_lookup[7][3] = {  
   {18, 60, 103},
   {15, 57, 100},
   {12, 54, 97},
   {9, 51, 94},
   {6, 48, 91},
   {3, 45, 88},
   {0, 42, 85}
};

FRESULT sd_fresult;
FATFS sd_fs;
FIL sd_file;
DIR dir;
FILINFO fno;
uint8_t mount_result;
uint16_t cmd_delay, char_delay;

uint8_t available_profile[MAX_PROFILES];
char temp_buf[PATH_SIZE];
char lfn_buf[FILENAME_SIZE];
char key_name_buf[FILENAME_SIZE];
char read_buffer[READ_BUF_SIZE];
char nonexistent_keyname[] = "-";

profile_cache p_cache;

const char syntax_NAME[] = "NAME ";
const char syntax_REM[] = "REM ";
const char syntax_C_COMMENT[] = "// ";
const char syntax_DEFAULTDELAY[] = "DEFAULTDELAY ";
const char syntax_DEFAULTCHARDELAY[] = "DEFAULTCHARDELAY ";
const char syntax_DELAY[] = "DELAY ";
const char syntax_STRING[] = "STRING ";
const char syntax_ENTER[] = "ENTER";
const char syntax_ESC[] = "ESCAPE";
const char syntax_[] = " ";

char* find_profile(uint8_t pid)
{
  char* profile_fn;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;

  if (f_opendir(&dir, "/") != FR_OK)
    return NULL;

  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "profile%d_", pid);
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      profile_fn = fno.lfname[0] ? fno.lfname : fno.fname;
      if(strncmp(temp_buf, profile_fn, strlen(temp_buf)) == 0)
        return profile_fn;
    }
  }
  f_closedir(&dir);
  return NULL;
}

char* get_keyname(char* pf_fn, uint8_t keynum)
{
  char* key_fn;
  char* start;
  char* end;
  char* ret = nonexistent_keyname;
  if(pf_fn == NULL)
    return ret;

  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s", pf_fn);
  if (f_opendir(&dir, temp_buf) != FR_OK)
    return ret;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "key%d_", keynum + 1);
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    key_fn = fno.lfname[0] ? fno.lfname : fno.fname;
    if(strncmp(temp_buf, key_fn, strlen(temp_buf)) == 0)
    {
      start = key_fn + strlen(temp_buf);
      end = strstr(start, ".txt");
      memset(key_name_buf, 0, FILENAME_SIZE);
      strncpy(key_name_buf, start, end - start > (KEYNAME_SIZE - 1) ? (KEYNAME_SIZE - 1) : end - start);
      ret = key_name_buf;
      break;
    }
  }
  f_closedir(&dir);
  return ret;
}

void scan_profiles(void)
{
  char* profile_fn;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;
  memset(available_profile, 0, MAX_PROFILES);

  if (f_opendir(&dir, "/") != FR_OK)
    return;

  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "profile");
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      profile_fn = fno.lfname[0] ? fno.lfname : fno.fname;
      if(strncmp(temp_buf, profile_fn, strlen(temp_buf)) == 0)
      {
        uint8_t num = atoi(profile_fn + strlen(temp_buf));
        if(num == 0)
          continue;
        if(num < MAX_PROFILES)
          available_profile[num] = 1;
      }
    }
  }
  f_closedir(&dir);
  return;
}

void load_profile(uint8_t pid)
{
  char* ppppp = find_profile(pid);
  if(ppppp == NULL)
    return;
  memset(p_cache.profile_fn, 0, FILENAME_SIZE);
  strcpy(p_cache.profile_fn, ppppp);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    memset(p_cache.key_fn[i], 0, KEYNAME_SIZE);
    strcpy(p_cache.key_fn[i], get_keyname(p_cache.profile_fn, i));
  }
  p_cache.current_profile = pid;
}

void print_keyname(char* keyname, uint8_t keynum)
{
  if(strlen(keyname) > 7)
    keyname[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  uint8_t x_start = col_lookup[strlen(keyname) - 1][col];
  ssd1306_SetCursor(x_start, (row + 1) * 10 + 3);
  ssd1306_WriteString(keyname, Font_6x10,White);
}

// print current profile
void print_legend(void)
{
  ssd1306_Fill(Black);
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "profile%d_", p_cache.current_profile);
  char* pf_name = p_cache.profile_fn + strlen(temp_buf);
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "P%d: %s", p_cache.current_profile, pf_name);
  if(strlen(temp_buf) > 21)
    temp_buf[21] = 0;
  uint8_t x_start = (21 - strlen(temp_buf)) * 3;
  ssd1306_SetCursor(x_start, 0);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    print_keyname(p_cache.key_fn[i], i);
  ssd1306_UpdateScreen();
}

void change_profile(uint8_t direction)
{
  uint8_t is_all_empty = 1;
  uint8_t next_profile = p_cache.current_profile;

  for (int i = 0; i < MAX_PROFILES; ++i)
    if(available_profile[i])
      is_all_empty = 0;

  if(is_all_empty)
  {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(32, 0);
    ssd1306_WriteString("no valid profiles!",Font_6x10,White);
    ssd1306_UpdateScreen();
    return;
  }

  while(1)
  {
    if(direction == NEXT_PROFILE)
      next_profile = next_profile + 1 > MAX_PROFILES - 1 ? 0 : next_profile + 1;
    else
      next_profile = next_profile - 1 > MAX_PROFILES - 1 ? MAX_PROFILES - 1 : next_profile - 1;
    if(available_profile[next_profile])
      break;
  }
  load_profile(next_profile);
  print_legend();
  f_closedir(&dir);
  f_close(&sd_file);
}

uint8_t parse_line(char* line)
{
  printf(">> %s\n", line);
  return 0;
}

void handle_keypress(uint8_t keynum)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/%s", p_cache.profile_fn, p_cache.key_fn[keynum]);
  printf("%s\n", temp_buf);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto end;
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    parse_line(read_buffer);
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  end:
  f_close(&sd_file);
}


void parser_test(void)
{
  ;
}
