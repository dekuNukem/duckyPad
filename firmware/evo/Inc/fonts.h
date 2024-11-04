#ifndef Fonts
#define Fonts

#include "stm32f0xx_hal.h"

typedef struct {
	const uint8_t FontWidth;
	const uint8_t FontHeight;
	const uint8_t *data;
} FontDef;

extern FontDef Font_6x10;

#endif

