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

extern FRESULT sd_fresult;
extern FATFS sd_fs;
extern FIL sd_file;
extern DIR dir;
extern FILINFO fno;
extern uint8_t mount_result;

void parser_test(void);

#ifdef __cplusplus
}
#endif

#endif


