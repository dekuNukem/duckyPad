#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "buttons.h"

uint8_t prev_state[KEY_COUNT];
uint8_t curr_state[KEY_COUNT];
uint8_t button_event[KEY_COUNT];
re_status rot1, rot2;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == E2A_Pin)
  {
    if(HAL_GetTick() - rot2.last_pinchange <= ROT_DEBOUNCE_DELAY_MS)
    {
      rot2.last_pinchange = HAL_GetTick();
      return;
    }
    rot2.last_pinchange = HAL_GetTick();
    rot2.A_status = HAL_GPIO_ReadPin(E2A_GPIO_Port, E2A_Pin);
    rot2.B_status = HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin);

    if(rot2.A_status == GPIO_PIN_SET)
      return;

    // this rotation is cw
    if(rot2.B_status == GPIO_PIN_RESET)
    {
      // last rotation is also cw
      if(rot2.last_rotation_dir == ROT_CW)
      {
        rot2.last_rotation_ts = HAL_GetTick();
        button_event[KEY_ROT2A_CW] = 1;
      }
      // last rotation is ccw
      else
      {
        if(HAL_GetTick() - rot2.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          button_event[KEY_ROT2A_CW] = 1;
          rot2.last_rotation_dir = ROT_CW;
        }
      }
    }
    // this rotation is ccw
    else
    {
      // last rotation is also CCW
      if(rot2.last_rotation_dir == ROT_CCW)
      {
        rot2.last_rotation_ts = HAL_GetTick();
        button_event[KEY_ROT2B_CCW] = 1;
      }
      // last rotation is cw
      else
      {
        if(HAL_GetTick() - rot2.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          button_event[KEY_ROT2B_CCW] = 1;
          rot2.last_rotation_dir = ROT_CCW;
        }
      }
    }
  }

  else if(GPIO_Pin == E1A_Pin)
  {
    if(HAL_GetTick() - rot1.last_pinchange <= ROT_DEBOUNCE_DELAY_MS)
    {
      rot1.last_pinchange = HAL_GetTick();
      return;
    }
    rot1.last_pinchange = HAL_GetTick();
    rot1.A_status = HAL_GPIO_ReadPin(E1A_GPIO_Port, E1A_Pin);
    rot1.B_status = HAL_GPIO_ReadPin(E1B_GPIO_Port, E1B_Pin);

    if(rot1.A_status == GPIO_PIN_SET)
      return;

    // this rotation is cw
    if(rot1.B_status == GPIO_PIN_RESET)
    {
      // last rotation is also cw
      if(rot1.last_rotation_dir == ROT_CW)
      {
        rot1.last_rotation_ts = HAL_GetTick();
        button_event[KEY_ROT1A_CW] = 1;
      }
      // last rotation is ccw
      else
      {
        if(HAL_GetTick() - rot1.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          button_event[KEY_ROT1A_CW] = 1;
          rot1.last_rotation_dir = ROT_CW;
        }
      }
    }
    // this rotation is ccw
    else
    {
      // last rotation is also CCW
      if(rot1.last_rotation_dir == ROT_CCW)
      {
        rot1.last_rotation_ts = HAL_GetTick();
        button_event[KEY_ROT1B_CCW] = 1;
      }
      // last rotation is cw
      else
      {
        if(HAL_GetTick() - rot1.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          button_event[KEY_ROT1B_CCW] = 1;
          rot1.last_rotation_dir = ROT_CCW;
        }
      }
    }
  }
}
