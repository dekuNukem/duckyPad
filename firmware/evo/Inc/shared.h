#ifndef __SHARED_H
#define __SHARED_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "stm32f0xx_hal.h"
#include <time.h>

#define TEMP_BUFSIZE 256

#define STM32F0_UUID0 ((uint32_t *)0x1FFFF7AC)
#define STM32F0_UUID1 ((uint32_t *)0x1FFFF7B0)
#define STM32F0_UUID2 ((uint32_t *)0x1FFFF7B4)

#define RTC_MAGICNUM_REG RTC_BKP_DR0
#define RTC_UTC_OFFSET_REG RTC_BKP_DR1
#define RTC_MAGIC_NUMBER 0xd11c

void delay_ms(uint32_t amount);
char* goto_next_arg(char* buf, char* buf_end);
void strip_newline(char* line, uint32_t size);
void idle_loop(void);
uint32_t get_uuid(void);
uint32_t millis(void);
uint8_t is_rtc_valid(void);
void mark_rtc_as_valid(void);
uint8_t RTC_SetFromUnixTimestamp(RTC_HandleTypeDef *rtc_ptr, uint32_t unix_timestamp);
struct tm* get_local_time(RTC_HandleTypeDef *rtc_ptr, int16_t offset_minutes);
void set_utc_offset(int16_t minutes);
int16_t get_utc_offset(void);

extern char temp_buf[TEMP_BUFSIZE];

#define CLEAR_TEMP_BUF() memset(temp_buf, 0, TEMP_BUFSIZE)

#ifdef __cplusplus
}
#endif

#endif


