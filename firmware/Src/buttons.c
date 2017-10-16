#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shared.h"
#include "buttons.h"

but_status button_status[KEY_COUNT];
re_status rot1, rot2;

void fresh_pressed(but_status* butt)
{
  butt->button_state = BUTTON_PRESSED;
  butt->service_status = BUTTON_SERVICE_UNSERVICED;
  butt->last_service = 0;
}

uint8_t is_fresh_pressed(but_status* butt)
{
  return butt->button_state == BUTTON_PRESSED && butt->service_status == BUTTON_SERVICE_UNSERVICED;
}

void service_fresh_press(but_status* butt)
{
  butt->service_status = BUTTON_SERVICE_FRESH;
  butt->last_service = HAL_GetTick();
}

void keyboard_update(void)
{
  button_status[KEY_0].button_state = 1 - HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
  if(button_status[KEY_0].prev_state == BUTTON_RELEASED && button_status[KEY_0].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_0]);
  button_status[KEY_0].prev_state = button_status[KEY_0].button_state;

  button_status[KEY_1].button_state = 1 - HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
  if(button_status[KEY_1].prev_state == BUTTON_RELEASED && button_status[KEY_1].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_1]);
  button_status[KEY_1].prev_state = button_status[KEY_1].button_state;

  button_status[KEY_2].button_state = 1 - HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
  if(button_status[KEY_2].prev_state == BUTTON_RELEASED && button_status[KEY_2].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_2]);
  button_status[KEY_2].prev_state = button_status[KEY_2].button_state;

  button_status[KEY_3].button_state = 1 - HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
  if(button_status[KEY_3].prev_state == BUTTON_RELEASED && button_status[KEY_3].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_3]);
  button_status[KEY_3].prev_state = button_status[KEY_3].button_state;

  button_status[KEY_4].button_state = 1 - HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
  if(button_status[KEY_4].prev_state == BUTTON_RELEASED && button_status[KEY_4].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_4]);
  button_status[KEY_4].prev_state = button_status[KEY_4].button_state;

  button_status[KEY_5].button_state = 1 - HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin);
  if(button_status[KEY_5].prev_state == BUTTON_RELEASED && button_status[KEY_5].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_5]);
  button_status[KEY_5].prev_state = button_status[KEY_5].button_state;

  button_status[KEY_6].button_state = 1 - HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin);
  if(button_status[KEY_6].prev_state == BUTTON_RELEASED && button_status[KEY_6].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_6]);
  button_status[KEY_6].prev_state = button_status[KEY_6].button_state;

  button_status[KEY_7].button_state = 1 - HAL_GPIO_ReadPin(SW8_GPIO_Port, SW8_Pin);
  if(button_status[KEY_7].prev_state == BUTTON_RELEASED && button_status[KEY_7].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_7]);
  button_status[KEY_7].prev_state = button_status[KEY_7].button_state;

  button_status[KEY_8].button_state = 1 - HAL_GPIO_ReadPin(SW9_GPIO_Port, SW9_Pin);
  if(button_status[KEY_8].prev_state == BUTTON_RELEASED && button_status[KEY_8].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_8]);
  button_status[KEY_8].prev_state = button_status[KEY_8].button_state;

  button_status[KEY_9].button_state = 1 - HAL_GPIO_ReadPin(SW10_GPIO_Port, SW10_Pin);
  if(button_status[KEY_9].prev_state == BUTTON_RELEASED && button_status[KEY_9].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_9]);
  button_status[KEY_9].prev_state = button_status[KEY_9].button_state;

  button_status[KEY_10].button_state = 1 - HAL_GPIO_ReadPin(SW11_GPIO_Port, SW11_Pin);
  if(button_status[KEY_10].prev_state == BUTTON_RELEASED && button_status[KEY_10].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_10]);
  button_status[KEY_10].prev_state = button_status[KEY_10].button_state;

  button_status[KEY_11].button_state = 1 - HAL_GPIO_ReadPin(SW12_GPIO_Port, SW12_Pin);
  if(button_status[KEY_11].prev_state == BUTTON_RELEASED && button_status[KEY_11].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_11]);
  button_status[KEY_11].prev_state = button_status[KEY_11].button_state;

  button_status[KEY_12].button_state = 1 - HAL_GPIO_ReadPin(SW13_GPIO_Port, SW13_Pin);
  if(button_status[KEY_12].prev_state == BUTTON_RELEASED && button_status[KEY_12].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_12]);
  button_status[KEY_12].prev_state = button_status[KEY_12].button_state;

  button_status[KEY_13].button_state = 1 - HAL_GPIO_ReadPin(SW14_GPIO_Port, SW14_Pin);
  if(button_status[KEY_13].prev_state == BUTTON_RELEASED && button_status[KEY_13].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_13]);
  button_status[KEY_13].prev_state = button_status[KEY_13].button_state;

  button_status[KEY_14].button_state = 1 - HAL_GPIO_ReadPin(SW15_GPIO_Port, SW15_Pin);
  if(button_status[KEY_14].prev_state == BUTTON_RELEASED && button_status[KEY_14].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_14]);
  button_status[KEY_14].prev_state = button_status[KEY_14].button_state;

  button_status[KEY_ROT1SW].button_state = 1 - HAL_GPIO_ReadPin(E1_SW_GPIO_Port, E1_SW_Pin);
  if(button_status[KEY_ROT1SW].prev_state == BUTTON_RELEASED && button_status[KEY_ROT1SW].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_ROT1SW]);
  button_status[KEY_ROT1SW].prev_state = button_status[KEY_ROT1SW].button_state;

  button_status[KEY_ROT2SW].button_state = 1 - HAL_GPIO_ReadPin(E2_SW_GPIO_Port, E2_SW_Pin);
  if(button_status[KEY_ROT2SW].prev_state == BUTTON_RELEASED && button_status[KEY_ROT2SW].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_ROT2SW]);
  button_status[KEY_ROT2SW].prev_state = button_status[KEY_ROT2SW].button_state;

  button_status[KEY_BUTTON1].button_state = 1 - HAL_GPIO_ReadPin(BUTTON_1_GPIO_Port, BUTTON_1_Pin);
  if(button_status[KEY_BUTTON1].prev_state == BUTTON_RELEASED && button_status[KEY_BUTTON1].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_BUTTON1]);
  button_status[KEY_BUTTON1].prev_state = button_status[KEY_BUTTON1].button_state;

  button_status[KEY_BUTTON2].button_state = 1 - HAL_GPIO_ReadPin(BUTTON_2_GPIO_Port, BUTTON_2_Pin);
  if(button_status[KEY_BUTTON2].prev_state == BUTTON_RELEASED && button_status[KEY_BUTTON2].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_BUTTON2]);
  button_status[KEY_BUTTON2].prev_state = button_status[KEY_BUTTON2].button_state;
}

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
        fresh_pressed(&button_status[KEY_ROT2A_CW]);
      }
      // last rotation is ccw
      else
      {
        if(HAL_GetTick() - rot2.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          fresh_pressed(&button_status[KEY_ROT2A_CW]);
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
        fresh_pressed(&button_status[KEY_ROT2B_CCW]);
      }
      // last rotation is cw
      else
      {
        if(HAL_GetTick() - rot2.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          fresh_pressed(&button_status[KEY_ROT2B_CCW]);
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
        fresh_pressed(&button_status[KEY_ROT1A_CW]);
      }
      // last rotation is ccw
      else
      {
        if(HAL_GetTick() - rot1.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          fresh_pressed(&button_status[KEY_ROT1A_CW]);
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
        fresh_pressed(&button_status[KEY_ROT1B_CCW]);
      }
      // last rotation is cw
      else
      {
        if(HAL_GetTick() - rot1.last_rotation_ts > ROT_DIR_DELAY_MS)
        {
          fresh_pressed(&button_status[KEY_ROT1B_CCW]);
          rot1.last_rotation_dir = ROT_CCW;
        }
      }
    }
  }
}
