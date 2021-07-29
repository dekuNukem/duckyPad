#ifndef __NEOPIXEL_H
#define __NEOPIXEL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx_hal.h"
#include "shared.h"

#define NEOPIXEL_RESET_BUF_SIZE 50
#ifdef FRANKENDUCK
	#define NEOPIXEL_COUNT 17
#else
	#define NEOPIXEL_COUNT 15
#endif
#define WS_SPI_BUF_SIZE (24*NEOPIXEL_COUNT)
#define WS_BIT_0 0xc0
#define WS_BIT_1 0xf8

void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue);

#ifdef __cplusplus
}
#endif

#endif

