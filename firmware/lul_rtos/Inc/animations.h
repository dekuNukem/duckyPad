#ifndef __ANIMATIONS_H
#define __ANIMATIONS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define THREE 3
#define ANIME_FPS 30
#define ANIMATION_NO_ANIMATION 0
#define ANIMATION_CROSS_FADE 1
#define ANIMATION_FULLY_ON 2
#define DEFAULT_BG_RED 12
#define DEFAULT_BG_GREEN 64
#define DEFAULT_BG_BLUE 128
#define DEFAULT_KD_RED 255
#define DEFAULT_KD_GREEN 0
#define DEFAULT_KD_BLUE 0

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
void keypress_anime_handler(uint8_t idx);
void error_animation(uint8_t stage);
void change_bg(void);

extern uint8_t bg_color[THREE];
extern uint8_t keydown_color[THREE];

#ifdef __cplusplus
}
#endif

#endif


