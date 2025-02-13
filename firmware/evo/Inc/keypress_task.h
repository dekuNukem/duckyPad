#ifndef __KEYPRESS_TASK_H
#define __KEYPRESS_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

void keypress_task(void);

extern volatile uint8_t is_sleeping;
extern volatile uint8_t is_busy;

#ifdef __cplusplus
}
#endif

#endif


