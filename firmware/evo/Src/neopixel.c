#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "shared.h"
#include "neopixel.h"
#include "ui_task.h"
#include "profiles.h"

uint8_t ws_padding_buf[NEOPIXEL_PADDING_BUF_SIZE];
uint8_t ws_spi_buf[WS_SPI_BUF_SIZE];

void spi_fastwrite_buf_size_even(uint8_t *pData, int count)
{
  while(count > 0)
  {
    while(!__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE))
      ;
    hspi1.Instance->DR = *((uint16_t *)pData);
    pData += sizeof(uint16_t);
    count -= 2;
  }
}

uint8_t red_after_brightness[NEOPIXEL_COUNT];
uint8_t green_after_brightness[NEOPIXEL_COUNT];
uint8_t blue_after_brightness[NEOPIXEL_COUNT];

// make sure spi speed is between 8MHz and 10MHz
void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t brightness)
{
  __disable_irq();
  float brightness_percent = (float)brightness/100;
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    red_after_brightness[i] = (float)red[i] * brightness_percent;
    green_after_brightness[i] = (float)green[i] * brightness_percent;
    blue_after_brightness[i] = (float)blue[i] * brightness_percent;
  }

  HAL_SPI_Transmit(&hspi1, ws_padding_buf, NEOPIXEL_PADDING_BUF_SIZE, 5);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    memset(ws_spi_buf, 0, WS_SPI_BUF_SIZE);
    for (int j = 0; j < 8; ++j)
    {
      if((uint8_t)(green_after_brightness[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[j] = WS_BIT_1;
      else
        ws_spi_buf[j] = WS_BIT_0;

      if((uint8_t)(red_after_brightness[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[8 + j] = WS_BIT_1;
      else
        ws_spi_buf[8 + j] = WS_BIT_0;

      if((uint8_t)(blue_after_brightness[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[16 + j] = WS_BIT_1;
      else
        ws_spi_buf[16 + j] = WS_BIT_0;
    }
    HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_SET);
    spi_fastwrite_buf_size_even(ws_spi_buf, WS_SPI_BUF_SIZE);
    spi_fastwrite_buf_size_even(ws_padding_buf, NEOPIXEL_PADDING_BUF_SIZE);
    HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_RESET);
  }
  __enable_irq();
}

//---------------- animation ----------------
volatile uint32_t frame_counter;
uint8_t pixel_map[NEOPIXEL_COUNT] = {2, 1, 0, 3, 4, 5, 8, 7, 6, 9, 10, 11, 14, 13, 12};
uint8_t red_buf[NEOPIXEL_COUNT];
uint8_t green_buf[NEOPIXEL_COUNT];
uint8_t blue_buf[NEOPIXEL_COUNT];

led_animation neo_anime[NEOPIXEL_COUNT];
uint8_t color_red[THREE] = {64 , 0, 0};
uint8_t brightness_index_to_percent_lookup[BRIGHTNESS_LEVEL_SIZE] = {100, 70, 50, 20, 0};


void set_pixel_3color(uint8_t which, uint8_t r, uint8_t g, uint8_t b)
{
  if(which >= NEOPIXEL_COUNT)
    return;
  red_buf[pixel_map[which]] = r;
  green_buf[pixel_map[which]] = g;
  blue_buf[pixel_map[which]] = b;
}

void set_pixel_3color_update_buffer(uint8_t which, uint8_t r, uint8_t g, uint8_t b)
{
  if(which >= NEOPIXEL_COUNT)
    return;
  neo_anime[which].animation_type = ANIMATION_NONE;
  set_pixel_3color(which, r, g, b);
  neo_anime[which].current_color[0] = r;
  neo_anime[which].current_color[1] = g;
  neo_anime[which].current_color[2] = b;
  neo_anime[which].target_color[0] = r;
  neo_anime[which].target_color[1] = g;
  neo_anime[which].target_color[2] = b;
}

void set_pixel_color(uint8_t which, uint8_t dest_color[THREE])
{
  set_pixel_3color(which, dest_color[0], dest_color[1], dest_color[2]);
}

void neopixel_draw_current_buffer(void)
{
  neopixel_show(red_buf, green_buf, blue_buf, brightness_index_to_percent_lookup[dp_settings.brightness_index]);
}

void neopixel_fill(uint8_t rr, uint8_t gg, uint8_t bb)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    neo_anime[i].animation_type = ANIMATION_NONE;
    set_pixel_3color(i, rr, gg, bb);
  }
  neopixel_draw_current_buffer();
}

void neopixel_off(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    neo_anime[i].animation_type = ANIMATION_NONE;
    set_pixel_3color(i, 0, 0, 0);
  }
  neopixel_draw_current_buffer();
}

void redraw_bg(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    neo_anime[i].animation_type = ANIMATION_NONE;
    set_pixel_color(i, curr_pf_info.sw_color[i]);
  }
  neopixel_draw_current_buffer();
}

void reset_key_color(uint8_t which)
{
  if(which >= NEOPIXEL_COUNT)
    return;
  neo_anime[which].animation_type = ANIMATION_NONE;
  set_pixel_color(which, curr_pf_info.sw_color[which]);
  neopixel_draw_current_buffer();
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

void play_keydown_animation(uint8_t sw_number)
{
  if(sw_number > NEOPIXEL_COUNT)
    return;
  set_pixel_color(sw_number, curr_pf_info.sw_activation_color[sw_number]);
  neo_anime[sw_number].current_color[0] = curr_pf_info.sw_activation_color[sw_number][0];
  neo_anime[sw_number].current_color[1] = curr_pf_info.sw_activation_color[sw_number][1];
  neo_anime[sw_number].current_color[2] = curr_pf_info.sw_activation_color[sw_number][2];
  neo_anime[sw_number].target_color[0] = curr_pf_info.sw_activation_color[sw_number][0];
  neo_anime[sw_number].target_color[1] = curr_pf_info.sw_activation_color[sw_number][1];
  neo_anime[sw_number].target_color[2] = curr_pf_info.sw_activation_color[sw_number][2];
  neopixel_draw_current_buffer();
}

void play_keyup_animation(uint8_t sw_number)
{
  if(sw_number > NEOPIXEL_COUNT)
    return;
  led_start_animation(&neo_anime[sw_number], curr_pf_info.sw_color[sw_number], ANIMATION_CROSS_FADE, 50);
}

// this runs every frame
void led_animation_handler(void)
{
  frame_counter++;
  uint8_t needs_update = 0;
  for (int idx = 0; idx < NEOPIXEL_COUNT; idx++)
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
    set_pixel_3color(idx, (uint8_t)neo_anime[idx].current_color[0], (uint8_t)neo_anime[idx].current_color[1], (uint8_t)neo_anime[idx].current_color[2]);
  }
  if(needs_update)
    neopixel_show(red_buf, green_buf, blue_buf, brightness_index_to_percent_lookup[dp_settings.brightness_index]);
}

void led_animation_init()
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    for (int cc = 0; cc < THREE; cc++)
    {
      neo_anime[i].current_color[cc] = 0;
      neo_anime[i].step[cc] = 0;
      neo_anime[i].target_color[cc] = 0;
    }
    neo_anime[i].animation_start = 0;
    neo_anime[i].animation_duration = 0;
    neo_anime[i].animation_type = ANIMATION_NONE;
  }
}

void draw_settings_led(void)
{
  neopixel_off();
  for (size_t i = 0; i < SETTINGS_ENTRY_SIZE; i++)
    set_pixel_3color(i, 0, 0, 255);
  neopixel_draw_current_buffer();
}

void get_current_color(uint8_t which, uint8_t* red, uint8_t* green, uint8_t* blue)
{
  *red = red_buf[pixel_map[which]];
  *green = green_buf[pixel_map[which]];
  *blue = blue_buf[pixel_map[which]];
}

void halt_all_animations(void)
{
  for (size_t i = 0; i < NEOPIXEL_COUNT; i++)
    neo_anime[i].animation_type = ANIMATION_NONE;  
}

