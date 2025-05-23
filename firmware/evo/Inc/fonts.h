#ifndef Fonts
#define Fonts
#include "stm32f0xx_hal.h"

//
//	Structure om font te definieren
//
typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	const uint8_t FontHeight;   /*!< Font height in pixels */
	const uint8_t *data; /*!< Pointer to data font data array */
} FontDef;

extern const FontDef Font_6x10;

#endif

