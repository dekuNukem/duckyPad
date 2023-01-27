#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shared.h"
#include "buttons.h"

but_status button_status[KEY_COUNT];

void mark_as_pressed(but_status* butt)
{
  butt->button_state = BUTTON_PRESSED;
  butt->service_status = BUTTON_SERVICE_UNSERVICED;
}

void mark_as_released(but_status* butt)
{
  butt->button_state = BUTTON_RELEASED;
  butt->service_status = BUTTON_SERVICE_UNSERVICED;
}

uint8_t is_pressed(but_status* butt)
{
  return butt->button_state == BUTTON_PRESSED && butt->service_status == BUTTON_SERVICE_UNSERVICED;
}

uint8_t is_released_but_not_serviced(but_status* butt)
{
  return butt->button_state == BUTTON_RELEASED && butt->service_status == BUTTON_SERVICE_UNSERVICED;
}

void service_press(but_status* butt)
{
  butt->service_status = BUTTON_SERVICE_SERVICED;
}

void keyboard_update(void)
{
  button_status[KEY_0].button_state = 1 - HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
  button_status[KEY_1].button_state = 1 - HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
  button_status[KEY_2].button_state = 1 - HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
  button_status[KEY_3].button_state = 1 - HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
  button_status[KEY_4].button_state = 1 - HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
  button_status[KEY_5].button_state = 1 - HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin);
  button_status[KEY_6].button_state = 1 - HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin);
  button_status[KEY_7].button_state = 1 - HAL_GPIO_ReadPin(SW8_GPIO_Port, SW8_Pin);
  button_status[KEY_8].button_state = 1 - HAL_GPIO_ReadPin(SW9_GPIO_Port, SW9_Pin);
  button_status[KEY_9].button_state = 1 - HAL_GPIO_ReadPin(SW10_GPIO_Port, SW10_Pin);
  button_status[KEY_10].button_state = 1 - HAL_GPIO_ReadPin(SW11_GPIO_Port, SW11_Pin);
  button_status[KEY_11].button_state = 1 - HAL_GPIO_ReadPin(SW12_GPIO_Port, SW12_Pin);
  button_status[KEY_12].button_state = 1 - HAL_GPIO_ReadPin(SW13_GPIO_Port, SW13_Pin);
  button_status[KEY_13].button_state = 1 - HAL_GPIO_ReadPin(SW14_GPIO_Port, SW14_Pin);
  button_status[KEY_14].button_state = 1 - HAL_GPIO_ReadPin(SW15_GPIO_Port, SW15_Pin);
  button_status[KEY_BUTTON1].button_state = 1 - HAL_GPIO_ReadPin(BUTTON_1_GPIO_Port, BUTTON_1_Pin);
  button_status[KEY_BUTTON2].button_state = 1 - HAL_GPIO_ReadPin(BUTTON_2_GPIO_Port, BUTTON_2_Pin);

  for (int i = 0; i < KEY_COUNT; ++i)
  {
    if(button_status[i].prev_state == BUTTON_RELEASED && button_status[i].button_state == BUTTON_PRESSED)
      mark_as_pressed(&button_status[i]);
    else if(button_status[i].prev_state == BUTTON_PRESSED && button_status[i].button_state == BUTTON_RELEASED)
      mark_as_released(&button_status[i]);
    button_status[i].prev_state = button_status[i].button_state;
  }
}

void button_service_all(void)
{
  for (int i = 0; i < KEY_COUNT; ++i)
    service_press(&button_status[i]);
}

uint8_t get_first_active_key(uint8_t curr_key)
{
  for (int i = 0; i < KEY_COUNT; ++i)
    if(is_pressed(&button_status[i]))
      return i+1;
  return 0;
}