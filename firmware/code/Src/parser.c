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

static const uint8_t f_key_lookup[24] = {0X3A, 0X3B, 0X3C, 0X3D, 0X3E, 0X3F, 0X40, 0X41, 0X42, 0X43, 0X44, 0X45, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73};

FRESULT sd_fresult;
FATFS sd_fs;
FIL sd_file;
DIR dir;
FILINFO fno;
uint8_t mount_result;
uint8_t has_valid_profiles;
int32_t cmd_delay, char_delay, cmd_delay_fuzz, char_delay_fuzz;
unsigned int bytes_read;
duckypad_parsed_command my_dpc;
char temp_buf[PATH_SIZE];
char lfn_buf[FILENAME_SIZE];
char key_name_buf[FILENAME_SIZE];
char read_buffer[READ_BUF_SIZE];
char prev_line[READ_BUF_SIZE];
char nonexistent_keyname[] = "\253";
profile_cache p_cache;
dp_global_settings dp_settings;
my_key hold_cache[MAPPABLE_KEY_COUNT];
my_key hold_cach2[MAPPABLE_KEY_COUNT];
char curr_kb_layout[FILENAME_SIZE] = "default";
uint8_t key_max_loop[MAPPABLE_KEY_COUNT];
uint8_t key_press_count[MAPPABLE_KEY_COUNT];

char project_url[] = "git.io/duckypad";
const char cmd_REPEAT[] = "REPEAT ";
const char cmd_REM[] = "REM ";
const char cmd_DEFAULTDELAY[] = "DEFAULTDELAY ";
const char cmd_DEFAULTCHARDELAY[] = "DEFAULTCHARDELAY ";
const char cmd_DEFAULTDELAYFUZZ[] = "DEFAULTDELAYFUZZ ";
const char cmd_DEFAULTCHARDELAYFUZZ[] = "DEFAULTCHARDELAYFUZZ ";
const char cmd_DELAY[] = "DELAY ";
const char cmd_STRING[] = "STRING ";
const char cmd_STRINGLN[] = "STRINGLN ";
const char cmd_UARTPRINT[] = "UARTPRINT ";
const char cmd_ESCAPE[] = "ESCAPE";
const char cmd_ESC[] = "ESC";
const char cmd_ENTER[] = "ENTER";
const char cmd_UP[] = "UP";
const char cmd_DOWN[] = "DOWN";
const char cmd_LEFT[] = "LEFT";
const char cmd_RIGHT[] = "RIGHT";
const char cmd_UPARROW[] = "UPARROW";
const char cmd_DOWNARROW[] = "DOWNARROW";
const char cmd_LEFTARROW[] = "LEFTARROW";
const char cmd_RIGHTARROW[] = "RIGHTARROW";
const char cmd_BACKSPACE[] = "BACKSPACE";
const char cmd_TAB[] = "TAB";
const char cmd_CAPSLOCK[] = "CAPSLOCK";
const char cmd_PRINTSCREEN[] = "PRINTSCREEN";
const char cmd_SCROLLLOCK[] = "SCROLLLOCK";
const char cmd_PAUSE[] = "PAUSE";
const char cmd_BREAK[] = "BREAK";
const char cmd_INSERT[] = "INSERT";
const char cmd_HOME[] = "HOME";
const char cmd_PAGEUP[] = "PAGEUP";
const char cmd_PAGEDOWN[] = "PAGEDOWN";
const char cmd_DELETE[] = "DELETE";
const char cmd_END[] = "END";
const char cmd_SPACE[] = "SPACE";

const char cmd_SHIFT[] = "SHIFT";
const char cmd_RSHIFT[] = "RSHIFT";

const char cmd_ALT[] = "ALT";
const char cmd_RALT[] = "RALT";
const char cmd_OPTION[] = "OPTION";
const char cmd_ROPTION[] = "ROPTION";

const char cmd_GUI[] = "GUI";
const char cmd_WINDOWS[] = "WINDOWS";
const char cmd_COMMAND[] = "COMMAND";
const char cmd_RWINDOWS[] = "RWINDOWS";
const char cmd_RCOMMAND[] = "RCOMMAND";

const char cmd_CONTROL[] = "CONTROL";
const char cmd_CTRL[] = "CTRL";
const char cmd_RCTRL[] = "RCTRL";

const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_SWCOLOR[] = "SWCOLOR_";
const char cmd_SW_SELF_COLOR[] = "SWCOLOR ";
const char cmd_DIM_UNUSED_KEYS[] = "DIM_UNUSED_KEYS ";

const char cmd_NUMLOCK[] = "NUMLOCK"; // Keyboard Num Lock and Clear
const char cmd_KPSLASH[] = "KP_SLASH"; // Keypad /
const char cmd_KPASTERISK[] = "KP_ASTERISK"; // Keypad *
const char cmd_KPMINUS[] = "KP_MINUS"; // Keypad -
const char cmd_KPPLUS[] = "KP_PLUS"; // Keypad +
const char cmd_KPENTER[] = "KP_ENTER"; // Keypad ENTER
const char cmd_KP1[] = "KP_1"; // Keypad 1 and End
const char cmd_KP2[] = "KP_2"; // Keypad 2 and Down Arrow
const char cmd_KP3[] = "KP_3"; // Keypad 3 and PageDn
const char cmd_KP4[] = "KP_4"; // Keypad 4 and Left Arrow
const char cmd_KP5[] = "KP_5"; // Keypad 5
const char cmd_KP6[] = "KP_6"; // Keypad 6 and Right Arrow
const char cmd_KP7[] = "KP_7"; // Keypad 7 and Home
const char cmd_KP8[] = "KP_8"; // Keypad 8 and Up Arrow
const char cmd_KP9[] = "KP_9"; // Keypad 9 and Page Up
const char cmd_KP0[] = "KP_0"; // Keypad 0 and Insert
const char cmd_KPDOT[] = "KP_DOT"; // Keypad . and Delete
const char cmd_KPEQUAL[] = "KP_EQUAL"; // Keypad EQUAL

const char cmd_MK_VOLUP[] = "MK_VOLUP";
const char cmd_MK_VOLDOWN[] = "MK_VOLDOWN";
const char cmd_MK_VOLMUTE[] = "MK_MUTE";
const char cmd_MK_PREV[] = "MK_PREV";
const char cmd_MK_NEXT[] = "MK_NEXT";
const char cmd_MK_PLAYPAUSE[] = "MK_PP";
const char cmd_MK_STOP[] = "MK_STOP";

const char cmd_MENU[] = "MENU";
const char cmd_APP[] = "APP";
const char cmd_OLD_HOLD[] = "EMUK "; // OLD_HOLD
const char cmd_POWER[] = "POWER";

const char cmd_LOOP[] = "LOOP";
const char cmd_LCR[] = "LCR"; // loop counter reset

const char cmd_SLEEP[] = "DP_SLEEP";
const char cmd_PREV_PROFILE[] = "PREV_PROFILE";
const char cmd_NEXT_PROFILE[] = "NEXT_PROFILE";
const char cmd_GOTO_PROFILE[] = "GOTO_PROFILE";

const char cmd_LMOUSE[] = "LMOUSE";
const char cmd_RMOUSE[] = "RMOUSE";
const char cmd_MMOUSE[] = "MMOUSE";
const char cmd_MOUSE_MOVE[] = "MOUSE_MOVE ";
const char cmd_MOUSE_WHEEL[] = "MOUSE_WHEEL ";

const char cmd_HOLD[] = "KEYDOWN ";
const char cmd_RELEASE[] = "KEYUP ";

const char cmd_INJECT_MOD[] = "INJECT_MOD";

int32_t make_fuzz(int32_t amount, int32_t fuzz)
{
  if(fuzz == 0)
    return amount;
  return amount + rand() % fuzz;
}

void delay_wrapper(int32_t amount, int32_t fuzz)
{
  osDelay(make_fuzz(amount, fuzz));
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

void list_profiles(uint8_t page)
{
  uint8_t p_start = page*MAPPABLE_KEY_COUNT;
  uint8_t p_end = p_start + MAPPABLE_KEY_COUNT;
  // uint32_t time = HAL_GetTick();
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "Profile %d - %d", p_start+1, p_end);
  temp_buf[21] = 0;
  ssd1306_Fill(Black);
  ssd1306_SetCursor((21 - strlen(temp_buf)) * 3, 0);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
  for (uint8_t i = p_start; i < p_end; ++i)
    if(i < MAX_PROFILES)
      print_keyname(pf_name_cache[i], i - p_start, 0, 0);
  ssd1306_UpdateScreen();
  // printf("took %dms\n", HAL_GetTick() - time);
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
    hold_cach2[i].key_type = KEY_TYPE_UNKNOWN;
    hold_cach2[i].code = 0;
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

void parse_special_key(char* msg, my_key* this_key)
{
  if(msg == NULL || this_key == NULL)
    return;
 
  this_key->key_type = KEY_TYPE_SPECIAL;
  if(strncmp(msg, cmd_UP, strlen(cmd_UP)) == 0)
  {
    this_key->code = KEY_UP_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_DOWN, strlen(cmd_DOWN)) == 0)
  {
    this_key->code = KEY_DOWN_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_LEFT, strlen(cmd_LEFT)) == 0)
  {
    this_key->code = KEY_LEFT_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_RIGHT, strlen(cmd_RIGHT)) == 0)
  {
    this_key->code = KEY_RIGHT_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_UPARROW, strlen(cmd_UPARROW)) == 0)
  {
    this_key->code = KEY_UP_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_DOWNARROW, strlen(cmd_DOWNARROW)) == 0)
  {
    this_key->code = KEY_DOWN_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_LEFTARROW, strlen(cmd_LEFTARROW)) == 0)
  {
    this_key->code = KEY_LEFT_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_RIGHTARROW, strlen(cmd_RIGHTARROW)) == 0)
  {
    this_key->code = KEY_RIGHT_ARROW;
    return;
  }
  else if(strncmp(msg, cmd_ESCAPE, strlen(cmd_ESCAPE)) == 0)
  {
    this_key->code = KEY_ESC;
    return;
  }
  else if(strncmp(msg, cmd_ESC, strlen(cmd_ESC)) == 0)
  {
    this_key->code = KEY_ESC;
    return;
  }
  else if(strncmp(msg, cmd_ENTER, strlen(cmd_ENTER)) == 0)
  {
    this_key->code = KEY_RETURN;
    return;
  }
  else if(strncmp(msg, cmd_BACKSPACE, strlen(cmd_BACKSPACE)) == 0)
  {
    this_key->code = KEY_BACKSPACE;
    return;
  }
  else if(strncmp(msg, cmd_TAB, strlen(cmd_TAB)) == 0)
  {
    this_key->code = KEY_TAB;
    return;
  }
  else if(strncmp(msg, cmd_CAPSLOCK, strlen(cmd_CAPSLOCK)) == 0)
  {
    this_key->code = KEY_CAPS_LOCK;
    return;
  }
  else if(strncmp(msg, cmd_PRINTSCREEN, strlen(cmd_PRINTSCREEN)) == 0)
  {
    this_key->code = KEY_PRINT_SCREEN;
    return;
  }
  else if(strncmp(msg, cmd_SCROLLLOCK, strlen(cmd_SCROLLLOCK)) == 0)
  {
    this_key->code = KEY_SCROLL_LOCK;
    return;
  }
  else if(strncmp(msg, cmd_PAUSE, strlen(cmd_PAUSE)) == 0)
  {
    this_key->code = KEY_PAUSE;
    return;
  }
  else if(strncmp(msg, cmd_BREAK, strlen(cmd_BREAK)) == 0)
  {
    this_key->code = KEY_PAUSE;
    return;
  }
  else if(strncmp(msg, cmd_INSERT, strlen(cmd_INSERT)) == 0)
  {
    this_key->code = KEY_INSERT;
    return;
  }
  else if(strncmp(msg, cmd_HOME, strlen(cmd_HOME)) == 0)
  {
    this_key->code = KEY_HOME;
    return;
  }
  else if(strncmp(msg, cmd_PAGEUP, strlen(cmd_PAGEUP)) == 0)
  {
    this_key->code = KEY_PAGE_UP;
    return;
  }
  else if(strncmp(msg, cmd_PAGEDOWN, strlen(cmd_PAGEDOWN)) == 0)
  {
    this_key->code = KEY_PAGE_DOWN;
    return;
  }
  else if(strncmp(msg, cmd_DELETE, strlen(cmd_DELETE)) == 0)
  {
    this_key->code = KEY_DELETE;
    return;
  }
  else if(strncmp(msg, cmd_END, strlen(cmd_END)) == 0)
  {
    this_key->code = KEY_END;
    return;
  }

  else if(strncmp(msg, cmd_NUMLOCK, strlen(cmd_NUMLOCK)) == 0)
  {
    this_key->code = KEY_NUMLOCK;
    return;
  }

  else if(strncmp(msg, cmd_KPSLASH, strlen(cmd_KPSLASH)) == 0)
  {
    this_key->code = KEY_KPSLASH;
    return;
  }
  else if(strncmp(msg, cmd_KPASTERISK, strlen(cmd_KPASTERISK)) == 0)
  {
    this_key->code = KEY_KPASTERISK;
    return;
  }
  else if(strncmp(msg, cmd_KPMINUS, strlen(cmd_KPMINUS)) == 0)
  {
    this_key->code = KEY_KPMINUS;
    return;
  }
  else if(strncmp(msg, cmd_KPPLUS, strlen(cmd_KPPLUS)) == 0)
  {
    this_key->code = KEY_KPPLUS;
    return;
  }
  else if(strncmp(msg, cmd_KPENTER, strlen(cmd_KPENTER)) == 0)
  {
    this_key->code = KEY_KPENTER;
    return;
  }
  else if(strncmp(msg, cmd_KP1, strlen(cmd_KP1)) == 0)
  {
    this_key->code = KEY_KP1;
    return;
  }
  else if(strncmp(msg, cmd_KP2, strlen(cmd_KP2)) == 0)
  {
    this_key->code = KEY_KP2;
    return;
  }
  else if(strncmp(msg, cmd_KP3, strlen(cmd_KP3)) == 0)
  {
    this_key->code = KEY_KP3;
    return;
  }
  else if(strncmp(msg, cmd_KP4, strlen(cmd_KP4)) == 0)
  {
    this_key->code = KEY_KP4;
    return;
  }
  else if(strncmp(msg, cmd_KP5, strlen(cmd_KP5)) == 0)
  {
    this_key->code = KEY_KP5;
    return;
  }
  else if(strncmp(msg, cmd_KP6, strlen(cmd_KP6)) == 0)
  {
    this_key->code = KEY_KP6;
    return;
  }
  else if(strncmp(msg, cmd_KP7, strlen(cmd_KP7)) == 0)
  {
    this_key->code = KEY_KP7;
    return;
  }
  else if(strncmp(msg, cmd_KP8, strlen(cmd_KP8)) == 0)
  {
    this_key->code = KEY_KP8;
    return;
  }
  else if(strncmp(msg, cmd_KP9, strlen(cmd_KP9)) == 0)
  {
    this_key->code = KEY_KP9;
    return;
  }
  else if(strncmp(msg, cmd_KP0, strlen(cmd_KP0)) == 0)
  {
    this_key->code = KEY_KP0;
    return;
  }
  else if(strncmp(msg, cmd_KPDOT, strlen(cmd_KPDOT)) == 0)
  {
    this_key->code = KEY_KPDOT;
    return;
  }
  else if(strncmp(msg, cmd_KPEQUAL, strlen(cmd_KPEQUAL)) == 0)
  {
    this_key->code = KEY_KPEQUAL;
    return;
  }
  else if(strncmp(msg, cmd_POWER, strlen(cmd_POWER)) == 0)
  {
    this_key->code = KEY_POWER;
    return;
  }
  else if(msg[0] == 'F')
  {
    uint8_t f_number = atoi(msg+1);
    if(f_number == 0 || f_number > 24)
    {
      init_my_key(this_key);
      return;
    }
    this_key->code = f_key_lookup[f_number-1];
    return;
  }
  else if(strncmp(msg, cmd_MENU, strlen(cmd_MENU)) == 0)
  {
    this_key->code = KEY_MENU;
    return;
  }
  else if(strncmp(msg, cmd_APP, strlen(cmd_APP)) == 0)
  {
    this_key->code = KEY_MENU;
    return;
  }

// ----------------------------------
  this_key->key_type = KEY_TYPE_CHAR;
  if(strncmp(msg, cmd_SPACE, strlen(cmd_SPACE)) == 0)
  {
    this_key->code = ' ';
    return;
  }

// ----------------------------------

  this_key->key_type = KEY_TYPE_MODIFIER;
  if(strncmp(msg, cmd_SHIFT, strlen(cmd_SHIFT)) == 0)
  {
    this_key->code = KEY_LEFT_SHIFT;
    return;
  }
  if(strncmp(msg, cmd_RSHIFT, strlen(cmd_RSHIFT)) == 0)
  {
    this_key->code = KEY_RIGHT_SHIFT;
    return;
  }
  else if((strncmp(msg, cmd_ALT, strlen(cmd_ALT)) == 0) || strncmp(msg, cmd_OPTION, strlen(cmd_OPTION)) == 0)
  {
    this_key->code = KEY_LEFT_ALT;
    return;
  }
  else if((strncmp(msg, cmd_RALT, strlen(cmd_RALT)) == 0) || strncmp(msg, cmd_ROPTION, strlen(cmd_ROPTION)) == 0)
  {
    this_key->code = KEY_RIGHT_ALT;
    return;
  }
  else if((strncmp(msg, cmd_GUI, strlen(cmd_GUI)) == 0) || (strncmp(msg, cmd_WINDOWS, strlen(cmd_WINDOWS)) == 0) || (strncmp(msg, cmd_COMMAND, strlen(cmd_COMMAND)) == 0))
  {
    this_key->code = KEY_LEFT_GUI;
    return;
  }
  else if((strncmp(msg, cmd_RWINDOWS, strlen(cmd_RWINDOWS)) == 0) || (strncmp(msg, cmd_RCOMMAND, strlen(cmd_RCOMMAND)) == 0))
  {
    this_key->code = KEY_RIGHT_GUI;
    return;
  }
  else if((strncmp(msg, cmd_CTRL, strlen(cmd_CTRL)) == 0) || (strncmp(msg, cmd_CONTROL, strlen(cmd_CONTROL)) == 0))
  {
    this_key->code = KEY_LEFT_CTRL;
    return;
  }
  else if(strncmp(msg, cmd_RCTRL, strlen(cmd_RCTRL)) == 0)
  {
    this_key->code = KEY_RIGHT_CTRL;
    return;
  }

// ----------------------------------
  this_key->key_type = KEY_TYPE_MEDIA;
  if(strncmp(msg, cmd_MK_VOLUP, strlen(cmd_MK_VOLUP)) == 0)
  {
    this_key->code = KEY_MK_VOLUP;
    return;
  }
  else if(strncmp(msg, cmd_MK_VOLDOWN, strlen(cmd_MK_VOLDOWN)) == 0)
  {
    this_key->code = KEY_MK_VOLDOWN;
    return;
  }
  else if(strncmp(msg, cmd_MK_VOLMUTE, strlen(cmd_MK_VOLMUTE)) == 0)
  {
    this_key->code = KEY_MK_VOLMUTE;
    return;
  }
  else if(strncmp(msg, cmd_MK_PREV, strlen(cmd_MK_PREV)) == 0)
  {
    this_key->code = KEY_MK_PREV;
    return;
  }
  else if(strncmp(msg, cmd_MK_NEXT, strlen(cmd_MK_NEXT)) == 0)
  {
    this_key->code = KEY_MK_NEXT;
    return;
  }
  else if(strncmp(msg, cmd_MK_PLAYPAUSE, strlen(cmd_MK_PLAYPAUSE)) == 0)
  {
    this_key->code = KEY_MK_PLAYPAUSE;
    return;
  }
  else if(strncmp(msg, cmd_MK_VOLMUTE, strlen(cmd_MK_VOLMUTE)) == 0)
  {
    this_key->code = KEY_MK_VOLMUTE;
    return;
  }
  else if(strncmp(msg, cmd_MK_STOP, strlen(cmd_MK_STOP)) == 0)
  {
    this_key->code = KEY_MK_STOP;
    return;
  }

  // ----------------------------------
  this_key->key_type = KEY_TYPE_MOUSE_BUTTON;
  if(strncmp(msg, cmd_LMOUSE, strlen(cmd_LMOUSE)) == 0)
  {
    this_key->code = 1;
    return;
  }
  else if(strncmp(msg, cmd_RMOUSE, strlen(cmd_RMOUSE)) == 0)
  {
    this_key->code = 2;
    return;
  }
  else if(strncmp(msg, cmd_MMOUSE, strlen(cmd_MMOUSE)) == 0)
  {
    this_key->code = 4;
    return;
  }
  else if(strncmp(msg, cmd_MOUSE_MOVE, strlen(cmd_MOUSE_MOVE)) == 0)
  {
    char* msg_end = msg + strlen(msg);
    char* curr = goto_next_arg(msg, msg_end);
    this_key->code = atoi(curr);
    this_key->code2 = atoi(goto_next_arg(curr, msg_end));
    this_key->key_type = KEY_TYPE_MOUSE_MOVEMENT;
    return;
  }
  else if(strncmp(msg, cmd_MOUSE_WHEEL, strlen(cmd_MOUSE_WHEEL)) == 0)
  {
    this_key->code = atoi(goto_next_arg(msg, msg + strlen(msg)));
    this_key->key_type = KEY_TYPE_MOUSE_WHEEL;
    return;
  }
  init_my_key(this_key);
}

#define ACTION_PRESS_ONLY 0
#define ACTION_RELEASE_ONLY 1
#define ACTION_PRESS_RELEASE 2

/* able to press 6 keys at once
action type
0 press only
1 release only
2 press then release
*/
void parse_combo(char* line, my_key* first_key, uint8_t action_type)
{
  if(line == NULL || first_key == NULL)
    return;

  my_key key_1, key_2, key_3, key_4, key_5;
  char* line_end = line + strlen(line);
  char *arg1 = goto_next_arg(line, line_end);
  char *arg2 = goto_next_arg(arg1, line_end);
  char *arg3 = goto_next_arg(arg2, line_end);
  char *arg4 = goto_next_arg(arg3, line_end);
  char *arg5 = goto_next_arg(arg4, line_end);

  parse_special_key(arg1, &key_1);
  if(arg1 != NULL && key_1.key_type == KEY_TYPE_UNKNOWN)
  {
    key_1.key_type = KEY_TYPE_CHAR;
    key_1.code = arg1[0];
  }
  parse_special_key(arg2, &key_2);
  if(arg2 != NULL && key_2.key_type == KEY_TYPE_UNKNOWN)
  {
    key_2.key_type = KEY_TYPE_CHAR;
    key_2.code = arg2[0];
  }
  parse_special_key(arg3, &key_3);
  if(arg3 != NULL && key_3.key_type == KEY_TYPE_UNKNOWN)
  {
    key_3.key_type = KEY_TYPE_CHAR;
    key_3.code = arg3[0];
  }
  parse_special_key(arg4, &key_4);
  if(arg4 != NULL && key_4.key_type == KEY_TYPE_UNKNOWN)
  {
    key_4.key_type = KEY_TYPE_CHAR;
    key_4.code = arg4[0];
  }
  parse_special_key(arg5, &key_5);
  if(arg5 != NULL && key_5.key_type == KEY_TYPE_UNKNOWN)
  {
    key_5.key_type = KEY_TYPE_CHAR;
    key_5.code = arg5[0];
  }
  // ------ press --------
  if(action_type == ACTION_PRESS_ONLY || action_type == ACTION_PRESS_RELEASE)
  {
    keyboard_press(first_key, 0);
    delay_wrapper(char_delay, char_delay_fuzz);
    if(arg1 != NULL)
    {
      keyboard_press(&key_1, 0);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg2 != NULL)
    {
      keyboard_press(&key_2, 0);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg3 != NULL)
    {
      keyboard_press(&key_3, 0);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg4 != NULL)
    {
      keyboard_press(&key_4, 0);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg5 != NULL)
    {
      keyboard_press(&key_5, 0);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
  }
  // ------ release --------
  if(action_type == ACTION_RELEASE_ONLY || action_type == ACTION_PRESS_RELEASE)
  {
    if(arg5 != NULL)
    {
      keyboard_release(&key_5);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg4 != NULL)
    {
      keyboard_release(&key_4);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg3 != NULL)
    {
      keyboard_release(&key_3);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg2 != NULL)
    {
      keyboard_release(&key_2);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    if(arg1 != NULL)
    {
      keyboard_release(&key_1);
      delay_wrapper(char_delay, char_delay_fuzz);
    }
    keyboard_release(first_key);
    delay_wrapper(char_delay, char_delay_fuzz);
  }
}

int32_t get_arg(char* line)
{
  return atoi(goto_next_arg(line, line + strlen(line)));
}

uint8_t is_empty_line(char* line)
{
  for (int i = 0; i < strlen(line); ++i)
    if(line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\r')
      return 0;
  return 1;
}

uint8_t parse_hold(char* line, uint8_t keynum)
{
  if(line == NULL)
    return PARSE_ERROR;

  my_key key_1, key_2;
  char* line_end = line + strlen(line);
  char *arg1 = goto_next_arg(line, line_end);
  char *arg2 = goto_next_arg(arg1, line_end);
  
  parse_special_key(arg1, &key_1);
  if(key_1.key_type == KEY_TYPE_UNKNOWN)
  {
    key_1.key_type = KEY_TYPE_CHAR;
    key_1.code = arg1[0];
  }
  hold_cache[keynum].key_type = key_1.key_type;
  hold_cache[keynum].code = key_1.code;

  if(arg2 != NULL)
  {
    parse_special_key(arg2, &key_2);
    if(key_2.key_type == KEY_TYPE_UNKNOWN)
    {
      key_2.key_type = KEY_TYPE_CHAR;
      key_2.code = arg2[0];
    }
    hold_cach2[keynum].key_type = key_2.key_type;
    hold_cach2[keynum].code = key_2.code;
  }

  keyboard_press(&key_1, 0);
  osDelay(DEFAULT_CHAR_DELAY_MS);
  if(arg2 != NULL)
  {
    keyboard_press(&key_2, 0);
    osDelay(DEFAULT_CHAR_DELAY_MS);
  }
  return PARSE_OK;
}

uint8_t parse_line(char* line, uint8_t keynum)
{
  uint8_t result = PARSE_OK;

  // cut off at line ending
  for(int i = 0; i < strlen(line); ++i)
    if(line[i] == '\r' || line[i] == '\n')
      line[i] = 0;
  // printf("this line: %s\n", line);
  char* line_end = line + strlen(line);

  if(strncmp(cmd_OLD_HOLD, line, strlen(cmd_OLD_HOLD)) == 0)
  {
    result = parse_hold(line, keynum);
    goto parse_end;
  }

  my_key this_key;
  parse_special_key(line, &this_key);
  if(is_empty_line(line))
    result = PARSE_EMPTY_LINE;
  else if(is_mouse_type(&this_key))
  {
    keyboard_press(&this_key, 0);
    delay_wrapper(cmd_delay, cmd_delay_fuzz);
    keyboard_release(&this_key);
    delay_wrapper(cmd_delay, cmd_delay_fuzz);
  }
  else if(this_key.key_type != KEY_TYPE_UNKNOWN)
    parse_combo(line, &this_key, ACTION_PRESS_RELEASE);
  else if(strncmp(cmd_REM, line, strlen(cmd_REM)) == 0)
    ;
  else if(strncmp(cmd_LCR, line, strlen(cmd_LCR)) == 0)
  {
    char* curr = line;
    char* msg_end = line + PATH_SIZE;
    curr = goto_next_arg(curr, msg_end);
    uint8_t keynum_to_reset = atoi(curr);
    curr = goto_next_arg(curr, msg_end);
    uint8_t press_count_val = atoi(curr);
    // printf("$%d %d\n", keynum_to_reset, press_count_val);
    if(keynum_to_reset == 0)
      memset(key_press_count, 0, MAPPABLE_KEY_COUNT);
    keynum_to_reset--;
    if(keynum_to_reset < MAPPABLE_KEY_COUNT)
      key_press_count[keynum_to_reset] = press_count_val;
    result = PARSE_LOOP_STATE_SAVE_NEEDED;
    goto parse_end;
  }
  else if(strncmp(cmd_HOLD, line, strlen(cmd_HOLD)) == 0)
  {
    parse_special_key(line + strlen(cmd_HOLD), &this_key);
    if(this_key.key_type == KEY_TYPE_UNKNOWN)
    {
      this_key.key_type = KEY_TYPE_CHAR;
      this_key.code = (line + strlen(cmd_HOLD))[0];
    }
    parse_combo(line + strlen(cmd_HOLD), &this_key, ACTION_PRESS_ONLY);
  }
  else if(strncmp(cmd_RELEASE, line, strlen(cmd_RELEASE)) == 0)
  {
    parse_special_key(line + strlen(cmd_RELEASE), &this_key);
    if(this_key.key_type == KEY_TYPE_UNKNOWN)
    {
      this_key.key_type = KEY_TYPE_CHAR;
      this_key.code = (line + strlen(cmd_RELEASE))[0];
    }
    parse_combo(line + strlen(cmd_RELEASE), &this_key, ACTION_RELEASE_ONLY);
  }
  else if(strncmp(cmd_STRING, line, strlen(cmd_STRING)) == 0)
  {
    kb_print(line + strlen(cmd_STRING), char_delay, char_delay_fuzz);
  }
  else if(strncmp(cmd_STRINGLN, line, strlen(cmd_STRING)) == 0)
  {
    kb_print(line + strlen(cmd_STRINGLN), char_delay, char_delay_fuzz);
    this_key.key_type = KEY_TYPE_SPECIAL;
    this_key.code = KEY_RETURN;
    kb_print_char(&this_key, char_delay, char_delay_fuzz);
  }
  else if(strncmp(cmd_UARTPRINT, line, strlen(cmd_UARTPRINT)) == 0)
  {
    printf("UART %s\n", line + strlen(cmd_UARTPRINT));
    osDelay(25);
  }
  else if(strncmp(cmd_INJECT_MOD, line, strlen(cmd_INJECT_MOD)) == 0)
  {
    ; // do nothing since modifier key already works on its own
  }
  else if(is_sw_color_line(line) == 1)
  {
    uint8_t this_key_num = atoi(line + strlen(cmd_SWCOLOR)) - 1;
    if(this_key_num > MAX_PROFILES)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    assign_colors(this_key_num, line, line_end);
    redraw_bg();
    delay_wrapper(char_delay, char_delay_fuzz);
    goto parse_end;
  }
  else if(is_sw_color_line(line) == 2)
  {
    assign_colors(keynum, line, line_end);
    p_cache.individual_keydown_color[keynum][0] = p_cache.individual_key_color[keynum][0];
    p_cache.individual_keydown_color[keynum][1] = p_cache.individual_key_color[keynum][1];
    p_cache.individual_keydown_color[keynum][2] = p_cache.individual_key_color[keynum][2];
    redraw_bg();
    delay_wrapper(char_delay, char_delay_fuzz);
    goto parse_end;
  }
  else if(strncmp(cmd_DELAY, line, strlen(cmd_DELAY)) == 0)
  {
    int32_t argg = get_arg(line);
    if(argg == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    osDelay(argg);
  }
  else if(strncmp(cmd_DEFAULTDELAY, line, strlen(cmd_DEFAULTDELAY)) == 0)
  {
    int32_t argg = get_arg(line);
    if(argg == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    cmd_delay = argg;
  }
  else if(strncmp(cmd_DEFAULTCHARDELAY, line, strlen(cmd_DEFAULTCHARDELAY)) == 0)
  {
    int32_t argg = get_arg(line);
    if(argg == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    char_delay = argg;
  }
  else if(strncmp(cmd_DEFAULTDELAYFUZZ, line, strlen(cmd_DEFAULTDELAYFUZZ)) == 0)
  {
    cmd_delay_fuzz = get_arg(line);
  }
  else if(strncmp(cmd_DEFAULTCHARDELAYFUZZ, line, strlen(cmd_DEFAULTCHARDELAYFUZZ)) == 0)
  {
    char_delay_fuzz = get_arg(line);
  }
  else
    result = PARSE_ERROR;

  parse_end:
  if(result == PARSE_OK)
    delay_wrapper(cmd_delay, cmd_delay_fuzz);
  return result;
}

void keypress_wrap(uint8_t keynum)
{
  uint16_t line_num = 0;
  uint8_t result;
  uint8_t found_start = 0;
  uint8_t need_to_save_loop_state = 0;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/key%d.txt", p_cache.profile_fn, keynum+1);
  // printf("%s\n", temp_buf);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto kp_end;
  cmd_delay = DEFAULT_CMD_DELAY_MS;
  char_delay = DEFAULT_CHAR_DELAY_MS;
  cmd_delay_fuzz = 0;
  char_delay_fuzz = 0;
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    line_num++;
    // if this key has loops, keep going until we are at the correct starting point
    if(key_max_loop[keynum] != 0 && found_start == 0)
    {
      uint8_t current_loop = key_press_count[keynum] % key_max_loop[keynum];
      memset(temp_buf, 0, PATH_SIZE);
      sprintf(temp_buf, "%s%d", cmd_LOOP, current_loop);
      if(strncmp(read_buffer, temp_buf, strlen(temp_buf)) != 0)
        continue;
      found_start = 1;
      continue;
    }
    // stop at the next loop
    if(strncmp(cmd_LOOP, read_buffer, strlen(cmd_LOOP)) == 0)
    {
      goto kp_end;
    }
    if(strncmp(cmd_REPEAT, read_buffer, strlen(cmd_REPEAT)) == 0)
    {
      uint8_t repeats = atoi(goto_next_arg(read_buffer, read_buffer + READ_BUF_SIZE));
      for (int i = 0; i < repeats; ++i)
        parse_line(prev_line, keynum);
      continue;
    }
    if(strncmp(cmd_SLEEP, read_buffer, strlen(cmd_SLEEP)) == 0)
    {
      my_dpc.type = DPC_SLEEP;
      goto kp_end;
    }
    if(strncmp(cmd_PREV_PROFILE, read_buffer, strlen(cmd_PREV_PROFILE)) == 0)
    {
      my_dpc.type = DPC_PREV_PROFILE;
      goto kp_end;
    }
    if(strncmp(cmd_NEXT_PROFILE, read_buffer, strlen(cmd_NEXT_PROFILE)) == 0)
    {
      my_dpc.type = DPC_NEXT_PROFILE;
      goto kp_end;
    }
    if(strncmp(cmd_GOTO_PROFILE, read_buffer, strlen(cmd_GOTO_PROFILE)) == 0)
    {
      my_dpc.type = DPC_GOTO_PROFILE;
      my_dpc.data = atoi(goto_next_arg(read_buffer, read_buffer + READ_BUF_SIZE));
      goto kp_end;
    }
    result = parse_line(read_buffer, keynum);
    if(result == PARSE_ERROR)
    {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(5, 0);
      ssd1306_WriteString("Parse error in key:", Font_6x10,White);
      ssd1306_SetCursor(5, 12);
      ssd1306_WriteString(p_cache.key_fn[keynum], Font_6x10,White);
      memset(temp_buf, 0, PATH_SIZE);
      sprintf(temp_buf, "line %d:", line_num);
      ssd1306_SetCursor(5, 30);
      ssd1306_WriteString(temp_buf, Font_6x10,White);
      read_buffer[21] = 0;
      ssd1306_SetCursor(5, 42);
      ssd1306_WriteString(read_buffer, Font_6x10,White);
      ssd1306_UpdateScreen();
      error_animation(0);
      osDelay(10000);
      error_animation(1);
      print_legend(0, 0);
      goto kp_end;
    }
    else if(result == PARSE_OK)
    {
      memset(prev_line, 0, READ_BUF_SIZE);
      strcpy(prev_line, read_buffer);
    }
    else if(result == PARSE_LOOP_STATE_SAVE_NEEDED)
    {
      need_to_save_loop_state = 1;
    }
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  kp_end:
  f_close(&sd_file);
  key_press_count[keynum]++;

  if(need_to_save_loop_state || key_max_loop[keynum] != 0)
    save_loop_state();
}

void save_loop_state(void)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/state.txt", p_cache.profile_fn);
  if(f_open(&sd_file, temp_buf, FA_CREATE_ALWAYS | FA_WRITE) != 0)
  {
    f_close(&sd_file);
    return;
  }
  for(uint8_t iii = 0; iii < MAPPABLE_KEY_COUNT; iii++)
  {
    if(key_max_loop[iii] == 0)
      continue;
    memset(temp_buf, 0, PATH_SIZE);
    // ls = loop state, cs = colour state
    sprintf(temp_buf, "ls %d %d\ncs %d %d %d %d\n", iii, key_press_count[iii] % key_max_loop[iii], iii, p_cache.individual_key_color[iii][0], p_cache.individual_key_color[iii][1], p_cache.individual_key_color[iii][2]);
    f_write(&sd_file, temp_buf, strlen(temp_buf), &bytes_read);
    // printf("%s", temp_buf);
  }
  f_close(&sd_file);
}

void dpc_init(duckypad_parsed_command* dpc)
{
  dpc->type = DPC_NONE;
  dpc->data = 0;
}

void handle_keypress(uint8_t keynum, but_status* b_status)
{
  dpc_init(&my_dpc);
  keypress_wrap(keynum);
  // don't repeat if this key asks to sleep or change profiles
  if(my_dpc.type != DPC_NONE)
    return;
  // don't repeat if this key is HOLD command
  if(hold_cache[keynum].key_type != KEY_TYPE_UNKNOWN && hold_cache[keynum].code != 0)
    return;
  // wait 500ms
  uint32_t hold_start = HAL_GetTick();
  while(1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    keyboard_update();
    if(b_status->button_state == BUTTON_RELEASED)
      return;
    if(HAL_GetTick() - hold_start > 500)
      break;
  }

  // start repeating
  while(1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    keyboard_update();
    keypress_wrap(keynum);
    if(b_status->button_state == BUTTON_RELEASED)
      return;
  }
}
