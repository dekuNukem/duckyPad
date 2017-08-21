#ifndef __PARSER_H
#define __PARSER_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"

#define LFN_SIZE 64
#define PARSE_OK 0
#define PARSE_ERROR 1
#define PARSE_NOT_FOUND 2
#define NEXT_PROFILE 0
#define PREV_PROFILE 1

extern FRESULT sd_fresult;
extern FATFS sd_fs;
extern FIL sd_file;
extern DIR dir;
extern FILINFO fno;
extern uint8_t mount_result;
extern uint8_t current_profile;

void parser_test(void);
void change_profile(uint8_t dir);

#ifdef __cplusplus
}
#endif

#endif


