#ifndef __BUTTONS_H
#define __BUTTONS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define KEY_COUNT 23
#define KEY_0 0
#define KEY_1 1
#define KEY_2 2
#define KEY_3 3
#define KEY_4 4
#define KEY_5 5
#define KEY_6 6
#define KEY_7 7
#define KEY_8 8
#define KEY_9 9
#define KEY_10 10
#define KEY_11 11
#define KEY_12 12
#define KEY_13 13
#define KEY_14 14
#define KEY_ROT1SW 15
#define KEY_ROT2SW 16
#define KEY_ROT1A_CW 17
#define KEY_ROT1B_CCW 18
#define KEY_ROT2A_CW 19
#define KEY_ROT2B_CCW 20
#define KEY_BUTTON1 21
#define KEY_BUTTON2 22
#define ROT_CW 0
#define ROT_CCW 1
#define ROT_DEBOUNCE_DELAY_MS 40
#define ROT_DIR_DELAY_MS 400
#define BUTTON_SERVICE_UNSERVICED 0
#define BUTTON_SERVICE_FRESH 1
#define BUTTON_SERVICE_REPEAT 2
#define BUTTON_RELEASED 0   
#define BUTTON_PRESSED 1
typedef struct
{
  uint32_t last_pinchange;
  uint32_t last_rotation_dir;
  uint32_t last_rotation_ts;
  uint32_t A_status;
  uint32_t B_status;
} re_status;

typedef struct
{
  uint8_t prev_state;
  uint8_t button_state; // pressed or unpressed
  uint8_t service_status; 
  uint32_t last_service;
} but_status;

extern but_status button_status[KEY_COUNT];

uint8_t is_fresh_pressed(but_status* butt);
void service_fresh_press(but_status* butt);
void keyboard_update(void);

#ifdef __cplusplus
}
#endif

#endif


