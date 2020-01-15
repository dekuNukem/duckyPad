#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shared.h"
#include "buttons.h"
#include "parser.h"
#include "ssd1306.h"
#include "keyboard.h"
#include "animations.h"

#define DEFAULT_CMD_DELAY_MS 10
#define DEFAULT_CHAR_DELAY_MS 10

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
unsigned int ignore_this;

uint8_t available_profile[MAX_PROFILES];
char temp_buf[PATH_SIZE];
char lfn_buf[FILENAME_SIZE];
char key_name_buf[FILENAME_SIZE];
char read_buffer[READ_BUF_SIZE];
char prev_line[READ_BUF_SIZE];
char nonexistent_keyname[] = "-";
profile_cache p_cache;

const char cmd_NAME[] = "NAME ";
const char cmd_REPEAT[] = "REPEAT ";
const char cmd_REM[] = "REM ";
const char cmd_C_COMMENT[] = "// ";
const char cmd_DEFAULTDELAY[] = "DEFAULTDELAY ";
const char cmd_DEFAULTCHARDELAY[] = "DEFAULTCHARDELAY ";
const char cmd_DELAY[] = "DELAY ";
const char cmd_STRING[] = "STRING ";
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
const char cmd_F1[] = "F1";
const char cmd_F2[] = "F2";
const char cmd_F3[] = "F3";
const char cmd_F4[] = "F4";
const char cmd_F5[] = "F5";
const char cmd_F6[] = "F6";
const char cmd_F7[] = "F7";
const char cmd_F8[] = "F8";
const char cmd_F9[] = "F9";
const char cmd_F10[] = "F10";
const char cmd_F11[] = "F11";
const char cmd_F12[] = "F12";
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
const char cmd_ALT[] = "ALT";
const char cmd_GUI[] = "GUI";
const char cmd_WINDOWS[] = "WINDOWS";
const char cmd_CONTROL[] = "CONTROL";
const char cmd_CTRL[] = "CTRL";
const char cmd_BG_COLOR[] = "BG_COLOR ";
const char cmd_KD_COLOR[] = "KEYDOWN_COLOR ";
const char cmd_VOLUP[] = "VOLUP";
const char cmd_VOLDOWN[] = "VOLDOWN";
const char cmd_VOLMUTE[] = "MUTE";

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

char* get_keyname(char* pf_fn, uint8_t keynum)
{
  char* key_fn;
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
      memset(key_name_buf, 0, FILENAME_SIZE);
      strncpy(key_name_buf, key_fn, FILENAME_SIZE);
      ret = key_name_buf;
      break;
    }
  }
  f_closedir(&dir);
  return ret;
}

uint8_t load_colors(char* pf_fn)
{
  char *curr, *msg_end;
  uint8_t ret;

  bg_color[0] = DEFAULT_BG_RED;
  bg_color[1] = DEFAULT_BG_GREEN;
  bg_color[2] = DEFAULT_BG_BLUE;
  keydown_color[0] = DEFAULT_KD_RED;
  keydown_color[1] = DEFAULT_KD_GREEN;
  keydown_color[2] = DEFAULT_KD_BLUE;

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
      bg_color[0] = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      bg_color[1] = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      bg_color[2] = atoi(curr);
    }
    if(strncmp(cmd_KD_COLOR, read_buffer, strlen(cmd_KD_COLOR)) == 0)
    {
      curr = goto_next_arg(curr, msg_end);
      keydown_color[0] = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      keydown_color[1] = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      keydown_color[2] = atoi(curr);
    }
  }
  ret = PARSE_OK;
  color_end:
  f_close(&sd_file);
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
    memset(p_cache.key_fn[i], 0, FILENAME_SIZE);
    strcpy(p_cache.key_fn[i], get_keyname(p_cache.profile_fn, i));
  }
  load_colors(p_cache.profile_fn);
  change_bg();
  p_cache.current_profile = pid;
}

void print_keyname(char* keyname, uint8_t keynum)
{
  char* start;
  char* end;
  memset(temp_buf, 0, FILENAME_SIZE);
  sprintf(temp_buf, "key%d_", keynum + 1);
  start = keyname + strlen(temp_buf);
  end = strstr(start, ".txt");
  if(end == NULL)
  {
    start = keyname;
    end = keyname + strlen(keyname);
  }
  memset(key_name_buf, 0, FILENAME_SIZE);
  strncpy(key_name_buf, start, end - start);
  if(strlen(key_name_buf) > 7)
    key_name_buf[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  uint8_t x_start = col_lookup[strlen(key_name_buf) - 1][col];
  ssd1306_SetCursor(x_start, (row + 1) * 10 + 3);
  ssd1306_WriteString(key_name_buf, Font_6x10,White);
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
  uint8_t x_start = (21 - strlen(temp_buf)) * 3;
  ssd1306_SetCursor(x_start, 0);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    print_keyname(p_cache.key_fn[i], i);
  ssd1306_UpdateScreen();
}

void save_last_profile(uint8_t profile_id)
{
  if(f_open(&sd_file, "last_profile.kbd", FA_CREATE_ALWAYS | FA_WRITE) != 0)
    goto slp_end;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "%d\n", profile_id);
  f_write(&sd_file, temp_buf, strlen(temp_buf), &ignore_this);
	slp_end:
  f_close(&sd_file);
}

uint8_t get_last_profile(void)
{
  uint8_t ret = 0;
  if(f_open(&sd_file, "last_profile.kbd", FA_READ) != 0)
    goto glp_end;
  memset(temp_buf, 0, PATH_SIZE);
  while(f_gets(temp_buf, PATH_SIZE, &sd_file))
    ret = atoi(temp_buf);

  if(available_profile[ret] == 0)
    ret = 0;
  glp_end:
  f_close(&sd_file);
  return ret;
}

void restore_profile(uint8_t profile_id)
{
  load_profile(profile_id);
  print_legend();
  f_closedir(&dir);
  f_close(&sd_file);
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
    ssd1306_SetCursor(10, 13);
    ssd1306_WriteString("No Valid Profiles",Font_6x10,White);
    ssd1306_SetCursor(0, 37);
    ssd1306_WriteString(instruction,Font_6x10,White);
    ssd1306_SetCursor(25, 50);
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
    if(available_profile[next_profile])
      break;
  }
  load_profile(next_profile);
  print_legend();
  f_closedir(&dir);
  f_close(&sd_file);
  save_last_profile(next_profile);
}

uint8_t parse_special_key(char* msg)
{
  if(msg == NULL)
    return 0;
  if(strncmp(msg, cmd_F1, strlen(cmd_F1)) == 0)
    return KEY_F1;
  else if(strncmp(msg, cmd_F2, strlen(cmd_F2)) == 0)
    return KEY_F2;
  else if(strncmp(msg, cmd_F3, strlen(cmd_F3)) == 0)
    return KEY_F3;
  else if(strncmp(msg, cmd_F4, strlen(cmd_F4)) == 0)
    return KEY_F4;
  else if(strncmp(msg, cmd_F5, strlen(cmd_F5)) == 0)
    return KEY_F5;
  else if(strncmp(msg, cmd_F6, strlen(cmd_F6)) == 0)
    return KEY_F6;
  else if(strncmp(msg, cmd_F7, strlen(cmd_F7)) == 0)
    return KEY_F7;
  else if(strncmp(msg, cmd_F8, strlen(cmd_F8)) == 0)
    return KEY_F8;
  else if(strncmp(msg, cmd_F9, strlen(cmd_F9)) == 0)
    return KEY_F9;
  else if(strncmp(msg, cmd_F10, strlen(cmd_F10)) == 0)
    return KEY_F10;
  else if(strncmp(msg, cmd_F11, strlen(cmd_F11)) == 0)
    return KEY_F11;
  else if(strncmp(msg, cmd_F12, strlen(cmd_F12)) == 0)
    return KEY_F12;
  else if(strncmp(msg, cmd_UP, strlen(cmd_UP)) == 0)
    return KEY_UP_ARROW;
  else if(strncmp(msg, cmd_DOWN, strlen(cmd_DOWN)) == 0)
    return KEY_DOWN_ARROW;
  else if(strncmp(msg, cmd_LEFT, strlen(cmd_LEFT)) == 0)
    return KEY_LEFT_ARROW;
  else if(strncmp(msg, cmd_RIGHT, strlen(cmd_RIGHT)) == 0)
    return KEY_RIGHT_ARROW;
  else if(strncmp(msg, cmd_UPARROW, strlen(cmd_UPARROW)) == 0)
    return KEY_UP_ARROW;
  else if(strncmp(msg, cmd_DOWNARROW, strlen(cmd_DOWNARROW)) == 0)
    return KEY_DOWN_ARROW;
  else if(strncmp(msg, cmd_LEFTARROW, strlen(cmd_LEFTARROW)) == 0)
    return KEY_LEFT_ARROW;
  else if(strncmp(msg, cmd_RIGHTARROW, strlen(cmd_RIGHTARROW)) == 0)
    return KEY_RIGHT_ARROW;
  else if(strncmp(msg, cmd_ESCAPE, strlen(cmd_ESCAPE)) == 0)
    return KEY_ESC;
  else if(strncmp(msg, cmd_ESC, strlen(cmd_ESC)) == 0)
    return KEY_ESC;
  else if(strncmp(msg, cmd_ENTER, strlen(cmd_ENTER)) == 0)
    return KEY_RETURN;
  else if(strncmp(msg, cmd_BACKSPACE, strlen(cmd_BACKSPACE)) == 0)
    return KEY_BACKSPACE;
  else if(strncmp(msg, cmd_TAB, strlen(cmd_TAB)) == 0)
    return KEY_TAB;
  else if(strncmp(msg, cmd_CAPSLOCK, strlen(cmd_CAPSLOCK)) == 0)
    return KEY_CAPS_LOCK;
  else if(strncmp(msg, cmd_PRINTSCREEN, strlen(cmd_PRINTSCREEN)) == 0)
    return KEY_PRINT_SCREEN;
  else if(strncmp(msg, cmd_SCROLLLOCK, strlen(cmd_SCROLLLOCK)) == 0)
    return KEY_SCROLL_LOCK;
  else if(strncmp(msg, cmd_PAUSE, strlen(cmd_PAUSE)) == 0)
    return KEY_PAUSE;
  else if(strncmp(msg, cmd_BREAK, strlen(cmd_BREAK)) == 0)
    return KEY_PAUSE;
  else if(strncmp(msg, cmd_INSERT, strlen(cmd_INSERT)) == 0)
    return KEY_INSERT;
  else if(strncmp(msg, cmd_HOME, strlen(cmd_HOME)) == 0)
    return KEY_HOME;
  else if(strncmp(msg, cmd_PAGEUP, strlen(cmd_PAGEUP)) == 0)
    return KEY_PAGE_UP;
  else if(strncmp(msg, cmd_PAGEDOWN, strlen(cmd_PAGEDOWN)) == 0)
    return KEY_PAGE_DOWN;
  else if(strncmp(msg, cmd_DELETE, strlen(cmd_DELETE)) == 0)
    return KEY_DELETE;
  else if(strncmp(msg, cmd_END, strlen(cmd_END)) == 0)
    return KEY_END;
  else if(strncmp(msg, cmd_SHIFT, strlen(cmd_SHIFT)) == 0)
    return KEY_LEFT_SHIFT;
  else if(strncmp(msg, cmd_ALT, strlen(cmd_ALT)) == 0)
    return KEY_LEFT_ALT;
  else if(strncmp(msg, cmd_GUI, strlen(cmd_GUI)) == 0)
    return KEY_LEFT_GUI;
  else if(strncmp(msg, cmd_WINDOWS, strlen(cmd_WINDOWS)) == 0)
    return KEY_LEFT_GUI;
  else if(strncmp(msg, cmd_CONTROL, strlen(cmd_CONTROL)) == 0)
    return KEY_LEFT_CTRL;
  else if(strncmp(msg, cmd_CTRL, strlen(cmd_CTRL)) == 0)
    return KEY_LEFT_CTRL;
  else if(strncmp(msg, cmd_SPACE, strlen(cmd_SPACE)) == 0)
    return ' ';
  else if(strncmp(msg, cmd_VOLUP, strlen(cmd_VOLUP)) == 0)
    return KEY_VOL_UP;
  else if(strncmp(msg, cmd_VOLDOWN, strlen(cmd_VOLDOWN)) == 0)
    return KEY_VOL_DOWN;
  else if(strncmp(msg, cmd_VOLMUTE, strlen(cmd_VOLMUTE)) == 0)
    return KEY_VOL_MUTE;
  return 0;
}

void parse_combo(char* line, uint8_t key)
{
  uint8_t spk1 = 0;
  uint8_t spk2 = 0;
  char* line_end = line + strlen(line);
  char *arg1 = goto_next_arg(line, line_end);
  char *arg2 = goto_next_arg(arg1, line_end);

  spk1 = parse_special_key(arg1);
  if(arg1 != NULL && spk1 == 0)
    spk1 = arg1[0];

  spk2 = parse_special_key(arg2);
  if(arg2 != NULL && spk2 == 0)
    spk2 = arg2[0];

  keyboard_press(key, 1);
  osDelay(char_delay);
  if(arg1 != NULL)
  {
    keyboard_press(spk1, 0);
    osDelay(char_delay);
  }
  if(arg2 != NULL)
  {
    keyboard_press(spk2, 0);
    osDelay(char_delay);
  }
  keyboard_release_all();
  osDelay(char_delay);
}

uint16_t get_arg(char* line)
{
  char* arg = goto_next_arg(line, line + strlen(line));
  if(arg == NULL)
    return PARSE_ERROR;
  return atoi(arg);
}

uint8_t parse_line(char* line)
{
  uint8_t result = PARSE_OK;

  for (int i = 0; i < strlen(line); ++i)
    if(line[i] == '\r' || line[i] == '\n')
      line[i] = 0;

  char* line_end = line + strlen(line);
  char spk = parse_special_key(line);

  if(spk != 0)
    parse_combo(line, spk);
  else if(strncmp(cmd_NAME, line, strlen(cmd_NAME)) == 0)
    ;
  else if(strncmp(cmd_REM, line, strlen(cmd_REM)) == 0)
    ;
  else if(strncmp(cmd_C_COMMENT, line, strlen(cmd_C_COMMENT)) == 0)
    ;
  else if(strncmp(cmd_STRING, line, strlen(cmd_STRING)) == 0)
    kb_print(line + strlen(cmd_STRING), char_delay);
  else if(strncmp(cmd_DELAY, line, strlen(cmd_DELAY)) == 0)
  {
    uint16_t argg = get_arg(line);
    if(argg == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    osDelay(argg);
  }
  else if(strncmp(cmd_DEFAULTDELAY, line, strlen(cmd_DEFAULTDELAY)) == 0)
  {
    uint16_t argg = get_arg(line);
    if(argg == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    cmd_delay = argg;
  }
  else if(strncmp(cmd_DEFAULTCHARDELAY, line, strlen(cmd_DEFAULTCHARDELAY)) == 0)
  {
    uint16_t argg = get_arg(line);
    if(argg == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    char_delay = argg;
  }
  else if(strlen(line) <= 2)
    result = PARSE_EMPTY_LINE;
  else
    result = PARSE_ERROR;

  parse_end:
  if(result == PARSE_OK)
    osDelay(cmd_delay);
  return result;
}

void handle_keypress(uint8_t keynum)
{
  uint16_t line_num = 0;
  uint8_t result;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/%s", p_cache.profile_fn, p_cache.key_fn[keynum]);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto kp_end;

  cmd_delay = DEFAULT_CMD_DELAY_MS;
  char_delay = DEFAULT_CHAR_DELAY_MS;
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    line_num++;
    if(strncmp(cmd_REPEAT, read_buffer, strlen(cmd_REPEAT)) == 0)
    {
      uint8_t repeats = atoi(goto_next_arg(read_buffer, read_buffer + strlen(read_buffer)));
      for (int i = 0; i < repeats; ++i)
        parse_line(prev_line);
      continue;
    }
    result = parse_line(read_buffer);
    if(result == PARSE_ERROR)
    {
      error_animation(0);
      ssd1306_Fill(Black);
      ssd1306_SetCursor(5, 0);
      ssd1306_WriteString("Parse error in", Font_6x10,White);
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
      osDelay(10000);
      error_animation(1);
      print_legend();
      goto kp_end;
    }
    else if(result == PARSE_OK)
    {
      memset(prev_line, 0, READ_BUF_SIZE);
      strcpy(prev_line, read_buffer);
    }
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  kp_end:
  f_close(&sd_file);
}
