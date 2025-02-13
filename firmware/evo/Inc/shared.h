#ifndef __SHARED_H
#define __SHARED_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#define millis() HAL_GetTick()

void delay_ms(uint32_t amount);

#ifdef __cplusplus
}
#endif

#endif


