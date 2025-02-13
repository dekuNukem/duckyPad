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

volatile uint8_t is_sleeping;
volatile uint8_t is_busy;

static inline uint8_t is_plus_minus_button(uint8_t swid)
{
  return swid == SW_MINUS || swid == SW_PLUS;
}

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
}

void handle_sw_event(switch_event_t* this_sw_event)
{
  process_keyevent(this_sw_event->id, this_sw_event->type);
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
    printf("key %d, type %d\n", sw_event.id, sw_event.type);

    is_busy = 1;
    handle_sw_event(&sw_event);
    is_busy = 0;
  }
}

