#ifndef __ANIMATIONS_H
#define __ANIMATIONS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define THREE 3
#define DEFAULT_BG_RED 64
#define DEFAULT_BG_GREEN 16
#define DEFAULT_BG_BLUE 255
#define DEFAULT_KD_RED 255
#define DEFAULT_KD_GREEN 255
#define DEFAULT_KD_BLUE 255
#define BRIGHTNESS_LEVELS 5

void keydown_anime_start(uint8_t idx);
void keydown_anime_end(uint8_t idx);
void error_animation(uint8_t stage);
void redraw_bg(void);
void change_bg(void);
void neopixel_off(void);
void profile_quickswitch_animation(void);

extern int8_t brightness_index;
extern uint8_t brightness_values[BRIGHTNESS_LEVELS];
#ifdef __cplusplus
}
#endif

#endif


