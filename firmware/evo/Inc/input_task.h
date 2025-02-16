#ifndef __INPUT_TASK_H
#define __INPUT_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "cQueue.h"

#define MECH_OBSW_COUNT 15
#define TOTAL_OBSW_COUNT 17
#define MAX_TOTAL_SW_COUNT TOTAL_OBSW_COUNT

typedef struct
{
    uint8_t id;
    uint8_t type;
} switch_event_t;

// mechnical switches
#define MSW_0 0
#define MSW_1 1
#define MSW_2 2
#define MSW_3 3
#define MSW_4 4
#define MSW_5 5
#define MSW_6 6
#define MSW_7 7
#define MSW_8 8
#define MSW_9  9
#define MSW_10  10
#define MSW_11  11
#define MSW_12  12
#define MSW_13  13
#define MSW_14  14
#define MAX_MSW MSW_14
#define SW_MINUS 15
#define SW_PLUS 16

#define EXP_BUTTON_START 20
#define EXP_BUTTON_END (MAX_TOTAL_SW_COUNT-1)

#define SW_EVENT_RELEASE 0
#define SW_EVENT_SHORT_PRESS 1
#define SW_EVENT_LONG_PRESS 2

void switch_init(void);
void sw_scan(void);
void kb_scan_task(void);
void clear_sw_queue(void);
uint8_t sw_queue_has_keydown_event(void);

extern Queue_t switch_event_queue;

#ifdef __cplusplus
}
#endif

#endif


