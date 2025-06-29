#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "fonts.h"
#include "ssd1306.h"
#include "sd_util.h"
#include "cQueue.h"
#include "input_task.h"
#include "shared.h"
#include "neopixel.h"
#include "ui_task.h"
#include "profiles.h"
#include "keypress_task.h"
#include "neopixel.h"
#include "usb_device.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"
#include "keyboard.h"
#include "ds_vm.h"
#include "main.h"
#include "hid_task.h"
#include "dsb_cache.h"

#define PLUS_MINUS_BUTTON_COOLDOWN_MS 250

volatile uint8_t is_sleeping;
volatile uint8_t is_busy;
volatile uint32_t last_keypress;
volatile uint32_t last_execution_exit;

char dsb_on_press_path_buf[FILENAME_BUFSIZE];
char dsb_on_release_path_buf[FILENAME_BUFSIZE];

void block_until_anykey(uint8_t event_type)
{
  clear_sw_queue();
  while(1)
  {
    delay_ms(33);
    switch_event_t sw_event = {0};
    if(q_pop(&switch_event_queue, &sw_event) && sw_event.type == event_type)
      return;
  }
}

static inline uint8_t is_plus_minus_button(uint8_t swid)
{
  return swid == SW_MINUS || swid == SW_PLUS;
}

ds3_exe_result this_exe;

void der_init(ds3_exe_result* der)
{
  der->result = EXE_OK;
  der->next_pc = 0;
  der->data = 0;
  der->epilogue_actions = 0;
}

#define DSB_ALLOW_AUTOREPEAT 0
#define DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY 1
#define DSB_DONT_REPEAT_RETURN_IMMEDIATELY 2
uint8_t run_once(uint8_t swid, char* dsb_path, uint8_t* to_increment)
{
  der_init(&this_exe);

  uint8_t is_press = strstr(dsb_path, key_release_file_string) == NULL;
  uint8_t is_cached = dsbc_search(current_profile_number, swid, is_press, dsvm_cached_data);
  // printf("is_cached: %d\n", is_cached);

  run_dsb(&this_exe, swid, dsb_path, is_cached);
  // printf("---\nexecution finished:\nresult: %d\ndata: %d\nepilogue: 0x%x\n---\n", this_exe.result, this_exe.data, this_exe.epilogue_actions);
  if(to_increment != NULL)
    *to_increment = *to_increment + 1;
  
  uint8_t what_to_do = DSB_ALLOW_AUTOREPEAT;

  if(this_exe.result == EXE_ABORTED)
  {
    neopixel_fill(128, 128, 128);
    oled_say("Aborted");
    delay_ms(100);
    media_key_release();
    delay_ms(100);
    keyboard_release_all();
    delay_ms(100);
    mouse_release_all();
    delay_ms(1000);
    goto_profile(current_profile_number);
    return DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }

  if(this_exe.epilogue_actions & EPILOGUE_SAVE_GV)
  {
    save_gv();
  }
  if(this_exe.epilogue_actions & EPILOGUE_SAVE_LOOP_STATE)
  {
    save_persistent_state(this_exe.epilogue_actions, swid);
  }
  if(this_exe.epilogue_actions & EPILOGUE_SAVE_COLOR_STATE)
  {
    save_persistent_state(this_exe.epilogue_actions, swid);
    what_to_do = DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }
  if(this_exe.epilogue_actions & EPILOGUE_NEED_OLED_RESTORE)
  {
    goto_profile_without_updating_rgb_LED(current_profile_number);
  }
  if(this_exe.epilogue_actions & EPILOGUE_DONT_AUTO_REPEAT)
  {
    what_to_do = DSB_DONT_REPEAT_RETURN_IMMEDIATELY;
  }

  if(this_exe.result >= EXE_ERROR_CODE_START)
  {
    neopixel_fill(128, 0, 0);
    draw_exe_error(this_exe.result);
    block_until_anykey(SW_EVENT_SHORT_PRESS);
    goto_profile(current_profile_number);
    return DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }
  else if(this_exe.result == EXE_ACTION_NEXT_PROFILE)
  {
    goto_next_profile();
    return DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }
  else if(this_exe.result == EXE_ACTION_PREV_PROFILE)
  {
    goto_prev_profile();
    return DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }
  else if(this_exe.result == EXE_ACTION_SLEEP)
  {
    start_sleeping();
    return DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }
  else if(this_exe.result == EXE_ACTION_GOTO_PROFILE)
  {
    goto_profile(this_exe.data);
    return DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY;
  }
  return what_to_do;
}

void update_last_keypress(void)
{
  last_keypress = millis();
}

void onboard_offboard_switch_press(uint8_t swid, char* press_path)
{
  if((curr_pf_info.dsb_exists[swid] & DSB_ON_PRESS_EXISTS) == 0)
    return;

  uint32_t hold_start;
  play_keydown_animation(swid);
  uint8_t* to_increment = &curr_pf_info.keypress_count[swid];
  //-------------
  uint8_t run_result = run_once(swid, press_path, to_increment);
  if(run_result == DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY)
    return;
  // don't repeat if on_release script exists
  if(curr_pf_info.dsb_exists[swid] & DSB_ON_RELEASE_EXISTS)
    return;
  if(run_result == DSB_DONT_REPEAT_RETURN_IMMEDIATELY)
    goto handle_obsw_keydown_end;
  //--------------

  hold_start = millis();
  while(1)
  {
    if(poll_sw_state(swid, 1) == 0)
      goto handle_obsw_keydown_end;
    if(millis()- hold_start > 500)
      break;
  }

  while(1)
  {
    if(poll_sw_state(swid, 1) == 0)
      break;
    uint8_t* to_inc = &curr_pf_info.keypress_count[swid];
    if(run_once(swid, press_path, to_inc) == DSB_DONT_PLAY_KEYUP_ANIMATION_RETURN_IMMEDIATELY)
      return;
  }
  
  handle_obsw_keydown_end:
  // play keyup animation only if there is no on-release DSB file 
  if((curr_pf_info.dsb_exists[swid] & DSB_ON_RELEASE_EXISTS) == 0)
    play_keyup_animation(swid);
}

void settings_menu(void)
{
  draw_settings(&dp_settings);
  draw_settings_led();
  while(1)
  {
    delay_ms(10);
    switch_event_t sw_event = { 0 };
    if(q_pop(&switch_event_queue, &sw_event) == 0)
      continue;
    if(sw_event.type != SW_EVENT_RELEASE)
      continue;

    // printf("settings_menu id: %d type: %d\n", sw_event.id, sw_event.type);
    if(sw_event.id == MSW_0)
    {
      dp_settings.brightness_index = (dp_settings.brightness_index + 1) % BRIGHTNESS_LEVEL_SIZE;
      draw_settings(&dp_settings);
      draw_settings_led();
    }
    else if(sw_event.id == MSW_1)
    {
      dp_settings.sleep_index = (dp_settings.sleep_index + 1) % SLEEP_OPTION_SIZE;
      draw_settings(&dp_settings);
    }
    else if(sw_event.id == MSW_2)
    {
      CLEAR_TEMP_BUF();
      if(get_next_keymap(dp_settings.current_kb_layout, temp_buf))
      {
        memset(dp_settings.current_kb_layout, 0, FILENAME_BUFSIZE);
        get_first_keymap(dp_settings.current_kb_layout);
      }
      else
      {
        strcpy(dp_settings.current_kb_layout, temp_buf);
      }
      draw_settings(&dp_settings);
      load_keymap_by_name(dp_settings.current_kb_layout);
    }
    else if(sw_event.id <= MAX_MSW)
    {
      break;
    }
  }
  save_settings(&dp_settings);
}

void onboard_offboard_switch_release(uint8_t swid, char* release_path)
{
  if((curr_pf_info.dsb_exists[swid] & DSB_ON_RELEASE_EXISTS) == 0)
    return;
  run_once(swid, release_path, NULL);
  play_keyup_animation(swid);
}

void process_keyevent(uint8_t swid, uint8_t event_type)
{
  ssd1306_SetContrast(OLED_CONTRAST_BRIGHT);
  if(swid == SW_PLUS && event_type == SW_EVENT_RELEASE)
  {
    goto_next_profile();
    return;
  }
  if(swid == SW_MINUS && event_type == SW_EVENT_RELEASE)
  {
    goto_prev_profile();
    return;
  }
  if(is_plus_minus_button(swid) && event_type == SW_EVENT_LONG_PRESS)
  {
    settings_menu();
    goto_profile(current_profile_number);
    return;
  }
  if(is_plus_minus_button(swid))
    return; // just in case lol

  memset(dsb_on_press_path_buf, 0, FILENAME_BUFSIZE);
  sprintf(dsb_on_press_path_buf, "/profile_%s/key%d.dsb", profile_name_list[current_profile_number], swid+1);

  memset(dsb_on_release_path_buf, 0, FILENAME_BUFSIZE);
  sprintf(dsb_on_release_path_buf, "/profile_%s/key%d-release.dsb", profile_name_list[current_profile_number], swid+1);

  if(event_type == SW_EVENT_SHORT_PRESS)
    onboard_offboard_switch_press(swid, dsb_on_press_path_buf);
  else if(event_type == SW_EVENT_RELEASE)
    onboard_offboard_switch_release(swid, dsb_on_release_path_buf);

  last_execution_exit = millis();
}

void start_sleeping(void)
{
  if(is_sleeping)
    return;
  neopixel_off();
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  is_sleeping = 1;
}

void wakeup_from_sleep_no_load(void)
{
  update_last_keypress();
  delay_ms(20);
  is_sleeping = 0;
  ssd1306_SetContrast(OLED_CONTRAST_BRIGHT);
}

void wakeup_from_sleep_and_load_profile(uint8_t profile_to_load)
{
  wakeup_from_sleep_no_load();
  goto_profile(profile_to_load);
}

void handle_sw_event(switch_event_t* this_sw_event)
{
  // printf("swid: %d type: %d\n", this_sw_event->id, this_sw_event->type);
  if(is_sleeping && is_plus_minus_button(this_sw_event->id) && this_sw_event->type != SW_EVENT_RELEASE)
  {
    return;
  }
  else if(is_sleeping && !is_plus_minus_button(this_sw_event->id) && this_sw_event->type == SW_EVENT_SHORT_PRESS)
  {
    wakeup_from_sleep_and_load_profile(current_profile_number);
    return;
  }
  else if(is_sleeping && is_plus_minus_button(this_sw_event->id) && this_sw_event->type == SW_EVENT_RELEASE)
  {
    wakeup_from_sleep_and_load_profile(current_profile_number);
    return;
  }
  uint32_t ke_start = millis();
  process_keyevent(this_sw_event->id, this_sw_event->type);
  uint32_t execution_duration = millis() - ke_start;
  printf("took %ldms\n", execution_duration);
  if(execution_duration > 750)
    clear_sw_queue();
}

uint32_t sleep_after_ms_index_to_time_lookup[SLEEP_OPTION_SIZE] = {
  2*ONE_MINUTE_IN_MS,
  5*ONE_MINUTE_IN_MS,
  15*ONE_MINUTE_IN_MS,
  30*ONE_MINUTE_IN_MS,
  ONE_HOUR_IN_MS,
  2*ONE_HOUR_IN_MS,
  DONT_SLEEP,
  };

void file_access_mode_task(void)
{
  while(1)
  {
    delay_ms(5);
    ssd1306_SetContrast(OLED_CONTRAST_BRIGHT);
    switch_event_t sw_event = {0};
    if(q_pop(&switch_event_queue, &sw_event) == 0)
      continue;
    if(is_plus_minus_button(sw_event.id) && sw_event.type == SW_EVENT_LONG_PRESS)
      NVIC_SystemReset();
  }
}

uint32_t last_settings_save;

void keypress_task(void)
{
  while(1)
  {
    delay_ms(5);

    if(is_in_file_access_mode)
      file_access_mode_task();
    
    if(millis() - last_settings_save > 5000)
    {
      save_settings(&dp_settings);
      last_settings_save = millis();
    }

    if(needs_gv_save)
    {
      save_gv();
      needs_gv_save = 0;
    }

    uint32_t ms_since_last_keypress = millis() - last_keypress;
    if(ms_since_last_keypress > sleep_after_ms_index_to_time_lookup[dp_settings.sleep_index])
      start_sleeping();
    else if(ms_since_last_keypress > OLED_DIM_AFTER_MS)
      ssd1306_SetContrast(OLED_CONTRAST_DIM);
    
    if(is_sleeping == 0)
      update_kbled_icon(kb_led_status);

    switch_event_t sw_event = {0};
    if(q_pop(&switch_event_queue, &sw_event) == 0)
      continue;

    if(is_plus_minus_button(sw_event.id) && millis() - last_execution_exit < PLUS_MINUS_BUTTON_COOLDOWN_MS)
      continue;

    is_busy = 1;
    handle_sw_event(&sw_event);
    update_last_keypress();    
    is_busy = 0;
  }
}

