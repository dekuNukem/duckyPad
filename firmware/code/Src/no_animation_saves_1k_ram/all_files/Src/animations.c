#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "animations.h"
#include "neopixel.h"
#include "shared.h"
#include "parser.h"

// the LED on the PCB is arranged in a zig zag pattern, this maps it back to linear
uint8_t pixel_map[NEOPIXEL_COUNT] = {2, 1, 0, 3, 4, 5, 8, 7, 6, 9, 10, 11, 14, 13, 12};
uint8_t red_buf[NEOPIXEL_COUNT];
uint8_t green_buf[NEOPIXEL_COUNT];
uint8_t blue_buf[NEOPIXEL_COUNT];
uint8_t error_color_red[THREE] = {255, 0, 0};
uint8_t color_black[THREE] = {0, 0, 0};
uint8_t color_blue[THREE] = {0, 0, 128};
uint8_t profile_quickswitch_color[THREE] = {50, 50, 50};
int8_t brightness_index = BRIGHTNESS_LEVELS - 1;
uint8_t brightness_values[BRIGHTNESS_LEVELS] = {0, 20, 50, 70, 100};

void neopixel_off(void)
{
  memset(red_buf, 0, NEOPIXEL_COUNT);
  memset(green_buf, 0, NEOPIXEL_COUNT);
  memset(blue_buf, 0, NEOPIXEL_COUNT);
  neopixel_show(red_buf, green_buf, blue_buf);
}

void set_pixel_color(uint8_t which, uint8_t dest_color[THREE])
{
  red_buf[pixel_map[which]] = (uint8_t)((uint16_t)dest_color[0] * (uint16_t)brightness_values[brightness_index] / (uint16_t)100);
  green_buf[pixel_map[which]] = (uint8_t)((uint16_t)dest_color[1] * (uint16_t)brightness_values[brightness_index] / (uint16_t)100);
  blue_buf[pixel_map[which]] = (uint8_t)((uint16_t)dest_color[2] * (uint16_t)brightness_values[brightness_index] / (uint16_t)100);
}

void redraw_bg(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    set_pixel_color(i, p_cache.individual_key_color[i]);
  neopixel_show(red_buf, green_buf, blue_buf);
}

void change_bg(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    set_pixel_color(i, p_cache.individual_key_color[i]);
  neopixel_show(red_buf, green_buf, blue_buf);
}

void error_animation(uint8_t stage)
{
  if(stage == 0)
  {
    for (int j = 0; j < 3; ++j)
    {
      for (int i = 0; i < NEOPIXEL_COUNT; ++i)
        set_pixel_color(i, error_color_red);
      neopixel_show(red_buf, green_buf, blue_buf);
      osDelay(500);
      for (int i = 0; i < NEOPIXEL_COUNT; ++i)
        set_pixel_color(i, color_black);
      neopixel_show(red_buf, green_buf, blue_buf);
      osDelay(500);
    }
    for (int i = 0; i < NEOPIXEL_COUNT; ++i)
      set_pixel_color(i, error_color_red);
    neopixel_show(red_buf, green_buf, blue_buf);
    osDelay(50);
  }
  else
  {
    for (int i = 0; i < NEOPIXEL_COUNT; ++i)
      set_pixel_color(i, p_cache.individual_key_color[i]);
    neopixel_show(red_buf, green_buf, blue_buf);
  }
}

void profile_quickswitch_animation(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    set_pixel_color(i, profile_quickswitch_color);
  neopixel_show(red_buf, green_buf, blue_buf);
}

void keydown_anime_start(uint8_t idx)
{
  set_pixel_color(idx, p_cache.individual_keydown_color[idx]);
  neopixel_show(red_buf, green_buf, blue_buf);
}

void keydown_anime_end(uint8_t idx)
{
  set_pixel_color(idx, p_cache.individual_key_color[idx]);
  neopixel_show(red_buf, green_buf, blue_buf);
}
