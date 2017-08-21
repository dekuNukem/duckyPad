#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "buttons.h"
#include "parser.h"
#include "ssd1306.h"

#define READ_BUF_SIZE 256

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

char* profile_fn;
uint8_t current_profile;
char temp_buf[LFN_SIZE];
char lfn_buf[LFN_SIZE];
char read_buffer[READ_BUF_SIZE];
char unnamed_keyname[] = "???";
char undefined_keyname[] = "-";
const char name_label[] = "NAME ";

char* find_profile(uint8_t pid)
{
  fno.lfname = lfn_buf; 
  fno.lfsize = LFN_SIZE - 1;

  if (f_opendir(&dir, "/") != FR_OK)
    return NULL;

  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "profile%d_", pid);
  while(1)
  {
    memset(lfn_buf, 0, LFN_SIZE);
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
  if(pf_fn == NULL)
    return undefined_keyname;
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s/key%d.txt", pf_fn, keynum + 1);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    f_close(&sd_file);
    return undefined_keyname;
  }
  memset(read_buffer, 0, READ_BUF_SIZE);
  f_gets(read_buffer, READ_BUF_SIZE, &sd_file);
  f_close(&sd_file);
  for (int i = 0; i < READ_BUF_SIZE; ++i)
    if(read_buffer[i] == '\r' || read_buffer[i] == '\n')
      read_buffer[i] = 0;
  if(strncmp(name_label, read_buffer, strlen(name_label)) == 0)
    return read_buffer + strlen(name_label);
  return unnamed_keyname;
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

void print_legend(uint8_t pf_num, char* pf_fn)
{
  ssd1306_Fill(Black);
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "profile%d_", pf_num);
  char* pf_name = pf_fn + strlen(temp_buf);
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "P%d: %s", pf_num, pf_name);
  if(strlen(temp_buf) > 21)
    temp_buf[21] = 0;
  uint8_t x_start = (21 - strlen(temp_buf)) * 3;
  ssd1306_SetCursor(x_start, 0);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    print_keyname(get_keyname(pf_fn, i), i);
  ssd1306_UpdateScreen();
}

void change_profile(uint8_t dir)
{
  uint16_t count = 0;
  while(1)
  {
    count++;
    if(dir == NEXT_PROFILE)
      current_profile++;
    else
      current_profile--;
    char* ddddd = find_profile(current_profile);
    if(ddddd != NULL)
    {
      print_legend(current_profile, ddddd);
      return;
    }
    else if(count > 255)
    {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("no valid profiles!",Font_6x10,White);
      ssd1306_UpdateScreen();
      return;
    }
  }
}

void parser_test(void)
{
  ;
}
  
  

