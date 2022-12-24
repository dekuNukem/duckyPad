
#include "stm32f0xx_hal.h"

#ifndef hash_lookup_h
#define hash_lookup_h

#define KEYWORD_HASH_SIZE 97

extern const uint16_t keyword_hash[KEYWORD_HASH_SIZE];
extern const uint8_t keyword_keycode[KEYWORD_HASH_SIZE];
extern const uint8_t keyword_keytype[KEYWORD_HASH_SIZE];

#endif

