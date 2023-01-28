#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "animations.h"
#include "neopixel.h"
#include "shared.h"
#include "parser.h"

uint32_t frame_counter;
uint8_t pixel_map[NEOPIXEL_COUNT] = {2, 1, 0, 3, 4, 5, 8, 7, 6, 9, 10, 11, 14, 13, 12};
uint8_t red_buf[NEOPIXEL_COUNT];
uint8_t green_buf[NEOPIXEL_COUNT];
uint8_t blue_buf[NEOPIXEL_COUNT];
led_animation neo_anime[NEOPIXEL_COUNT];
uint8_t error_color_red[THREE] = {255, 0, 0};
uint8_t color_black[THREE] = {0, 0, 0};
uint8_t color_blue[THREE] = {0, 0, 128};
uint8_t profile_quickswitch_color[THREE] = {50, 50, 50};
int8_t brightness_index = BRIGHTNESS_LEVELS - 1;
uint8_t brightness_values[BRIGHTNESS_LEVELS] = {0, 20, 50, 70, 100};

void set_pixel_3color(uint8_t which, uint8_t r, uint8_t g, uint8_t b)
{
  red_buf[pixel_map[which]] = (uint8_t)((uint16_t)r * (uint16_t)brightness_values[brightness_index] / (uint16_t)100);
  green_buf[pixel_map[which]] = (uint8_t)((uint16_t)g * (uint16_t)brightness_values[brightness_index] / (uint16_t)100);
  blue_buf[pixel_map[which]] = (uint8_t)((uint16_t)b * (uint16_t)brightness_values[brightness_index] / (uint16_t)100);
}

void set_pixel_color(uint8_t which, uint8_t dest_color[THREE])
{
  neo_anime[which].animation_type = ANIMATION_NONE;
  set_pixel_3color(which, dest_color[0], dest_color[1], dest_color[2]);
}

void set_pixel_3color_update_buffer(uint8_t which, uint8_t r, uint8_t g, uint8_t b)
{
  set_pixel_3color(which, r, g, b);
  neo_anime[which].current_color[0] = r;
  neo_anime[which].current_color[1] = g;
  neo_anime[which].current_color[2] = b;
  neo_anime[which].target_color[0] = r;
  neo_anime[which].target_color[1] = g;
  neo_anime[which].target_color[2] = b;
}

// this runs every single frame
void led_animation_handler(void)
{
  frame_counter++;
  uint8_t needs_update = 0;
  for (int idx = 0; idx < NEOPIXEL_COUNT; ++idx)
  {
    int32_t current_frame = frame_counter - neo_anime[idx].animation_start;
    if(current_frame <= 0)
      continue;
    if(neo_anime[idx].animation_type != ANIMATION_CROSS_FADE)
      continue;

    if(current_frame <= neo_anime[idx].animation_duration)
    {
      for (int i = 0; i < THREE; ++i)
      {
        neo_anime[idx].current_color[i] += neo_anime[idx].step[i];
        if(neo_anime[idx].current_color[i] > 255)
          neo_anime[idx].current_color[i] = 255;
        if(neo_anime[idx].current_color[i] < 0)
          neo_anime[idx].current_color[i] = 0;
      }
    }
    else
    {
      for (int i = 0; i < THREE; ++i)
        neo_anime[idx].current_color[i] = neo_anime[idx].target_color[i];
      neo_anime[idx].animation_type = ANIMATION_NONE;
    }
    needs_update = 1;
    set_pixel_3color(neo_anime[idx].index, neo_anime[idx].current_color[0], neo_anime[idx].current_color[1], neo_anime[idx].current_color[2]);
  }
  if(needs_update)
    neopixel_show(red_buf, green_buf, blue_buf);
}

void led_animation_init(led_animation* anime_struct, uint8_t index)
{
  for (int i = 0; i < THREE; ++i)
  {
    anime_struct->current_color[i] = 0;
    anime_struct->step[i] = 0;
    anime_struct->target_color[i] = 0;
  }
  anime_struct->animation_start = 0;
  anime_struct->animation_duration = 0;
  anime_struct->animation_type = ANIMATION_NONE;
  anime_struct->index = index;
}

void led_start_animation(led_animation* anime_struct, uint8_t dest_color[THREE], uint8_t anime_type, uint8_t durations_frames)
{
  for (int i = 0; i < THREE; ++i)
    anime_struct->step[i] = (dest_color[i] - anime_struct->current_color[i]) / (double)durations_frames;
  memcpy(anime_struct->target_color, dest_color, THREE);
  anime_struct->animation_start = frame_counter;
  anime_struct->animation_type = anime_type;
  anime_struct->animation_duration = durations_frames;
}

void neopixel_off(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    neo_anime[i].animation_type = ANIMATION_NONE;
  memset(red_buf, 0, NEOPIXEL_COUNT);
  neopixel_show(red_buf, red_buf, red_buf);
}

void anime_init(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    led_animation_init(&neo_anime[i], i);
}

void redraw_bg(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    neo_anime[i].animation_type = ANIMATION_NONE;
    set_pixel_color(i, p_cache.individual_key_color[i]);
  }
  neopixel_show(red_buf, green_buf, blue_buf);
}

void key_reset(uint8_t which)
{
  neo_anime[which].animation_type = ANIMATION_NONE;
  set_pixel_color(which, p_cache.individual_key_color[which]);
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

void play_keydown_animation(uint8_t idx)
{
  set_pixel_color(idx, p_cache.individual_keydown_color[idx]);
  neo_anime[idx].current_color[0] = p_cache.individual_keydown_color[idx][0];
  neo_anime[idx].current_color[1] = p_cache.individual_keydown_color[idx][1];
  neo_anime[idx].current_color[2] = p_cache.individual_keydown_color[idx][2];
  neo_anime[idx].target_color[0] = p_cache.individual_keydown_color[idx][0];
  neo_anime[idx].target_color[1] = p_cache.individual_keydown_color[idx][1];
  neo_anime[idx].target_color[2] = p_cache.individual_keydown_color[idx][2];
  neopixel_show(red_buf, green_buf, blue_buf);
}

void play_keyup_animation(uint8_t idx)
{
  led_start_animation(&neo_anime[idx], p_cache.individual_key_color[idx], ANIMATION_CROSS_FADE, 70);
}

void neopixel_update(void)
{
  neopixel_show(red_buf, green_buf, blue_buf);
}