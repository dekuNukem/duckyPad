#include <string.h>
#include <stdio.h>
#include "main.h"
#include "ui_task.h"
#include "ssd1306.h"
#include "shared.h"
#include "profiles.h"

#define OLED_LINE_BUF_SIZE 64
char oled_line_buf[OLED_LINE_BUF_SIZE];

#define OLED_CHAR_WIDTH_PIXELS 6
#define OLED_WIDTH_PIXELS SSD1306_WIDTH

static const uint8_t col_lookup[7][3] = {  
   {18, 60, 103},
   {15, 57, 100},
   {12, 54, 97},
   {9, 51, 94},
   {6, 48, 91},
   {3, 45, 88},
   {0, 42, 85}
};

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

void print_keyname(char* keyname, uint8_t keynum)
{
  memset(temp_buf, 0, TEMP_BUFSIZE);
  strcpy(temp_buf, keyname);
  if(temp_buf[0] == 0)
    temp_buf[0] = '-';
  if(strlen(temp_buf) > 7)
    temp_buf[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  int8_t x_start = col_lookup[strlen(temp_buf) - 1][col];
  int8_t y_start = (row + 1) * 10 + 2;
  if(x_start < 0)
    x_start = 0;
  if(y_start < 0)
    y_start = 0;
  ssd1306_SetCursor((uint8_t)x_start, (uint8_t)y_start);
  ssd1306_WriteString(temp_buf, Font_6x10,White);
}

void draw_current_profile(void)
{
  ssd1306_Fill(Black);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "%s", profile_name_list[current_profile_number]);
	ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 0);
	ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_Line(0,10,127,10,White); // title solid line

  for (int i = 0; i < MECH_OBSW_COUNT; ++i)
    print_keyname(curr_pf_info.sw_name[i], i);

  ssd1306_UpdateScreen();
}

const char* sleep_index_to_string_lookup[SLEEP_OPTION_SIZE] = {
  "2 min",
  "5 min",
  "15 min",
  "30 min",
  "1 Hr",
  "2 Hrs",
  "Never",
};

void draw_settings(dp_global_settings *dps)
{
  ssd1306_Fill(Black);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Settings V%d.%d.%d", fw_version_major, fw_version_minor, fw_version_patch);
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 0);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_Line(0,10,128,10,White);

//------------------------------

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "1-Brightness %d%%", brightness_index_to_percent_lookup[dps->brightness_index]);
  ssd1306_SetCursor(0, 13);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

//------------------------------

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "2-Sleep: %s", sleep_index_to_string_lookup[dps->sleep_index]);
  ssd1306_SetCursor(0, 25);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

//------------------------------

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "3-Keyboard Region");
  ssd1306_SetCursor(0, 37);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "English (US)");
  // if(strlen(dps->current_kb_layout) >= 9)
  // {
  //   strcpy(oled_line_buf, dps->current_kb_layout+5);
  //   oled_line_buf[strlen(oled_line_buf)-4] = 0; // don't print .txt extension
  // }
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 49);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);
  // load_keymap_by_name(dp_settings.current_kb_layout);

//------------------------------

  // ssd1306_Line(0,115,128,115,White);

  // memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  // sprintf(oled_line_buf, "FW V%d.%d.%d", fw_version_major, fw_version_minor, fw_version_patch);
  // ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 118);
  // ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_UpdateScreen();
  // ssd1306_take_screenshot();
}
