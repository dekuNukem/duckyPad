#include <string.h>
#include <stdio.h>
#include "main.h"
#include "ui_task.h"
#include "ssd1306.h"
#include "shared.h"

#define OLED_LINE_BUF_SIZE 64
char oled_line_buf[OLED_LINE_BUF_SIZE];

#define OLED_CHAR_WIDTH_PIXELS 6
#define OLED_WIDTH_PIXELS SSD1306_WIDTH
uint8_t center_line(uint8_t line_len)
{
  int16_t start_pixel = (OLED_WIDTH_PIXELS - line_len * OLED_CHAR_WIDTH_PIXELS) / 2;
  if(start_pixel < 0)
    start_pixel = 0;
  return start_pixel;
}

void draw_nosd(void)
{
  ssd1306_Fill(Black);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "duckyPad V%d.%d.%d", fw_version_major, fw_version_minor, fw_version_patch);
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 0);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Please Insert");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 15);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "SD card");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 27);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Instructions:");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 41);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "duckyPad.com");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 53);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_UpdateScreen();
}

void draw_noprofile(void)
{
  ssd1306_Fill(Black);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "No Valid Profiles");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 10);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);


  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Use Latest");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 30);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Configurator");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 43);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_UpdateScreen();
}

void ui_test(void)
{
  ssd1306_Fill(Black);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "all good!");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 10);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_UpdateScreen();
}
