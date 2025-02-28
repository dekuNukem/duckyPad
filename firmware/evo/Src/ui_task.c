#include <string.h>
#include <stdio.h>
#include "main.h"
#include "ui_task.h"
#include "ssd1306.h"
#include "shared.h"
#include "profiles.h"
#include "ds_vm.h"

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
  CLEAR_TEMP_BUF();
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

void draw_kbled_icon(uint8_t this_led_state, uint8_t update_screen)
{
  SSD1306_COLOR color;
  // numlock
  color = (this_led_state & 0x1) ? White : Black;
  ssd1306_FillCircle(118, 4, 1, color);
  // caps lock
  color = (this_led_state & 0x2) ? White : Black;
  ssd1306_FillCircle(122, 4, 1, color);
  // scroll lock
  color = (this_led_state & 0x4) ? White : Black;
  ssd1306_FillCircle(126, 4, 1, color);
  ssd1306_Line(117, 0, 119, 0, White);
  ssd1306_Line(121, 0, 123, 0, White);
  ssd1306_Line(125, 0, 127, 0, White);
  if(update_screen)
    ssd1306_UpdateScreen();
}

uint8_t last_led_state = 255;
void update_kbled_icon(uint8_t this_led_state)
{
  if(this_led_state == last_led_state)
    return;
  draw_kbled_icon(this_led_state, 1);
  last_led_state = this_led_state;
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

  draw_kbled_icon(kb_led_status, 0);
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
  if(strlen(dps->current_kb_layout) >= 9)
  {
    strcpy(oled_line_buf, dps->current_kb_layout+5);
    oled_line_buf[strlen(oled_line_buf)-4] = 0; // don't print .txt extension
  }
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 49);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

//------------------------------
  ssd1306_UpdateScreen();
  // ssd1306_take_screenshot();
}

void oled_say(char* what)
{
  ssd1306_Fill(Black);

  ssd1306_SetCursor(center_line(strlen(what)), 30);
  ssd1306_WriteString(what, Font_6x10, White);

  ssd1306_UpdateScreen();
}

void draw_exe_error(uint8_t err_code)
{
  ssd1306_Fill(Black);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Execution Error");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 0);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_Line(0,10,128,10,White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Error Code: %d", err_code);
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 15);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Open & Save");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 27);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "on PC app");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 39);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Press Any Key");
  ssd1306_SetCursor(center_line(strlen(oled_line_buf)), 53);
  ssd1306_WriteString(oled_line_buf, Font_6x10, White);

  ssd1306_UpdateScreen();
}

void draw_fatal_error(uint8_t err_code)
{
  memset(oled_line_buf, 0, OLED_LINE_BUF_SIZE);
  sprintf(oled_line_buf, "Fatal Error:%d", err_code);
  oled_say(oled_line_buf);
  while(1)
  {
    delay_ms(500);
  }
}
