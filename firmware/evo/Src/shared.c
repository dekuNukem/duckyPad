#include <string.h>
#include <stdio.h>
#include "main.h"
#include "shared.h"

void delay_ms(uint32_t amount)
{
  if(amount == 0)
    return;
  HAL_Delay(amount);
}

char* goto_next_arg(char* buf, char* buf_end)
{
  char* curr = buf;  
  if(buf == NULL || curr >= buf_end)
    return NULL;
  while(curr < buf_end && *curr != ' ')
      curr++;
  while(curr < buf_end && *curr == ' ')
      curr++;
  if(curr >= buf_end)
    return NULL;
  return curr;
}

void strip_newline(char* line, uint8_t size)
{
  for(int i = 0; i < size; ++i)
    if(line[i] == '\n' || line[i] == '\r')
      line[i] = 0;
}

