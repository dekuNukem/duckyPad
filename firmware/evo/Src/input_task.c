#include <string.h>
#include <stdio.h>
#include "input_task.h"
#include "main.h"
#include "shared.h"

#define SWITCH_EVENT_QUEUE_SIZE 10
Queue_t switch_event_queue;

uint8_t this_sw_state[MAX_TOTAL_SW_COUNT];
uint8_t last_sw_state[MAX_TOTAL_SW_COUNT];
uint32_t last_press_ts[MAX_TOTAL_SW_COUNT];

void switch_init(void)
{
    sw_scan();
	memcpy(last_sw_state, this_sw_state, MAX_TOTAL_SW_COUNT);
    q_init(&switch_event_queue, sizeof(switch_event_t), SWITCH_EVENT_QUEUE_SIZE, FIFO, 0);
}

void sw_scan(void)
{
    this_sw_state[MSW_0] = 1 - HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
    this_sw_state[MSW_1] = 1 - HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
    this_sw_state[MSW_2] = 1 - HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
    this_sw_state[MSW_3] = 1 - HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
    this_sw_state[MSW_4] = 1 - HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
    this_sw_state[MSW_5] = 1 - HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin);
    this_sw_state[MSW_6] = 1 - HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin);
    this_sw_state[MSW_7] = 1 - HAL_GPIO_ReadPin(SW8_GPIO_Port, SW8_Pin);
    this_sw_state[MSW_8] = 1 - HAL_GPIO_ReadPin(SW9_GPIO_Port, SW9_Pin);
    this_sw_state[MSW_9] = 1 - HAL_GPIO_ReadPin(SW10_GPIO_Port, SW10_Pin);
    this_sw_state[MSW_10] = 1 - HAL_GPIO_ReadPin(SW11_GPIO_Port, SW11_Pin);
    this_sw_state[MSW_11] = 1 - HAL_GPIO_ReadPin(SW12_GPIO_Port, SW12_Pin);
    this_sw_state[MSW_12] = 1 - HAL_GPIO_ReadPin(SW13_GPIO_Port, SW13_Pin);
    this_sw_state[MSW_13] = 1 - HAL_GPIO_ReadPin(SW14_GPIO_Port, SW14_Pin);
    this_sw_state[MSW_14] = 1 - HAL_GPIO_ReadPin(SW15_GPIO_Port, SW15_Pin);
    this_sw_state[SW_MINUS] = 1 - HAL_GPIO_ReadPin(MINUS_BUTTON_GPIO_Port, MINUS_BUTTON_Pin);
    this_sw_state[SW_PLUS] = 1 - HAL_GPIO_ReadPin(PLUS_BUTTON_GPIO_Port, PLUS_BUTTON_Pin);
}

void kb_scan_task(void)
{
    sw_scan();
    for (uint8_t i = 0; i < TOTAL_OBSW_COUNT; i++)
	{
        if(this_sw_state[i] == 1 && last_sw_state[i] == 0)
        {
            switch_event_t sw_event = 
            {
                .id = i,
                .type = SW_EVENT_SHORT_PRESS,
            };
            last_press_ts[i] = millis();
            q_push(&switch_event_queue, &sw_event);
        }
        else if(this_sw_state[i] == 0 && last_sw_state[i] == 1)
        {
            switch_event_t sw_event = 
            {
                .id = i,
                .type = SW_EVENT_RELEASE,
            };
            q_push(&switch_event_queue, &sw_event);
        }
    }
    memcpy(last_sw_state, this_sw_state, MAX_TOTAL_SW_COUNT);
}
