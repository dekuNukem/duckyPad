#ifndef __NEOPIXEL_H
#define __NEOPIXEL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define NEOPIXEL_PADDING_BUF_SIZE 4
#define NEOPIXEL_COUNT 15

#define WS_SPI_BUF_SIZE 24
#define WS_BIT_0 0xc0
#define WS_BIT_1 0xf8

#define BRIGHTNESS_LEVEL_SIZE 5
#define ANIMATION_NONE 0
#define ANIMATION_CROSS_FADE 1

#define THREE 3
#define DEFAULT_BG_RED 64
#define DEFAULT_BG_GREEN 16
#define DEFAULT_BG_BLUE 255
#define DEFAULT_KD_RED 255
#define DEFAULT_KD_GREEN 255
#define DEFAULT_KD_BLUE 255

typedef struct
{
	uint32_t animation_start;
	uint8_t animation_type;
	uint8_t animation_duration;
	uint8_t target_color[THREE];
	double current_color[THREE];
	double step[THREE];
} led_animation;

void spi_fastwrite_buf_size_even(uint8_t *pData, int count);
void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t brightness);
void set_pixel_3color(uint8_t which, uint8_t r, uint8_t g, uint8_t b);
void set_pixel_3color_update_buffer(uint8_t which, uint8_t r, uint8_t g, uint8_t b);
void set_pixel_color(uint8_t which, uint8_t dest_color[THREE]);
void neopixel_draw_current_buffer(void);
void neopixel_fill(uint8_t rr, uint8_t gg, uint8_t bb);
void neopixel_off(void);
void redraw_bg(void);
void reset_key_color(uint8_t which);
void led_start_animation(led_animation* anime_struct, uint8_t dest_color[THREE], uint8_t anime_type, uint8_t durations_frames);
void play_keydown_animation(uint8_t sw_number);
void play_keyup_animation(uint8_t sw_number);
void led_animation_handler(void);
void led_animation_init(void);
void draw_settings_led(void);
void get_current_color(uint8_t which, uint8_t* red, uint8_t* green, uint8_t* blue);
void halt_all_animations(void);

extern uint8_t brightness_index_to_percent_lookup[BRIGHTNESS_LEVEL_SIZE];
extern volatile uint32_t frame_counter;

#ifdef __cplusplus
}
#endif

#endif

