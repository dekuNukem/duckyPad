#ifndef __MY_TASKS_H
#define __MY_TASKS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

extern uint8_t init_complete;

void keypress_task_start(void const * argument);
void animation_task_start(void const * argument);
void start_sleeping(void);

#ifdef __cplusplus
}
#endif

#endif


