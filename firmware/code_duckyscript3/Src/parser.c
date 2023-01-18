#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shared.h"
#include "buttons.h"
#include "parser.h"
#include "ssd1306.h"
#include "keyboard.h"
#include "animations.h"
#include "usbd_desc.h"
#include "hash_lookup.h"

uint8_t pf_name_cache[MAX_PROFILES][PF_CACHE_FILENAME_MAXLEN];

static const uint8_t col_lookup[7][3] = {  
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
uint8_t has_valid_profiles;
int32_t cmd_delay, char_delay, char_delay_jitter;
unsigned int bytes_read;
ds3_exe_result my_der;
char temp_buf[PATH_SIZE];
char lfn_buf[FILENAME_SIZE];
char key_name_buf[FILENAME_SIZE];
char read_buffer[READ_BUF_SIZE];
char prev_line[READ_BUF_SIZE];
char nonexistent_keyname[] = "\253";
profile_cache p_cache;
dp_global_settings dp_settings;
my_key hold_cache[MAPPABLE_KEY_COUNT];
my_key hold_cache2[MAPPABLE_KEY_COUNT];
char curr_kb_layout[FILENAME_SIZE];
uint8_t key_max_loop[MAPPABLE_KEY_COUNT];
uint8_t key_press_count[MAPPABLE_KEY_COUNT];

char project_url[] = "duckyPad.com";

const char cmd_LOOP[] = "LOOP";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_SW_SELF_COLOR[] = "SWCOLOR ";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS ";

const char cmd_LMOUSE[] = "LMOUSE";
const char cmd_RMOUSE[] = "RMOUSE";
const char cmd_MMOUSE[] = "MMOUSE";
const char cmd_MOUSE_MOVE[] = "MOUSE_MOVE ";
const char cmd_MOUSE_WHEEL[] = "MOUSE_WHEEL ";

void delay_wrapper(int32_t amount, int32_t fuzz)
{
  osDelay(fuzz == 0 ? amount : amount + rand() % fuzz);
}

char* goto_next_arg(char* buf, char* buf_end)
{
  char* curr = buf;  
  if(buf == NULL || curr >= buf_end)
    return NULL;
  while(curr < buf_end && *curr != ' ')
      curr++;
  while(curr < buf_end && *curr == ' ')
      curr++;
  if(curr >= buf_end)
    return NULL;
  return curr;
}

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

void assign_colors(uint8_t keynum, char* curr, char* msg_end)
{
  curr = goto_next_arg(curr, msg_end);
  p_cache.individual_key_color[keynum][0] = atoi(curr);

  curr = goto_next_arg(curr, msg_end);
  p_cache.individual_key_color[keynum][1] = atoi(curr);

  curr = goto_next_arg(curr, msg_end);
  p_cache.individual_key_color[keynum][2] = atoi(curr);
}

uint8_t is_sw_color_line(char* line)
{
  if(line == NULL)
    return 0;
  if(strncmp(cmd_SWCOLOR, line, strlen(cmd_SWCOLOR)) == 0)
    return 1;
  if(strncmp(cmd_SW_SELF_COLOR, line, strlen(cmd_SW_SELF_COLOR)) == 0)
    return 2;
  return 0;
}

uint8_t load_colors(char* pf_fn)
{
  char *curr, *msg_end;
  uint8_t ret;
  uint8_t is_unused_keys_dimmed = 1;
  uint8_t has_user_kd = 0;

  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    p_cache.individual_key_color[i][0] = DEFAULT_BG_RED;
    p_cache.individual_key_color[i][1] = DEFAULT_BG_GREEN;
    p_cache.individual_key_color[i][2] = DEFAULT_BG_BLUE;
    p_cache.individual_keydown_color[i][0] = DEFAULT_KD_RED;
    p_cache.individual_keydown_color[i][1] = DEFAULT_KD_GREEN;
    p_cache.individual_keydown_color[i][2] = DEFAULT_KD_BLUE;
  }

  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/config.txt", pf_fn);

  ret = f_open(&sd_file, temp_buf, FA_READ);

  if(ret == FR_OK)
    goto color_normal;
  else if(ret == FR_NO_FILE)
    goto color_end;
  else
    HAL_NVIC_SystemReset();

  color_normal:
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    curr = read_buffer;
    msg_end = curr + strlen(read_buffer);
    if(strncmp(cmd_BG_COLOR, read_buffer, strlen(cmd_BG_COLOR)) == 0)
    {
      curr = goto_next_arg(curr, msg_end);
      uint8_t rrr = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      uint8_t ggg = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      uint8_t bbb = atoi(curr);
      for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
      {
        p_cache.individual_key_color[i][0] = rrr;
        p_cache.individual_key_color[i][1] = ggg;
        p_cache.individual_key_color[i][2] = bbb;

        if(has_user_kd == 0)
        {
          p_cache.individual_keydown_color[i][0] = 255 - rrr;
          p_cache.individual_keydown_color[i][1] = 255 - ggg;
          p_cache.individual_keydown_color[i][2] = 255 - bbb;
        }
      }
    }
    else if(strncmp(cmd_KD_COLOR, read_buffer, strlen(cmd_KD_COLOR)) == 0)
    {
      curr = goto_next_arg(curr, msg_end);
      uint8_t rrr = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      uint8_t ggg = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      uint8_t bbb = atoi(curr);
      for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
      {
        p_cache.individual_keydown_color[i][0] = rrr;
        p_cache.individual_keydown_color[i][1] = ggg;
        p_cache.individual_keydown_color[i][2] = bbb;
      }
      has_user_kd = 1;
    }
    else if(is_sw_color_line(read_buffer) == 1)
    {
      uint8_t keynum = atoi(read_buffer + strlen(cmd_SWCOLOR)) - 1;
      if(keynum > MAX_PROFILES)
        continue;
      assign_colors(keynum, curr, msg_end);
      if(has_user_kd == 0)
      {
        p_cache.individual_keydown_color[keynum][0] = 255 - p_cache.individual_key_color[keynum][0];
        p_cache.individual_keydown_color[keynum][1] = 255 - p_cache.individual_key_color[keynum][1];
        p_cache.individual_keydown_color[keynum][2] = 255 - p_cache.individual_key_color[keynum][2];
      }
    }

    else if(strncmp(cmd_DIM_UNUSED_KEYS, read_buffer, strlen(cmd_DIM_UNUSED_KEYS)) == 0)
    {
      curr = goto_next_arg(curr, msg_end);
      is_unused_keys_dimmed = atoi(curr);
    }
  }
  ret = PARSE_OK;
  color_end:
  f_close(&sd_file);
  if(is_unused_keys_dimmed)
  {
    for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    {
      if(strcmp(p_cache.key_fn[i], nonexistent_keyname) == 0)
      {
        p_cache.individual_key_color[i][0] = 0;
        p_cache.individual_key_color[i][1] = 0;
        p_cache.individual_key_color[i][2] = 0;
        p_cache.individual_keydown_color[i][0] = 0;
        p_cache.individual_keydown_color[i][1] = 0;
        p_cache.individual_keydown_color[i][2] = 0;
      }
    }
  }
  return ret;
}

uint8_t how_many_digits(uint8_t number)
{
  if(number >= 100)
    return 3;
  if(number >= 10)
    return 2;
  return 1;
}

// find out what profile folders are available
void scan_profiles(void)
{
  char* profile_fn;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;
  memset(p_cache.available_profile, 0, MAX_PROFILES);

  if (f_opendir(&dir, "/") != FR_OK)
    goto scan_profiles_end;

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
        {
          p_cache.available_profile[num] = 1;
          strncpy((char*)pf_name_cache[num-1], profile_fn+8+how_many_digits(num), PF_CACHE_FILENAME_MAXLEN-1);
        }
      }
    }
  }
  scan_profiles_end:
  f_closedir(&dir);
}

uint16_t djb2(uint8_t *str, uint8_t len)
{
    uint32_t hash = 5381;
    uint8_t *end = str + len;
    uint8_t c;
    while ((c = *str++) && (str <= end))
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return (uint16_t)hash;
}

char* get_nonspace_arg_len(char* buf, uint8_t *len)
{
  if(buf == NULL)
    return NULL;
  while(*buf != 0 && *buf == ' ')
    buf++;
  // now buf points to the start of first non-whitespace character
  char* space_ptr = strchr(buf, ' ');
  if(space_ptr == NULL)
    *len = strlen(buf);
  else
    *len = space_ptr - buf;
  return buf;
}

uint16_t get_hash_at_first_nonspace_word(char* msg)
{
  uint8_t kw_len = 0;
  char* arg_start = get_nonspace_arg_len(msg, &kw_len);
  return djb2(arg_start, kw_len);
}

void strip_newline(char* line, uint8_t size)
{
  for(int i = 0; i < size; ++i)
    if(line[i] == '\n' || line[i] == '\r')
      line[i] = 0;
}

uint8_t get_keynames(profile_cache* ppppppp)
{
  uint8_t ret = 1;
  uint8_t this_key_index;
  if(ppppppp == NULL)
    return ret;
  memset(key_max_loop, 0, MAPPABLE_KEY_COUNT);
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/config.txt", ppppppp->profile_fn);

  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    memset(ppppppp->key_fn[i], 0, FILENAME_SIZE);
    strcpy(ppppppp->key_fn[i], nonexistent_keyname);
  }

  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto glk_end;
  
  while(f_gets(temp_buf, PATH_SIZE, &sd_file))
  {
    if(temp_buf[0] == 'z')
    {
      this_key_index = atoi(temp_buf+1);
      if(this_key_index == 0)
        continue;
      this_key_index--;
      memset(ppppppp->key_fn[this_key_index], 0, FILENAME_SIZE);
      strip_newline(temp_buf, PATH_SIZE);
      strcpy(ppppppp->key_fn[this_key_index], goto_next_arg(temp_buf, temp_buf+PATH_SIZE));
  }
  if(temp_buf[0] == 's')
    {
      this_key_index = atoi(temp_buf+1);
      if(this_key_index == 0)
        continue;
      this_key_index--;
      uint8_t this_key_max_loop = atoi(goto_next_arg(temp_buf, temp_buf+PATH_SIZE));
      if(this_key_max_loop == 0)
        continue;
      key_max_loop[this_key_index] = this_key_max_loop;
    }
  }
  ret = 0;
  glk_end:
  f_close(&sd_file);
  return ret;
}

void parse_loop_state(char* curr, char* msg_end)
{
  uint8_t keynum, current_loop;
  curr = goto_next_arg(curr, msg_end);
  keynum = atoi(curr);
  curr = goto_next_arg(curr, msg_end);
  current_loop = atoi(curr);
  if(keynum >= MAPPABLE_KEY_COUNT)
    return;
  key_press_count[keynum] = current_loop;
}

void parse_color_state(char* curr, char* msg_end)
{
  uint8_t keynum;
  curr = goto_next_arg(curr, msg_end);
  keynum = atoi(curr);
  if(keynum >= MAPPABLE_KEY_COUNT)
    return;
  curr = goto_next_arg(curr, msg_end);
  p_cache.individual_key_color[keynum][0] = atoi(curr);
  curr = goto_next_arg(curr, msg_end);
  p_cache.individual_key_color[keynum][1] = atoi(curr);
  curr = goto_next_arg(curr, msg_end);
  p_cache.individual_key_color[keynum][2] = atoi(curr);
}

void load_loop_state(void)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/state.txt", p_cache.profile_fn);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    // !!!!!!!!!! RESET ALL THE BUFFERS HERE
    f_close(&sd_file);
    return;
  }
  memset(temp_buf, 0, PATH_SIZE);
  while(f_gets(temp_buf, PATH_SIZE, &sd_file))
  {
    if(strncmp(temp_buf, "ls ", 3) == 0)
      parse_loop_state(temp_buf, temp_buf + PATH_SIZE);
    if(strncmp(temp_buf, "cs ", 3) == 0)
      parse_color_state(temp_buf, temp_buf + PATH_SIZE);
    memset(temp_buf, 0, PATH_SIZE);
  }
  f_close(&sd_file);
}

void load_profile(uint8_t pid)
{
  char* profile_name = find_profile(pid);
  if(profile_name == NULL)
    return;
  memset(p_cache.profile_fn, 0, FILENAME_SIZE);
  strcpy(p_cache.profile_fn, profile_name);
  get_keynames(&p_cache);
  load_colors(p_cache.profile_fn);
  load_loop_state();
  change_bg();
  p_cache.current_profile = pid;
}

void print_keyname(char* keyname, uint8_t keynum, int8_t x_offset, int8_t y_offset)
{
  memset(key_name_buf, 0, FILENAME_SIZE);
  strcpy(key_name_buf, keyname);
  if(key_name_buf[0] == 0 || (key_name_buf[0] == nonexistent_keyname[0] && key_name_buf[1] == 0))
    key_name_buf[0] = '-';
  if(strlen(key_name_buf) > 7)
    key_name_buf[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  int8_t x_start = col_lookup[strlen(key_name_buf) - 1][col] + x_offset;
  int8_t y_start = (row + 1) * 10 + 2 + y_offset;
  if(x_start < 0)
    x_start = 0;
  if(y_start < 0)
    y_start = 0;
  ssd1306_SetCursor((uint8_t)x_start, (uint8_t)y_start);
  ssd1306_WriteString(key_name_buf, Font_6x10,White);
}

void print_legend(int8_t x_offset, int8_t y_offset)
{
  ssd1306_Fill(Black);
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "profile%d_", p_cache.current_profile);
  char* pf_name = p_cache.profile_fn + strlen(temp_buf);
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "P%d: %s", p_cache.current_profile, pf_name);
  if(strlen(temp_buf) > 21)
    temp_buf[21] = 0;
  int8_t x_start = (21 - strlen(temp_buf)) * 3 + x_offset;
  if(x_start < 0)
    x_start = 0;
  if(y_offset < 0)
    y_offset = 0;
  ssd1306_SetCursor((uint8_t)x_start, 0);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    print_keyname(p_cache.key_fn[i], i, x_offset, y_offset);
  ssd1306_UpdateScreen();
}

void save_last_profile(uint8_t profile_id)
{
  if(f_open(&sd_file, "dp_stats.txt", FA_CREATE_ALWAYS | FA_WRITE) != 0)
    goto slp_end;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "lp %d\nfw %d.%d.%d\nser %s", profile_id, fw_version_major, fw_version_minor, fw_version_patch, make_serial_string());
  f_write(&sd_file, temp_buf, strlen(temp_buf), &bytes_read);
  slp_end:
  f_close(&sd_file);
}

uint8_t get_last_profile(void)
{
  uint8_t ret = 0;
  if(f_open(&sd_file, "dp_stats.txt", FA_READ) != 0)
    goto glp_end;
  memset(temp_buf, 0, PATH_SIZE);

  while(f_gets(temp_buf, PATH_SIZE, &sd_file))
    if(strncmp(temp_buf, "lp ", 3) == 0)
      ret = atoi(temp_buf+3);

  if(ret >= MAX_PROFILES || p_cache.available_profile[ret] == 0)
    ret = 0;
  glp_end:
  f_close(&sd_file);
  return ret;
}

void save_settings(void)
{
  if(f_open(&sd_file, "dp_settings.txt", FA_CREATE_ALWAYS | FA_WRITE) != 0)
    goto ss_end;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "sleep_after_min %d\nbi %d\nkbl %s\n", dp_settings.sleep_after_ms/60000, brightness_index, curr_kb_layout);
  f_write(&sd_file, temp_buf, strlen(temp_buf), &bytes_read);
  ss_end:
  f_close(&sd_file);
}

void load_settings(void)
{
  if(f_open(&sd_file, "dp_settings.txt", FA_READ) != 0)
    goto ggs_end;
  memset(temp_buf, 0, PATH_SIZE);
  while(f_gets(temp_buf, PATH_SIZE, &sd_file))
  {
    if(strncmp(temp_buf, "sleep_after_min", 15) == 0)
      dp_settings.sleep_after_ms = atoi(temp_buf+15) * 60000;
    if(strncmp(temp_buf, "bi ", 3) == 0)
      brightness_index = atoi(temp_buf+3);
    if(brightness_index >= BRIGHTNESS_LEVELS)
      brightness_index = BRIGHTNESS_LEVELS - 1;
    if(strncmp(temp_buf, "kbl ", 4) == 0)
    {
      strcpy(curr_kb_layout, temp_buf + 4);
      for (int i = 0; i < FILENAME_SIZE; ++i)
        if(curr_kb_layout[i] == '\r' || curr_kb_layout[i] == '\n')
          curr_kb_layout[i] = 0;
    }
  }
  ggs_end:
  f_close(&sd_file);
}

void reset_hold_cache(void)
{
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    hold_cache[i].key_type = KEY_TYPE_UNKNOWN;
    hold_cache[i].code = 0;
    hold_cache2[i].key_type = KEY_TYPE_UNKNOWN;
    hold_cache2[i].code = 0;
  }
}

void restore_profile(uint8_t profile_id)
{
  load_profile(profile_id);
  print_legend(0, 0);
  has_valid_profiles = 1;
  f_closedir(&dir);
  f_close(&sd_file);
  save_last_profile(profile_id);
  reset_hold_cache();
}

void change_profile(uint8_t direction)
{
  uint8_t is_all_empty = 1;
  uint8_t next_profile = p_cache.current_profile;

  for (int i = 0; i < MAX_PROFILES; ++i)
    if(p_cache.available_profile[i])
      is_all_empty = 0;

  if(is_all_empty)
  {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 13);
    ssd1306_WriteString("No Valid Profiles",Font_6x10,White);
    ssd1306_SetCursor(0, 37);
    ssd1306_WriteString(instruction,Font_6x10,White);
    ssd1306_SetCursor(18, 50);
    ssd1306_WriteString(project_url,Font_6x10,White);
    ssd1306_UpdateScreen();
    return;
  }

  while(1)
  {
    if(direction == NEXT_PROFILE)
      next_profile = next_profile + 1 > MAX_PROFILES - 1 ? 0 : next_profile + 1;
    else
      next_profile = next_profile - 1 > MAX_PROFILES - 1 ? MAX_PROFILES - 1 : next_profile - 1;
    if(p_cache.available_profile[next_profile])
      break;
  }
  restore_profile(next_profile);
}

void keypress_wrap(uint8_t keynum)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/key%d.dsb", p_cache.profile_fn, keynum+1);
  printf("%s\n", temp_buf);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto kp_end;
  kp_end:
  f_close(&sd_file);
}

void der_init(ds3_exe_result* der)
{
  der->type = DER_NONE;
  der->data = 0;
}

void handle_keypress(uint8_t keynum, but_status* b_status)
{
  der_init(&my_der);
  keypress_wrap(keynum);
}
