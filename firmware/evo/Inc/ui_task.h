#ifndef __UI_TASK_H
#define __UI_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "profiles.h"

void draw_nosd(void);
void draw_noprofile(void);
void draw_current_profile(void);
void draw_settings(dp_global_settings *dps);

#define ONE_MINUTE_IN_MS (60*1000)
#define ONE_HOUR_IN_MS (60*ONE_MINUTE_IN_MS)
#define DONT_SLEEP 0xffffffff
#define SLEEP_OPTION_SIZE 7

#define OLED_DIM_AFTER_MS ONE_MINUTE_IN_MS
#define OLED_CONTRAST_DIM 16
#define OLED_CONTRAST_BRIGHT 255

#define SETTINGS_ENTRY_SIZE 3

#ifdef __cplusplus
}
#endif

#endif


