#ifndef __shared_H
#define __shared_H

#include "ff.h"
#include "stm32f0xx_hal.h"
#include "main.h"

extern FRESULT sd_fresult;
extern FATFS sd_fs;
extern FIL sd_file;
extern uint8_t mount_result;

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

#endif
