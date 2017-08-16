#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "buttons.h"

uint8_t prev_state[KEY_COUNT];
uint8_t curr_state[KEY_COUNT];
uint8_t button_event[KEY_COUNT];
int32_t last_rot;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin) == GPIO_PIN_SET)
    printf("ccw\n");
  else
    printf("cw\n");
}
