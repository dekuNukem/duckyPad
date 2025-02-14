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

#define PLUS_MINUS_BUTTON_COOLDOWN_MS 250

volatile uint8_t is_sleeping;
volatile uint8_t is_busy;
volatile uint8_t oled_brightness = OLED_CONTRAST_BRIGHT;
volatile uint32_t last_keypress;
volatile uint32_t last_execution_exit;

char dsb_on_press_path_buf[FILENAME_BUFSIZE];
char dsb_on_release_path_buf[FILENAME_BUFSIZE];

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

void process_keyevent(uint8_t swid, uint8_t event_type)
{
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
  if(is_plus_minus_button(swid))
    return;
  if(event_type == SW_EVENT_SHORT_PRESS)
    play_keydown_animation(swid);
  if(event_type == SW_EVENT_RELEASE)
    play_keyup_animation(swid);
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
    // wakeup_from_sleep_and_load_profile(current_profile_number);
    return;
  }
  else if(is_sleeping && is_plus_minus_button(this_sw_event->id) && this_sw_event->type == SW_EVENT_RELEASE)
  {
    // wakeup_from_sleep_and_load_profile(current_profile_number);
    return;
  }
  uint32_t ke_start = millis();
  process_keyevent(this_sw_event->id, this_sw_event->type);
  uint32_t execution_duration = millis() - ke_start;
  printf("took %ldms\n", execution_duration);
  if(execution_duration > 500)
    clear_sw_queue();
}

void keypress_task(void)
{
  while(1)
  {
    delay_ms(10);
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

