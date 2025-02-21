#ifndef __KEYPRESS_TASK_H
#define __KEYPRESS_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

extern volatile uint8_t is_sleeping;
extern volatile uint8_t is_busy;

void update_last_keypress(void);
void process_keyevent(uint8_t swid, uint8_t event_type);
void handle_sw_event(switch_event_t* this_sw_event);
void keypress_task(void);
void start_sleeping(void);

#ifdef __cplusplus
}
#endif

#endif


