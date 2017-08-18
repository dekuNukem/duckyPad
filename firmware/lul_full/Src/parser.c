#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "buttons.h"
#include "parser.h"

#define READ_BUF_SIZE 256

char filename[] = "/profile1/key1.txt";
char read_buffer[READ_BUF_SIZE];

void parser_test(void)
{
  if(f_stat(filename, NULL) != 0)
    return;
  if(f_open(&sd_file, filename, FA_READ) != 0)
    return;

  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file))
  {
    printf(">>>>%s\n", read_buffer);
  }

  f_close(&sd_file);
}

