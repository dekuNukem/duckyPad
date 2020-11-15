#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_tasks.h"
#include "shared.h"
#include "usbd_hid.h"
#include "ssd1306.h"
#include "fonts.h"
#include "neopixel.h"
#include "buttons.h"
#include "keyboard.h"
#include "parser.h"
#include "animations.h"

#define LONG_PRESS_MS 500
#define MAX_KEYMAP_SIZE 8

uint8_t init_complete;
uint32_t last_keypress;
uint32_t next_pixel_shift = 30000;
uint8_t is_sleeping, is_in_settings;
uint32_t button_hold_start, button_hold_duration;
keymap_cache my_keymap_cache[MAX_KEYMAP_SIZE];
char default_str[] = "default";

void draw_brightness_value()
{
    memset(temp_buf, 0, PATH_SIZE);
    sprintf(temp_buf, " %d%% ", brightness_values[brightness_index]);
    ssd1306_SetCursor((128 - strlen(temp_buf)*6)/2, 30);
    ssd1306_WriteString(temp_buf,Font_6x10,White);
    ssd1306_UpdateScreen();
}

void service_all(void)
{
  for (int i = 0; i < KEY_COUNT; ++i)
    service_press(&button_status[i]);
}

void set_brightness(void)
{
    draw_brightness_value();
    redraw_bg();
    osDelay(30);
    service_all();
    save_last_profile(p_cache.current_profile);
}

void change_brightness()
{   
    redraw_bg();
    osDelay(30);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(34, 8);
    ssd1306_WriteString("Brightness",Font_6x10,White);
    ssd1306_SetCursor(25, 30);
    ssd1306_WriteString("-",Font_6x10,White);
    ssd1306_SetCursor(98, 30);
    ssd1306_WriteString("+",Font_6x10,White);
    ssd1306_SetCursor(1, 53);
    ssd1306_WriteString("Press any key to exit",Font_6x10,White);
    ssd1306_UpdateScreen();
    draw_brightness_value();
    service_all();
    while(1)
    {
      HAL_IWDG_Refresh(&hiwdg);
      keyboard_update();

      for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
          if(is_pressed(&button_status[i]))
              return;

      if(is_pressed(&button_status[KEY_BUTTON1])) // -
      {
          brightness_index--;
          if(brightness_index < 0)
              brightness_index = 0;
          set_brightness();
      }
      if(is_pressed(&button_status[KEY_BUTTON2])) // +
      {
          brightness_index++;
          if(brightness_index >= BRIGHTNESS_LEVELS)
              brightness_index = BRIGHTNESS_LEVELS - 1;
          set_brightness();
      }
      osDelay(50);
    }
}

#define MAX_PQS_PAGES 2
void profile_quickswitch(void)
{
  int8_t pqs_page = p_cache.current_profile / MAPPABLE_KEY_COUNT;
  if(pqs_page >= MAX_PQS_PAGES)
    pqs_page = MAX_PQS_PAGES - 1;
  profile_quickswitch_animation();
  list_profiles(pqs_page);
  service_all();

  while(1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    keyboard_update();

    for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
      if(is_pressed(&button_status[i]))
      {
        uint8_t this_profile = pqs_page * MAPPABLE_KEY_COUNT + i + 1;
        if(p_cache.available_profile[this_profile])
        {
          restore_profile(this_profile);
          return;
        }
        service_all();
      }
      
    if(is_pressed(&button_status[KEY_BUTTON1])) // -
    {
        pqs_page--;
        if(pqs_page < 0)
            pqs_page = MAX_PQS_PAGES-1;
        list_profiles(pqs_page);
        service_all();
    }
    if(is_pressed(&button_status[KEY_BUTTON2])) // +
    {
        pqs_page++;
        if(pqs_page >= MAX_PQS_PAGES)
            pqs_page = 0;
        list_profiles(pqs_page);
        service_all();
    }
    osDelay(50);
  }
}

void handle_button_press(uint8_t button_num)
{
    button_hold_start = HAL_GetTick();
    while(1)
    {
        HAL_IWDG_Refresh(&hiwdg);
        keyboard_update();
        button_hold_duration = HAL_GetTick() - button_hold_start;
        if(button_status[button_num].button_state == BUTTON_RELEASED)
            break;
        if(button_hold_duration > LONG_PRESS_MS)
            break;
    }
    keyboard_release_all();
    media_key_release();
    if(button_hold_duration < LONG_PRESS_MS) // short press
    {
        if(button_num == KEY_BUTTON1) // -
          change_profile(PREV_PROFILE);
        else if(button_num == KEY_BUTTON2) // +
          change_profile(NEXT_PROFILE);
    }
    else // long press
    {
      is_in_settings = 1;
      if(button_num == KEY_BUTTON1) // -
      {
        change_brightness();
        save_settings();
      }
      else if(button_num == KEY_BUTTON2) // +
      {
        profile_quickswitch();
      }

      is_in_settings = 0;
      print_legend(0, 0);
      service_all();
    }
}

void scan_keymaps(void)
{
  char* keymap_filename;
  fno.lfname = lfn_buf;
  fno.lfsize = FILENAME_SIZE - 1;
  if (f_opendir(&dir, "/keymaps") != FR_OK)
    goto scan_keymaps_end;

  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "dpkm_");
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if(fno.fattrib & AM_DIR)
      continue;

    keymap_filename = fno.lfname[0] ? fno.lfname : fno.fname;
    if(strncmp(temp_buf, keymap_filename, strlen(temp_buf)) != 0)
      continue;

    if(strncmp(keymap_filename + strlen(keymap_filename) - 4, ".txt", 4) != 0)
      continue;

    for (int i = 0; i < MAX_KEYMAP_SIZE; ++i)
    {
      if(my_keymap_cache[i].is_in_use)
        continue;
      my_keymap_cache[i].is_in_use = 1;
      strcpy(my_keymap_cache[i].name, keymap_filename);
      my_keymap_cache[i].name[FILENAME_SIZE-1] = 0; // just in case
      break;
    }
  }
  scan_keymaps_end:
  f_closedir(&dir);
}

const char str_circumflex[] = "dk_circumflex";
const char str_diaeresis[] = "dk_diaeresis";
const char str_grave_accent[] = "dk_grave_accent";
const char str_acute_accent[] = "dk_acute_accent";
const char str_tilde[] = "dk_tilde";

uint8_t load_keymap_by_name(char* name)
{
  char* next;
  uint8_t result = 0;
  uint8_t ascii_index;
  uint16_t keycode;
  if(name == NULL)
    return 1;
  if(strcmp(name, default_str) == 0)
    return 0;
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/keymaps/%s", name);

  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    result = 2;
    goto load_keymap_end;
  }

  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    if(strncmp(read_buffer, str_circumflex, strlen(str_circumflex)) == 0)
    {
      circumflex = strtoul(read_buffer + strlen(str_circumflex), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_diaeresis, strlen(str_diaeresis)) == 0)
    {
      diaeresis = strtoul(read_buffer + strlen(str_diaeresis), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_grave_accent, strlen(str_grave_accent)) == 0)
    {
      grave_accent = strtoul(read_buffer + strlen(str_grave_accent), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_acute_accent, strlen(str_acute_accent)) == 0)
    {
      acute_accent = strtoul(read_buffer + strlen(str_acute_accent), NULL, 0);
      goto read_keymap_loop_end;
    }
    if(strncmp(read_buffer, str_tilde, strlen(str_tilde)) == 0)
    {
      tilde = strtoul(read_buffer + strlen(str_tilde), NULL, 0);
      goto read_keymap_loop_end;
    }

    ascii_index = strtoul(read_buffer, &next, 0);
    keycode = strtoul(next, NULL, 0);
    _asciimap[ascii_index] = keycode;

    read_keymap_loop_end:
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  strcpy(curr_kb_layout, name);
  load_keymap_end:
  f_close(&sd_file);
  return result;
}

uint8_t load_keymap_by_index(uint8_t index)
{
  if(index >= MAX_KEYMAP_SIZE)
    return 3;
  if(my_keymap_cache[index].is_in_use == 0)
    return 4;
  return load_keymap_by_name(my_keymap_cache[index].name);
}

#define MAX_KEYMAP_PAGES 2

void clean_keymap_name(char* orig, char* output, uint8_t prefix)
{
  sprintf(output, "%d. ", prefix);
  strncpy(output+3, orig+5, strlen(orig)-9);
}

void print_keymap(uint8_t page)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(5, 0);
  ssd1306_WriteString("- Keyboard Layout +", Font_6x10,White);
  if(page == 0)
  {
    ssd1306_SetCursor(5, 12+3);
    ssd1306_WriteString("1. QWERTY (default)",Font_6x10,White);

    for (int i = 0; i<=2; ++i)
    {
      if(my_keymap_cache[i].is_in_use)
      {
        memset(temp_buf, 0, PATH_SIZE);
        clean_keymap_name(my_keymap_cache[i].name, temp_buf, i+2);
        ssd1306_SetCursor(5, 27+12*i);
        ssd1306_WriteString(temp_buf,Font_6x10,White);
      }
    }
  }
  else if(page == 1)
  {
    for (int i = 3; i<=7; ++i)
    {
      if(my_keymap_cache[i].is_in_use)
      {
        memset(temp_buf, 0, PATH_SIZE);
        clean_keymap_name(my_keymap_cache[i].name, temp_buf, i-2);
        ssd1306_SetCursor(5, 15+12*(i-3));
        ssd1306_WriteString(temp_buf,Font_6x10,White);
      }
    }
  }
  ssd1306_UpdateScreen();
}

uint8_t validate_keymap(void)
{
  if(strcmp(curr_kb_layout, default_str) == 0)
    return 0;
  for (int i = 0; i < MAX_KEYMAP_SIZE; ++i)
  {
    if(my_keymap_cache[i].is_in_use == 0)
      continue;
    if(strcmp(curr_kb_layout, my_keymap_cache[i].name) == 0)
      return 0;
  }
  return 1;
}

uint8_t is_keymap_missing(void)
{
  for (int i = 0; i < MAX_KEYMAP_SIZE; ++i)
    if(my_keymap_cache[i].is_in_use)
      return 0;
  return 1;
}

void print_no_keymap_error(void)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(15, 0);
  ssd1306_WriteString("Keymap not found!", Font_6x10,White);
  ssd1306_SetCursor(5, 18);
  ssd1306_WriteString("Open & save SD card", Font_6x10,White);
  ssd1306_SetCursor(5, 30);
  ssd1306_WriteString("with latest PC app", Font_6x10,White);
  ssd1306_SetCursor(18, 50);
  ssd1306_WriteString(project_url,Font_6x10,White);
  ssd1306_UpdateScreen();
  error_animation(0);
  osDelay(10000);
  error_animation(1);
}

void keymap_config(void)
{
  int8_t current_keymap_page = 0;
  scan_keymaps();
  if(is_keymap_missing())
  {
    print_no_keymap_error();
    goto keymap_setting_end;
  }
  if(validate_keymap())
    goto force_keymap_setting;
  keyboard_update();
  if(is_pressed(&button_status[0]) == 0)
  {
    if(load_keymap_by_name(curr_kb_layout) != 0)
      goto force_keymap_setting;
    return;
  }
  force_keymap_setting:
  all_led_off();
  osDelay(50);
  service_all();
  is_in_settings = 1;
  print_keymap(current_keymap_page);

  while(1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    keyboard_update();

    for (int i = 0; i < 4; ++i)
    {
      if(is_pressed(&button_status[i]))
      {
        if(current_keymap_page == 0)
        {
          if(i == 0)
          {
            strcpy(curr_kb_layout, default_str);
            goto keymap_setting_end;
          }
          if(load_keymap_by_index(i-1) == 0)
            goto keymap_setting_end;
        }
        else if(current_keymap_page == 1)
        {
          if(load_keymap_by_index(i+3) == 0)
            goto keymap_setting_end;
        }
        service_all();
      }
    }

    if(is_pressed(&button_status[KEY_BUTTON1])) // -
    {
        current_keymap_page--;
        if(current_keymap_page < 0)
            current_keymap_page = MAX_KEYMAP_PAGES-1;
        print_keymap(current_keymap_page);
        service_all();
    }
    if(is_pressed(&button_status[KEY_BUTTON2])) // +
    {
        current_keymap_page++;
        if(current_keymap_page >= MAX_KEYMAP_PAGES)
            current_keymap_page = 0;
        print_keymap(current_keymap_page);
        service_all();
    }
    osDelay(50);
  }
  keymap_setting_end:
  save_last_profile(p_cache.current_profile);
  service_all();
  is_in_settings = 0;
  print_legend(0, 0);
  save_settings();
}

void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(16);
  keymap_config();
  change_bg();
  service_all();
  keyboard_release_all();
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
    {
      if(is_pressed(&button_status[i]))
      {
        last_keypress = HAL_GetTick();
        ssd1306_dim(0); // OLED back to full brightness

        if(is_sleeping) // wake up from sleep
        {
          change_bg();
          restore_profile(p_cache.current_profile);
          is_sleeping = 0;
          goto key_task_end;
        }
        if(i <= KEY_14)
        {
          keydown_anime_start(i);
          if(hold_cache[i].key_type != KEY_TYPE_UNKNOWN && hold_cache[i].code != 0)
          {
            keyboard_press(&hold_cache[i], 0);
          }
          else
          {
            handle_keypress(i, &button_status[i]); // handle the button state inside here for repeats
            keydown_anime_end(i);
          }
        }
        else if(i == KEY_BUTTON1 || i == KEY_BUTTON2)
          handle_button_press(i);
      }
      if(is_released_but_not_serviced(&button_status[i]) && hold_cache[i].key_type != KEY_TYPE_UNKNOWN && hold_cache[i].code != 0)
      {
        last_keypress = HAL_GetTick();
        keyboard_release(&hold_cache[i]);
        keydown_anime_end(i);
      }
      key_task_end:
      service_press(&button_status[i]);
    } 
    osDelay(16);
  }
}

void animation_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(20);
  anime_init();
  for(;;)
  {
    led_animation_handler();
    if(dp_settings.sleep_after_ms != 0 && is_sleeping == 0 && HAL_GetTick() - last_keypress > dp_settings.sleep_after_ms)
    {
      key_led_shutdown();
      ssd1306_Fill(Black);
      ssd1306_UpdateScreen();
      is_sleeping = 1;
    }
    // dim OLED screen after 5 minutes of idle to prevent burn-in
    if(HAL_GetTick() - last_keypress > 300000)
      ssd1306_dim(1);
    // shift pixels around every 2 minutes to prevent burn-in
    if(is_sleeping == 0 && is_in_settings == 0 && HAL_GetTick() > next_pixel_shift)
    {
      if(has_valid_profiles)
        print_legend(rand()%3-1, rand()%3-1); // -1 to 1
      next_pixel_shift = HAL_GetTick() + 120000;
    }
    osDelay(20);
  }
}
