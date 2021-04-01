#ifndef __shared_H
#define __shared_H

#include "ff.h"
#include "stm32f0xx_hal.h"
#include "main.h"
#define FRANKENDUCK

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern IWDG_HandleTypeDef hiwdg;

extern char instruction[];
extern char project_url[];

extern uint8_t fw_version_major;
extern uint8_t fw_version_minor;
extern uint8_t fw_version_patch;

#endif
