
#include "stm32f0xx_hal.h"

#ifndef hash_lookup_h
#define hash_lookup_h

#define KEYWORD_HASH_SIZE 102

extern const uint16_t keyword_hash[KEYWORD_HASH_SIZE];
extern const uint8_t keyword_keycode[KEYWORD_HASH_SIZE];
extern const uint8_t keyword_keytype[KEYWORD_HASH_SIZE];


#define REPEAT_HASH (44966)
#define REM_HASH (6377)
#define DEFAULTDELAY_HASH (32345)
#define DEFAULTCHARDELAY_HASH (18039)
#define DEFAULTDELAYFUZZ_HASH (56040)
#define DEFAULTCHARDELAYFUZZ_HASH (33286)
#define DELAY_HASH (42132)
#define STRING_HASH (29500)
#define STRINGLN_HASH (15446)
#define UARTPRINT_HASH (51214)
#define EMUK_HASH (14455)
#define LOOP_HASH (5855)
#define LCR_HASH (65318)
#define DP_SLEEP_HASH (45873)
#define PREV_PROFILE_HASH (18482)
#define NEXT_PROFILE_HASH (15508)
#define GOTO_PROFILE_HASH (18926)
#define KEYDOWN_HASH (34694)
#define KEYUP_HASH (35315)
#define INJECT_MOD_HASH (44929)
#define BG_COLOR_HASH (8908)
#define KEYDOWN_COLOR_HASH (45380)
#define SWCOLOR__HASH (11853)
#define SWCOLOR_HASH (10286)
#define DIM_UNUSED_KEYS_HASH (53421)


#define REPEAT_LEN (6)
#define REM_LEN (3)
#define DEFAULTDELAY_LEN (12)
#define DEFAULTCHARDELAY_LEN (16)
#define DEFAULTDELAYFUZZ_LEN (16)
#define DEFAULTCHARDELAYFUZZ_LEN (20)
#define DELAY_LEN (5)
#define STRING_LEN (6)
#define STRINGLN_LEN (8)
#define UARTPRINT_LEN (9)
#define EMUK_LEN (4)
#define LOOP_LEN (4)
#define LCR_LEN (3)
#define DP_SLEEP_LEN (8)
#define PREV_PROFILE_LEN (12)
#define NEXT_PROFILE_LEN (12)
#define GOTO_PROFILE_LEN (12)
#define KEYDOWN_LEN (7)
#define KEYUP_LEN (5)
#define INJECT_MOD_LEN (10)
#define BG_COLOR_LEN (8)
#define KEYDOWN_COLOR_LEN (13)
#define SWCOLOR__LEN (8)
#define SWCOLOR_LEN (7)
#define DIM_UNUSED_KEYS_LEN (15)



#endif

