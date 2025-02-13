#ifndef __SHARED_H
#define __SHARED_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#define millis() HAL_GetTick()

void delay_ms(uint32_t amount);
char* goto_next_arg(char* buf, char* buf_end);
void strip_newline(char* line, uint32_t size);
void idle_loop(void);

#ifdef __cplusplus
}
#endif

#endif


