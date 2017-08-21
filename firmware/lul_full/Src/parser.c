#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "buttons.h"
#include "parser.h"

#define READ_BUF_SIZE 256

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

uint8_t find_keys(char* pf_fn, uint8_t key_num)
{
  if(pf_fn == NULL)
    return PARSE_ERROR;

  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s/key%d.txt", pf_fn, key_num);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    f_close(&sd_file);
    return PARSE_ERROR;
  }
  printf("%s\n", temp_buf);
  memset(read_buffer, 0, READ_BUF_SIZE);
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file))
    printf(">>>>%s\n", read_buffer);
  f_close(&sd_file);
  return PARSE_OK;
}

void parser_test(void)
{
  char* ddddd = find_profile(1);
  find_keys(ddddd, 1);
  find_keys(ddddd, 2);
  find_keys(ddddd, 3);
  find_keys(ddddd, 4);
  find_keys(ddddd, 5);
}
  
  

