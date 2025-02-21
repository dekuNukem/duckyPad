#ifndef __SHARED_H
#define __SHARED_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#define TEMP_BUFSIZE 256
#define millis() HAL_GetTick()

#define STM32F0_UUID0 ((uint32_t *)0x1FFFF7AC)
#define STM32F0_UUID1 ((uint32_t *)0x1FFFF7B0)
#define STM32F0_UUID2 ((uint32_t *)0x1FFFF7B4)

void delay_ms(uint32_t amount);
char* goto_next_arg(char* buf, char* buf_end);
void strip_newline(char* line, uint32_t size);
void idle_loop(void);
uint32_t get_uuid(void);

extern char temp_buf[TEMP_BUFSIZE];

#ifdef __cplusplus
}
#endif

#endif


