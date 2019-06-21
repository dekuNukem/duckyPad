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

uint8_t init_complete;
uint32_t last_keypress;

void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(10);
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
      if(is_pressed(&button_status[i]))
      {
        if(i < 15)
        {
          keydown_anime_start(i);
          handle_keypress(i);
          keydown_anime_end(i);
        }
        else if(i == 21) // -
          change_profile(PREV_PROFILE);
        else if(i == 22) // +
          change_profile(NEXT_PROFILE);
        service_press(&button_status[i]);
        last_keypress = HAL_GetTick();
        // OLED full brightness on keypress
        ssd1306_dim(0);
      }
    osDelay(30);
  }
}

void animation_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(25);
  anime_init();
  for(;;)
  {
    led_animation_handler();
    // dim OLED screen after 2 minutes of idle to prevent burn-in
    if(HAL_GetTick() - last_keypress > 120000)
      ssd1306_dim(1);
    osDelay(25);
  }
}
