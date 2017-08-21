#ifndef __shared_H
#define __shared_H

#include "ff.h"
#include "stm32f0xx_hal.h"
#include "main.h"

#define COL_1_X 3
#define COL_2_X 45
#define COL_3_X 88

#define ROW_1_Y 0
#define ROW_2_Y 10
#define ROW_3_Y 20
#define ROW_4_Y 30
#define ROW_5_Y 40
#define ROW_6_Y 50

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

#endif
