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
#include "ds3_vm.h"

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
unsigned int bytes_read;
char temp_buf[PATH_SIZE];
char lfn_buf[FILENAME_SIZE];
char read_buffer[READ_BUF_SIZE];
char nonexistent_keyname[] = "\253";
profile_cache p_cache;
dp_global_settings dp_settings;
my_key hold_cache[MAPPABLE_KEY_COUNT];
char curr_kb_layout[FILENAME_SIZE];
uint8_t key_press_count[MAPPABLE_KEY_COUNT];

char project_url[] = "duckyPad.com";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_SW_SELF_COLOR[] = "SWCOLOR ";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS ";

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

uint8_t get_keynames(profile_cache* pcache)
{
  uint8_t ret = 1;
  uint8_t this_key_index;
  if(pcache == NULL)
    return ret;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/config.txt", pcache->profile_fn);

  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    memset(pcache->key_fn[i], 0, KEYNAME_SIZE);
    strcpy(pcache->key_fn[i], nonexistent_keyname);
  }

  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto gkn_end;
  
  while(f_gets(temp_buf, PATH_SIZE, &sd_file))
  {
    if(temp_buf[0] == 'z')
    {
      this_key_index = atoi(temp_buf+1);
      if(this_key_index == 0)
        continue;
      this_key_index--;
      memset(pcache->key_fn[this_key_index], 0, KEYNAME_SIZE);
      strip_newline(temp_buf, PATH_SIZE);
      strcpy(pcache->key_fn[this_key_index], goto_next_arg(temp_buf, temp_buf+PATH_SIZE));
    }
  }
  ret = 0;
  gkn_end:
  f_close(&sd_file);
  return ret;
}

uint8_t load_persistent_state(void)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/state.sps", p_cache.profile_fn);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    memset(key_press_count, 0, MAPPABLE_KEY_COUNT);
    f_close(&sd_file);
    return 1;
  }
  memset(read_buffer, 0, READ_BUF_SIZE);
  f_read(&sd_file, read_buffer, READ_BUF_SIZE, &bytes_read);
  f_close(&sd_file);
  memcpy(key_press_count, read_buffer, MAPPABLE_KEY_COUNT);

  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    uint8_t r_addr = i*3 + COLOR_START_ADDR;
    uint8_t red = read_buffer[r_addr];
    uint8_t green = read_buffer[r_addr+1];
    uint8_t blue = read_buffer[r_addr+2];
    set_pixel_3color_update_buffer(i, red, green, blue);
  }
  neopixel_update();
  return 0;
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
  if(load_persistent_state() != 0)
    redraw_bg();
  p_cache.current_profile = pid;
}

void print_keyname(char* keyname, uint8_t keynum)
{
  memset(temp_buf, 0, PATH_SIZE);
  strcpy(temp_buf, keyname);
  if(temp_buf[0] == 0 || (temp_buf[0] == nonexistent_keyname[0] && temp_buf[1] == 0))
    temp_buf[0] = '-';
  if(strlen(temp_buf) > 7)
    temp_buf[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  int8_t x_start = col_lookup[strlen(temp_buf) - 1][col];
  int8_t y_start = (row + 1) * 10 + 2;
  if(x_start < 0)
    x_start = 0;
  if(y_start < 0)
    y_start = 0;
  ssd1306_SetCursor((uint8_t)x_start, (uint8_t)y_start);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
}

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
  int8_t x_start = (21 - strlen(temp_buf)) * 3;
  if(x_start < 0)
    x_start = 0;
  ssd1306_SetCursor((uint8_t)x_start, 0);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    print_keyname(p_cache.key_fn[i], i);
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
    hold_cache[i].type = KEY_TYPE_UNKNOWN;
    hold_cache[i].code = 0;
  }
}

void restore_profile(uint8_t profile_id)
{
  load_profile(profile_id);
  print_legend();
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
    {
      next_profile = next_profile + 1;
      if(next_profile >= MAX_PROFILES)
       next_profile = 1;
    }
    else
    {
      next_profile = next_profile - 1;
      if(next_profile == 0)
        next_profile = MAX_PROFILES - 1;
    }
    if(p_cache.available_profile[next_profile])
      break;
  }
  restore_profile(next_profile);
}

void der_init(ds3_exe_result* der)
{
  der->result = EXE_EMPTY_FILE;
  der->next_pc = 0;
  der->data = 0;
  der->data2 = 0;
  der->epilogue_actions = 0;
}

void save_persistent_state(uint8_t options)
{
  memset(read_buffer, 0, READ_BUF_SIZE);
  if(options & LOOP_STATE)
    memcpy(read_buffer, key_press_count, MAPPABLE_KEY_COUNT);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    uint8_t r_addr = i*3 + COLOR_START_ADDR;
    uint8_t g_addr = r_addr + 1;
    uint8_t b_addr = g_addr + 1;
    uint8_t red, green, blue;
    if(options & COLOR_STATE)
    {
      get_current_color(i, &red, &green, &blue);
    }
    else
    {
      red = p_cache.individual_key_color[i][0];
      green = p_cache.individual_key_color[i][1];
      blue = p_cache.individual_key_color[i][2];
    }
    read_buffer[r_addr] = red;
    read_buffer[g_addr] = green;
    read_buffer[b_addr] = blue;
  }
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/state.sps", p_cache.profile_fn);
  f_open(&sd_file, temp_buf, FA_CREATE_ALWAYS | FA_WRITE);
  f_write(&sd_file, read_buffer, READ_BUF_SIZE, &bytes_read);
  f_close(&sd_file);
}

void keypress_wrapper(uint8_t keynum, ds3_exe_result* exe)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/key%d.dsb", p_cache.profile_fn, keynum+1);
  der_init(exe);
  if(f_stat(temp_buf, NULL) != 0)
    return;
  play_keydown_animation(keynum);
  run_dsb(exe, keynum);
  key_press_count[keynum]++;
  if(exe->epilogue_actions & 0x3)
    save_persistent_state(exe->epilogue_actions);
}

void handle_keypress(uint8_t keynum, but_status* b_status, ds3_exe_result* exe)
{
  keypress_wrapper(keynum, exe);

  if(exe->result != EXE_HALT)
    return;

  uint32_t hold_start = HAL_GetTick();
  while(1)
  {
    keyboard_update();
    if(b_status->button_state == BUTTON_RELEASED)
      return;
    if(HAL_GetTick() - hold_start > 500)
      break;
  }
  // start repeating
  while(1)
  {
    keyboard_update();
    keypress_wrapper(keynum, exe);
    if(b_status->button_state == BUTTON_RELEASED)
      return;
  }
}
