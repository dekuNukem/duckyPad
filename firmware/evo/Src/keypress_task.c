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
    if(q_getCount(&switch_event_queue) == 0)
      continue;
    switch_event_t sw_event = {0};
    q_pop(&switch_event_queue, &sw_event);
    if(sw_event.type == event_type)
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

/*
  draw_settings_led();
  draw_settings(&dp_settings);
*/

void settings_menu(void)
{
  draw_settings_led();
  draw_settings(&dp_settings);
  block_until_anykey(SW_EVENT_SHORT_PRESS);
  goto_profile(current_profile_number);
}

#define KB_BUF_SIZE 8
uint8_t kb_buf[KB_BUF_SIZE];
void kb_test(void)
{
  memset(kb_buf, 0, KB_BUF_SIZE);
  kb_buf[0] = 3;
  kb_buf[4] = 1;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, kb_buf, KB_BUF_SIZE);
  printf("kbtest\n");
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*) hUsbDeviceFS.pClassData;
  for (size_t i = 0; i < 64; i++)
  {
    printf("%02x ", hhid->Report_buf[i]);
  }
  printf("\n");
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

  // printf("%s\n%s\n", dsb_on_press_path_buf, dsb_on_release_path_buf);

  kb_test();

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

void keypress_task(void)
{
  while(1)
  {
    delay_ms(10);
    ssd1306_SetContrast(oled_brightness);

    uint32_t ms_since_last_keypress = millis() - last_keypress;
    if(ms_since_last_keypress > 5000) // sleep_after_ms_index_to_time_lookup[dp_settings.sleep_index]
      start_sleeping();
    else if(ms_since_last_keypress > 2500) // OLED_DIM_AFTER_MS
      oled_brightness = OLED_CONTRAST_DIM;

    if(q_getCount(&switch_event_queue) == 0)
      continue;
    switch_event_t sw_event = {0};
    q_pop(&switch_event_queue, &sw_event);

    if(is_plus_minus_button(sw_event.id) && millis() - last_execution_exit < PLUS_MINUS_BUTTON_COOLDOWN_MS)
      continue;

    printf("key %d, type %d\n", sw_event.id, sw_event.type);

    is_busy = 1;
    handle_sw_event(&sw_event);
    is_busy = 0;
  }
}

