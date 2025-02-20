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

#define PLUS_MINUS_BUTTON_COOLDOWN_MS 250

volatile uint8_t is_sleeping;
volatile uint8_t is_busy;
volatile uint8_t oled_brightness = OLED_CONTRAST_BRIGHT;
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

void update_last_keypress(void)
{
  last_keypress = millis();
}

void settings_menu(void)
{
  draw_settings_led();
  draw_settings(&dp_settings);
  block_until_anykey(SW_EVENT_SHORT_PRESS);
  goto_profile(current_profile_number);
}

void process_keyevent(uint8_t swid, uint8_t event_type)
{
  oled_brightness = OLED_CONTRAST_BRIGHT;
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
    play_keydown_animation(swid);
  if(event_type == SW_EVENT_RELEASE)
    play_keyup_animation(swid);

  printf("%s\n%s\n", dsb_on_press_path_buf, dsb_on_release_path_buf);

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
  oled_brightness = OLED_CONTRAST_BRIGHT;
}

void wakeup_from_sleep_and_load_profile(uint8_t profile_to_load)
{
  wakeup_from_sleep_no_load();
  goto_profile(profile_to_load);
}

void handle_sw_event(switch_event_t* this_sw_event)
{
  update_last_keypress();

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
  if(execution_duration > 500)
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

uint8_t is_all0(uint8_t* buff)
{
  for (size_t i = 0; i < USBD_CUSTOMHID_OUTREPORT_BUF_SIZE; i++)
    if(buff[i])
      return 0;
  return 1;
}

void der_init(ds3_exe_result* der)
{
  der->result = EXE_OK;
  der->next_pc = 0;
  der->data = 0;
  der->epilogue_actions = 0;
}

const char test_dsb_path[] = "/profile_Welcome/key7.dsb";

void keypress_task(void)
{
  while(1)
  {
    delay_ms(10);
    ssd1306_SetContrast(oled_brightness);

    uint32_t ms_since_last_keypress = millis() - last_keypress;
    if(ms_since_last_keypress > 500 * 1000) // sleep_after_ms_index_to_time_lookup[dp_settings.sleep_index]
      start_sleeping();
    else if(ms_since_last_keypress > OLED_DIM_AFTER_MS)
      oled_brightness = OLED_CONTRAST_DIM;

    if(q_getCount(&switch_event_queue) == 0)
      continue;
    switch_event_t sw_event = {0};
    q_pop(&switch_event_queue, &sw_event);

    if(is_plus_minus_button(sw_event.id) && millis() - last_execution_exit < PLUS_MINUS_BUTTON_COOLDOWN_MS)
      continue;

    printf("key %d, type %d\n", sw_event.id, sw_event.type);

    is_busy = 1;
    // handle_sw_event(&sw_event);
    ds3_exe_result this_exe;
    der_init(&this_exe);
    run_dsb(&this_exe, 6, test_dsb_path);
    printf("result: %d\n", this_exe.result);
    is_busy = 0;
  }
}

