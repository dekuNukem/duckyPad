#ifndef __shared_H
#define __shared_H

#include "ff.h"
#include "stm32f4xx_hal.h"
//#include "usbd_def.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
extern IWDG_HandleTypeDef hiwdg;
//extern USBD_HandleTypeDef hUsbDeviceFS;

extern char instruction[];
extern char project_url[];

#define USE_GERMAN_ASCIIMAP 1
#define USE_ENGLISH_ASCIIMAP 0

#endif
