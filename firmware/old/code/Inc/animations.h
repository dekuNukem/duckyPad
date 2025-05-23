#ifndef __ANIMATIONS_H
#define __ANIMATIONS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define THREE 3
#define ANIMATION_NONE 0
#define ANIMATION_CROSS_FADE 1
#define DEFAULT_BG_RED 64
#define DEFAULT_BG_GREEN 16
#define DEFAULT_BG_BLUE 255
#define DEFAULT_KD_RED 255
#define DEFAULT_KD_GREEN 255
#define DEFAULT_KD_BLUE 255
#define BRIGHTNESS_LEVELS 5

typedef struct
{
	uint32_t animation_start;
	uint8_t animation_type;
	uint8_t animation_duration;
	uint8_t index;
	uint8_t target_color[THREE];
	double current_color[THREE];
	double step[THREE];
} led_animation;

void led_animation_handler(void);
void anime_init(void);
void play_keydown_animation(uint8_t idx);
void play_keyup_animation(uint8_t idx);
void error_animation(uint8_t stage);
void redraw_bg(void);
void neopixel_off(void);
void set_pixel_3color_update_buffer(uint8_t which, uint8_t r, uint8_t g, uint8_t b);
void neopixel_update(void);
void key_reset(uint8_t which);
void get_current_color(uint8_t which, uint8_t* red, uint8_t* green, uint8_t* blue);

extern int8_t brightness_index;
extern uint8_t brightness_values[BRIGHTNESS_LEVELS];
#ifdef __cplusplus
}
#endif

#endif


