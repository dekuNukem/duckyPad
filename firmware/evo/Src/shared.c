#include <string.h>
#include <stdio.h>
#include "main.h"

void delay_ms(uint32_t amount)
{
  if(amount == 0)
    return;
  HAL_Delay(amount);
}
