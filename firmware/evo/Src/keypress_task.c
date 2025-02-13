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
  }
}

