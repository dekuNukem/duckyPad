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

uint32_t init_complete;

void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(10);
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
      if(is_fresh_pressed(&button_status[i]))
      {
        printf("%d\n", i);
        if(i < 15)
        {
          handle_keypress(i);
          keypress_anime_handler(i);
        }
        else if(i == 21) // -
          change_profile(PREV_PROFILE);
        else if(i == 22) // +
          change_profile(NEXT_PROFILE);
        service_fresh_press(&button_status[i]);
      }
    osDelay(30);
  }
}


void animation_task_start(void const * argument)
{
  anime_init();
  for(;;)
  {
    led_animation_handler();
    osDelay(30);
  }
}
