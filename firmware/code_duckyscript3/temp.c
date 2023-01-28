  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    printf("%d ", key_press_count[i]);
  printf("\n");
// printf("%d: %d %d %d\n", i, red_addr, green_addr, blue_addr);
  // printf("%d: %d %d %d\n", i, red, green, blue);
void keypress_wrap(uint8_t keynum)
{
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/key%d.dsb", p_cache.profile_fn, keynum+1);
  printf("%s\n", temp_buf);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto kp_end;
  kp_end:
  f_close(&sd_file);
}


  /*
  HID SET RGB LED COLOUR
  -----------
  PC to duckyPad:
  [0]   report_id: always 5
  [1]   seq number
  [2]   command: 5
  Key 1 (RGB) : 3 4 5
  Key 2 : 6 7 8
  Key 3 : 9 10 11
  Key 4 : 12 13 14
  Key 5 : 15 16 17
  Key 6 : 18 19 20
  Key 7 : 21 22 23
  Key 8 : 24 25 26
  Key 9 : 27 28 29
  Key 10 : 30 31 32
  Key 11 : 33 34 35
  Key 12 : 36 37 38
  Key 13 : 39 40 41
  Key 14 : 42 43 44
  Key 15 : 45 46 47
  -----------
  duckyPad to PC
  [0]   report_id: always 4
  [1]   seq number (same as above)
  [2]   0 = OK
  */
  // else if(command_type == HID_COMMAND_SW_COLOR)
  // {
  //   oled_full_brightness();
  //   for (int i = 3; i <= 47; ++i)
  //   {
  //     uint8_t key_number_0_to_14 = (i-3)/3;
  //     uint8_t rgb_index = (i-3) % 3;
  //     p_cache.individual_key_color[key_number_0_to_14][rgb_index] = hid_rx_buf[i];
  //   }
  //   USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
  // }

void terminate_newline(char* msg)
{
  char* to_find = strchr(msg, '\n');
  if(to_find != NULL)
    *to_find = 0;
  to_find = strchr(msg, '\r');
  if(to_find != NULL)
    *to_find = 0;
}


const char cmd_KATAKANAHIRAGANA[] = "KATAKANAHIRAGANA";
const char cmd_HENKAN[] = "HENKAN";
const char cmd_MUHENKAN[] = "MUHENKAN";
const char cmd_KATAKANA[] = "KATAKANA";
const char cmd_HIRAGANA[] = "HIRAGANA";
const char cmd_ZENKAKUHANKAKU[] = "ZENKAKUHANKAKU";


#define KEY_KATAKANAHIRAGANA 0x88
// #define KEY_YEN 0x89
#define KEY_HENKAN 0x8a
#define KEY_MUHENKAN 0x8b

// #define KEY_HANGEUL 0x90 // korea
// #define KEY_HANJA 0x91 // korea
#define KEY_KATAKANA 0x92
#define KEY_HIRAGANA 0x93
#define KEY_ZENKAKUHANKAKU 0x94

Program Size: Code=49784 RO-data=4644 RW-data=1456 ZI-data=14656  
Program Size: Code=49804 RO-data=4644 RW-data=1460 ZI-data=14268  
/*

see USB HID descriptor in usbd_hid.c, Consumer section
  0x95, 0x08,        //   Report Count (8)
  usages...

bit position corresponds to that
0x80 voldown
0x40 vol up
0x20 mute
etc
*/



// load loop status here???
  memset(key_max_loop, 0, MAPPABLE_KEY_COUNT);


  
    // printf("%d %d\n", keynum, key_press_count[keynum] % key_max_loop[keynum]);
    // printf("%d %d %d\n", p_cache.individual_key_color[keynum][0], p_cache.individual_key_color[keynum][1], p_cache.individual_key_color[keynum][2]);
    
// void mouse_test(void)
// {
//   memset(hid_tx_buf, 0, KB_BUF_SIZE);
//   hid_tx_buf[0] = 4;
//   // hid_tx_buf[2] = 20; // [1] buttons [2] x-axis, [3] y-axis [4] wheel, 0 to 127 scroll up, -1 to -127 scroll down
//   vTaskSuspendAll();USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);xTaskResumeAll();
//   printf("mouse\n");
// }
    printf("xx %d %d\n", (int8_t)this_key->code, (int8_t)this_key->code2);

if(i2c_status == HAL_OK)
    {
      uint32_t start = HAL_GetTick();
      HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x40,1,&SSD1306_Buffer[SSD1306_WIDTH * i],SSD1306_WIDTH,100);
      printf("took %dms\n", HAL_GetTick() - start);
    }

/*

void mouse_test(void)
{
  memset(hid_buf, 0, HID_BUF_SIZE);
  hid_buf[0] = 4;
  hid_buf[2] = 20; // [1] buttons [2] x-axis, [3] y-axis [4] wheel, 0 to 127 scroll up, -1 to -127 scroll down
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_buf, KB_BUF_SIZE);
  printf("hid\n");
}

// void mouse_release_all(void)
// {
//   memset(hid_buf, 0, HID_BUF_SIZE);
//   hid_buf[0] = 3;
//   USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_buf, KB_BUF_SIZE);
// }

memset(hid_tx_buf, 5, HID_TX_BUF_SIZE);
            // hid_tx_buf[0] = 5;
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, hid_tx_buf, HID_TX_BUF_SIZE);
void mouse_test(void)
{
  memset(hid_buf, 0, HID_BUF_SIZE);
  hid_buf[0] = 3;
  hid_buf[2] = 20; // [1] buttons [2] x-axis, [3] y-axis [4] wheel, 0 to 127 scroll up, -1 to -127 scroll down
  USBD_HID_SendReport(&hUsbDeviceFS, hid_buf, 8);
  printf("mouse\n");
}
*/

for (int i = 0; i < HID_RX_BUF_SIZE; ++i)
    printf("%d, ", hid_rx_buf[i]);
  printf("\ndone\n");
  memset(hid_rx_buf, 0, HID_RX_BUF_SIZE);

uint8_t is_keymap_missing(void)
{
  for (int i = 0; i < MAX_KEYMAP_SIZE; ++i)
    if(my_keymap_cache[i].is_in_use)
      return 0;
  return 1;
}

    // printf("%c %x %x\n", kk->code, kk->code, duckcode);

  // printf("%c %x %x %x\n", kk->code, kk->code, duckcode, duckcode >> 12);
  // keyboard_press(kk, 1);
  // osDelay(chardelay);

Program Size: Code=44124 RO-data=4356 RW-data=1084 ZI-data=14836  
      // printf("%d\n%s\n---\n", circumflex, read_buffer);
  printf("%x %x %x %x %x\n", circumflex, diaeresis, grave_accent, acute_accent, tilde);

/*
when key down and script is executing, 
the key should be breathing
32 frame lookup table
after its done then fade back to background color
*/

for (int i = 0; i < MAX_KEYMAPS; ++i)
    printf("%d %d %s\n", i, my_keymap_cache[i].is_in_use, my_keymap_cache[i].name);
const uint16_t _asciimap_azerty_fr[128] =
{
  0x00,             // NUL
  0x00,             // SOH
  0x00,             // STX
  0x00,             // ETX
  0x00,             // EOT
  0x00,             // ENQ
  0x00,             // ACK  
  0x00,             // BEL
  0x2a,     // BS Backspace
  0x2b,     // TAB  Tab
  0x28,     // LF Enter
  0x00,             // VT 
  0x00,             // FF 
  0x00,             // CR 
  0x00,             // SO 
  0x00,             // SI 
  0x00,             // DEL
  0x00,             // DC1
  0x00,             // DC2
  0x00,             // DC3
  0x00,             // DC4
  0x00,             // NAK
  0x00,             // SYN
  0x00,             // ETB
  0x00,             // CAN
  0x00,             // EM 
  0x00,             // SUB
  0x00,             // ESC
  0x00,             // FS 
  0x00,             // GS 
  0x00,             // RS 
  0x00,             // US 

  0x2c,      //  ' '
  0x38,    // !
  0x20,    // "
  0x20|ALT_GR,    // #
  0x30,    // $
  0x34|SHIFT,    // %
  0x1e,    // &
  0x21,          // '
  0x22,    // (
  0x2d,    // )
  0x32,    // *
  0x2e|SHIFT,    // +
  0x10,          // ,
  0x23,          // -
  0x36|SHIFT,          // .
  0x37|SHIFT,          // /
  0x27|SHIFT,          // 0
  0x1e|SHIFT,          // 1
  0x1f|SHIFT,          // 2
  0x20|SHIFT,          // 3
  0x21|SHIFT,          // 4
  0x22|SHIFT,          // 5
  0x23|SHIFT,          // 6
  0x24|SHIFT,          // 7
  0x25|SHIFT,          // 8
  0x26|SHIFT,          // 9
  0x37,      // :
  0x36,          // ;
  0x64,      // <
  0x2e,          // =
  0x64|SHIFT,      // >
  0x10|SHIFT,      // ?
  0x27|ALT_GR,      // @
  0x14|SHIFT,      // A
  0x05|SHIFT,      // B
  0x06|SHIFT,      // C
  0x07|SHIFT,      // D
  0x08|SHIFT,      // E
  0x09|SHIFT,      // F
  0x0a|SHIFT,      // G
  0x0b|SHIFT,      // H
  0x0c|SHIFT,      // I
  0x0d|SHIFT,      // J
  0x0e|SHIFT,      // K
  0x0f|SHIFT,      // L
  0x33|SHIFT,      // M
  0x11|SHIFT,      // N
  0x12|SHIFT,      // O
  0x13|SHIFT,      // P
  0x04|SHIFT,      // Q
  0x15|SHIFT,      // R
  0x16|SHIFT,      // S
  0x17|SHIFT,      // T
  0x18|SHIFT,      // U
  0x19|SHIFT,      // V
  0x1d|SHIFT,      // W
  0x1b|SHIFT,      // X
  0x1c|SHIFT,      // Y
  0x1a|SHIFT,      // Z
  0x22|ALT_GR,          // [
  0x25|ALT_GR,          // bslash
  0x2d|ALT_GR,          // ]
  0x26|ALT_GR,    // ^
  0x25,    // _
  0x24|ALT_GR,          // `
  0x14,      // a
  0x05,      // b
  0x06,      // c
  0x07,      // d
  0x08,      // e
  0x09,      // f
  0x0a,      // g
  0x0b,      // h
  0x0c,      // i
  0x0d,      // j
  0x0e,      // k
  0x0f,      // l
  0x33,      // m
  0x11,      // n
  0x12,      // o
  0x13,      // p
  0x04,      // q
  0x15,      // r
  0x16,      // s
  0x17,      // t
  0x18,      // u
  0x19,      // v
  0x1d,      // w
  0x1b,      // x
  0x1c,      // y
  0x1a,      // z
  0x21|ALT_GR,    // {
  0x23|ALT_GR,    // |
  0x2e|ALT_GR,    // }
  0x1f|ALT_GR,    // ~
  0       // DEL
};

const uint16_t _asciimap_azerty_be[128] =
{
  0x00,             // NUL
  0x00,             // SOH
  0x00,             // STX
  0x00,             // ETX
  0x00,             // EOT
  0x00,             // ENQ
  0x00,             // ACK  
  0x00,             // BEL
  0x2a,     // BS Backspace
  0x2b,     // TAB  Tab
  0x28,     // LF Enter
  0x00,             // VT 
  0x00,             // FF 
  0x00,             // CR 
  0x00,             // SO 
  0x00,             // SI 
  0x00,             // DEL
  0x00,             // DC1
  0x00,             // DC2
  0x00,             // DC3
  0x00,             // DC4
  0x00,             // NAK
  0x00,             // SYN
  0x00,             // ETB
  0x00,             // CAN
  0x00,             // EM 
  0x00,             // SUB
  0x00,             // ESC
  0x00,             // FS 
  0x00,             // GS 
  0x00,             // RS 
  0x00,             // US 

  0x2c,      //  ' '
  0x25,    // !
  0x20,    // "
  0x20|ALT_GR,    // #
  0x30,    // $
  0x34|SHIFT,    // %
  0x1e,    // &
  0x21,          // '
  0x22,    // (
  0x2d,    // )
  0x30|SHIFT,    // *
  0x38|SHIFT,    // +
  0x10,          // ,
  0x2e,          // -
  0x36|SHIFT,          // .
  0x37|SHIFT,          // /
  0x27|SHIFT,          // 0
  0x1e|SHIFT,          // 1
  0x1f|SHIFT,          // 2
  0x20|SHIFT,          // 3
  0x21|SHIFT,          // 4
  0x22|SHIFT,          // 5
  0x23|SHIFT,          // 6
  0x24|SHIFT,          // 7
  0x25|SHIFT,          // 8
  0x26|SHIFT,          // 9
  0x37,      // :
  0x36,          // ;
  0x64,      // <
  0x38,          // =
  0x64|SHIFT,      // >
  0x10|SHIFT,      // ?
  0x1f|ALT_GR,      // @
  0x14|SHIFT,      // A
  0x05|SHIFT,      // B
  0x06|SHIFT,      // C
  0x07|SHIFT,      // D
  0x08|SHIFT,      // E
  0x09|SHIFT,      // F
  0x0a|SHIFT,      // G
  0x0b|SHIFT,      // H
  0x0c|SHIFT,      // I
  0x0d|SHIFT,      // J
  0x0e|SHIFT,      // K
  0x0f|SHIFT,      // L
  0x33|SHIFT,      // M
  0x11|SHIFT,      // N
  0x12|SHIFT,      // O
  0x13|SHIFT,      // P
  0x04|SHIFT,      // Q
  0x15|SHIFT,      // R
  0x16|SHIFT,      // S
  0x17|SHIFT,      // T
  0x18|SHIFT,      // U
  0x19|SHIFT,      // V
  0x1d|SHIFT,      // W
  0x1b|SHIFT,      // X
  0x1c|SHIFT,      // Y
  0x1a|SHIFT,      // Z
  0x2f|ALT_GR,          // [
  0x64|ALT_GR,          // bslash
  0x30|ALT_GR,          // ]
  0x23|ALT_GR,    // ^
  0x2e|SHIFT,    // _
  0x32|ALT_GR,          // `
  0x14,      // a
  0x05,      // b
  0x06,      // c
  0x07,      // d
  0x08,      // e
  0x09,      // f
  0x0a,      // g
  0x0b,      // h
  0x0c,      // i
  0x0d,      // j
  0x0e,      // k
  0x0f,      // l
  0x33,      // m
  0x11,      // n
  0x12,      // o
  0x13,      // p
  0x04,      // q
  0x15,      // r
  0x16,      // s
  0x17,      // t
  0x18,      // u
  0x19,      // v
  0x1d,      // w
  0x1b,      // x
  0x1c,      // y
  0x1a,      // z
  0x26|ALT_GR,    // {
  0x1e|ALT_GR,    // |
  0x27|ALT_GR,    // }
  0x38|ALT_GR,    // ~
  0       // DEL
};

const uint16_t _asciimap_dvorak[128] =
{
  0x00,             // NUL
  0x00,             // SOH
  0x00,             // STX
  0x00,             // ETX
  0x00,             // EOT
  0x00,             // ENQ
  0x00,             // ACK  
  0x00,             // BEL
  0x2a,     // BS Backspace
  0x2b,     // TAB  Tab
  0x28,     // LF Enter
  0x00,             // VT 
  0x00,             // FF 
  0x00,             // CR 
  0x00,             // SO 
  0x00,             // SI 
  0x00,             // DEL
  0x00,             // DC1
  0x00,             // DC2
  0x00,             // DC3
  0x00,             // DC4
  0x00,             // NAK
  0x00,             // SYN
  0x00,             // ETB
  0x00,             // CAN
  0x00,             // EM 
  0x00,             // SUB
  0x00,             // ESC
  0x00,             // FS 
  0x00,             // GS 
  0x00,             // RS 
  0x00,             // US 

  0x2c,      //  ' '
  0x1e|SHIFT,    // !
  0x14|SHIFT,    // "
  0x20|SHIFT,    // #
  0x21|SHIFT,    // $
  0x22|SHIFT,    // %
  0x24|SHIFT,    // &
  0x14,          // '
  0x26|SHIFT,    // (
  0x27|SHIFT,    // )
  0x25|SHIFT,    // *
  0x30|SHIFT,    // +
  0x1a,          // ,
  0x34,          // -
  0x08,          // .
  0x2f,          // /
  0x27,          // 0
  0x1e,          // 1
  0x1f,          // 2
  0x20,          // 3
  0x21,          // 4
  0x22,          // 5
  0x23,          // 6
  0x24,          // 7
  0x25,          // 8
  0x26,          // 9
  0x1d|SHIFT,      // :
  0x1d,          // ;
  0x1a|SHIFT,      // <
  0x30,          // =
  0x08|SHIFT,      // >
  0x2f|SHIFT,      // ?
  0x1f|SHIFT,      // @
  0x04|SHIFT,      // A
  0x11|SHIFT,      // B
  0x0c|SHIFT,      // C
  0x0b|SHIFT,      // D
  0x07|SHIFT,      // E
  0x1c|SHIFT,      // F
  0x18|SHIFT,      // G
  0x0d|SHIFT,      // H
  0x0a|SHIFT,      // I
  0x06|SHIFT,      // J
  0x19|SHIFT,      // K
  0x13|SHIFT,      // L
  0x10|SHIFT,      // M
  0x0f|SHIFT,      // N
  0x16|SHIFT,      // O
  0x15|SHIFT,      // P
  0x1b|SHIFT,      // Q
  0x12|SHIFT,      // R
  0x33|SHIFT,      // S
  0x0e|SHIFT,      // T
  0x09|SHIFT,      // U
  0x37|SHIFT,      // V
  0x36|SHIFT,      // W
  0x05|SHIFT,      // X
  0x17|SHIFT,      // Y
  0x38|SHIFT,      // Z
  0x2d,          // [
  0x31,          // bslash
  0x2e,          // ]
  0x23|SHIFT,    // ^
  0x34|SHIFT,    // _
  0x35,          // `
  0x04,      // a
  0x11,      // b
  0x0c,      // c
  0x0b,      // d
  0x07,      // e
  0x1c,      // f
  0x18,      // g
  0x0d,      // h
  0x0a,      // i
  0x06,      // j
  0x19,      // k
  0x13,      // l
  0x10,      // m
  0x0f,      // n
  0x16,      // o
  0x15,      // p
  0x1b,      // q
  0x12,      // r
  0x33,      // s
  0x0e,      // t
  0x09,      // u
  0x37,      // v
  0x36,      // w
  0x05,      // x
  0x17,      // y
  0x38,      // z
  0x2d|SHIFT,    // {
  0x31|SHIFT,    // |
  0x2e|SHIFT,    // }
  0x35|SHIFT,    // ~
  0       // DEL
};

  for (int i = 0; i < MAX_KEYMAPS; ++i)
  {
    printf("%d %d %s\n", i, keymap_name_cache[i].is_in_use, keymap_name_cache[i].name);
  }
  // if any key is being held down via HOLD command, don't repeat
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    if(key_being_held[i])
      return;
          // printf("ph: %d %d\n", hold_cache[i].key_type, hold_cache[i].code);
else if(is_released_but_not_serviced(&button_status[i]))
      {
        last_keypress = HAL_GetTick();
        if(key_being_held[i])
        {
          keypress_wrap(i);
          keydown_anime_end(i);
        }
      }
if(i <= KEY_14)
        {
          // printf("ph: %d %d\n", hold_cache[i].key_type, hold_cache[i].code);
          keydown_anime_start(i);
          handle_keypress(i, &button_status[i]); // handle the button state inside here for repeats
          if(key_being_held[i] == 0)
            keydown_anime_end(i);
        }

// printf("button %d released\n", i);
        // for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
        //   printf("%d ", key_being_held[i]);
        // printf("\n");
void print_keyname(char* keyname, uint8_t keynum, int8_t x_offset, int8_t y_offset)
{
  memset(key_name_buf, 0, FILENAME_SIZE);
  if(keyname != NULL)
    strcpy(key_name_buf, keyname);
  if(keyname == NULL || (key_name_buf[0] == nonexistent_keyname[0] && key_name_buf[1] == 0))
    key_name_buf[0] = '-';
  if(strlen(key_name_buf) > 7)
    key_name_buf[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  int8_t x_start = col_lookup[strlen(key_name_buf) - 1][col] + x_offset;
  int8_t y_start = (row + 1) * 10 + 2 + y_offset;
  if(x_start < 0)
    x_start = 0;
  if(y_start < 0)
    y_start = 0;
  ssd1306_SetCursor((uint8_t)x_start, (uint8_t)y_start);
  ssd1306_WriteString(key_name_buf, Font_6x10,White);
}
void list_profiles(uint8_t page)
{
  // char* profile_name = find_profile(6);
  // if(profile_name == NULL)
  //   return;
  // printf("%s\n", profile_name);
   // for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    //   if(is_pressed(&button_status[i]))
    //   {
    //       service_press(&button_status[i]);
    //       is_in_settings = 0;
    //       return;
    //   }
  uint32_t start = HAL_GetTick();
  for (uint8_t i = 0; i < 15; ++i)
  {
    printf("%i %s\n", i, find_profile(i));
    // print_keyname(find_profile(i), i, 0, 0);
  }
  printf("took %dms\n", HAL_GetTick() - start);
  // ssd1306_UpdateScreen();

  // while(1)
  // {
  //   HAL_IWDG_Refresh(&hiwdg);
  // }
}
      // printf("%d %c 0x%x\n", this_key->code, this_key->code, usage_id);
void list_profiles(uint8_t page)
{
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;

  sd_fresult = f_findfirst(&dir, &fno, "", "profile5_*");
  printf("find: %s\n", fno.fname);

  f_closedir(&dir);
  // for (int i = 0; i < MAX_PROFILES; ++i)
  // {
  //   printf("%d: %d\n", i, p_cache.available_profile[i]);
  // }
}

// void print_kb_buf(void)
// {
//   for (int i = 0; i < KB_BUF_SIZE; ++i)
//     printf("0x%x ", kb_buf[i]);
//   printf("\n");
// }
void list_profiles(uint8_t page)
{
  char* profile_fn;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;

  if (f_opendir(&dir, "/") != FR_OK)
    return;

  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "profile");
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      profile_fn = fno.lfname[0] ? fno.lfname : fno.fname;
      printf("%s\n", profile_fn);
      if(strncmp(temp_buf, profile_fn, strlen(temp_buf)) == 0)
      {
        uint8_t num = atoi(profile_fn + strlen(temp_buf));
        if(num == 0)
          continue;
        // if(num >= MAX_PROFILES)
        //   break;
        printf("^ok\n");
      }
    }
  }
  f_closedir(&dir);
}
void media_key_press(uint8_t k)
{
  // uint8_t usage_id = 0;
  // if(k == KEY_VOL_UP)
  //   usage_id = 0x80;
  // else if(k == KEY_VOL_DOWN)
  //   usage_id = 0x81;
  // else if(k == KEY_VOL_MUTE)
  //   usage_id = 0x7f;

  memset(kb_buf, 0, KB_BUF_SIZE);
  kb_buf[0] = 0x02;
  kb_buf[1] = 0x80;
  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, 2);
  // osDelay(30);
  // memset(kb_buf, 0, KB_BUF_SIZE);
  // kb_buf[1] = 0x40;
  // USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, 2);
}

  // ssd1306_SetCursor(18, 20);
  // ssd1306_WriteString(project_url,Font_6x10,White);
  // memset(temp_buf, 0, PATH_SIZE);
  // sprintf(temp_buf, "FW V%d.%d.%d", fw_version_major, fw_version_minor, fw_version_patch);
  // ssd1306_SetCursor(35, 35);
  // ssd1306_WriteString(temp_buf,Font_6x10,White);
  // ssd1306_UpdateScreen();
  // HAL_IWDG_Refresh(&hiwdg);
  // osDelay(1000);
  // HAL_IWDG_Refresh(&hiwdg);

  // while(1)
  // {
  //   HAL_IWDG_Refresh(&hiwdg);
  //   osDelay(30);
  // }
    # if '_' in key_file_list[0]:
    #   key_file_list.sort(key=lambda s: int(s[3:].split("_")[0])) # sort by number not by letter
    # else:
    #   key_file_list.sort(key=lambda s: int(s[3:].split(".txt")[0])) # sort by number not by letter
printf("%s", temp_buf);
    printf("%d\n", atoi(temp_buf));
    printf("%s\n", goto_next_arg(temp_buf, temp_buf+PATH_SIZE));
void shift_pixel_vertical(int8_t offset)
{
  ssd1306_WriteCommand(SSD1306_SETDISPLAYOFFSET);
  ssd1306_WriteCommand((uint8_t)offset);
}


this line: TAB PAGEDOWN F12 CTRL 
this line: F11 F1 END CONTROL 
this line: ESCAPE INSERT F2 WINDOWS 
this line: BREAK SCROLLLOCK DELETE CONTROL 
this line: SHIFT DOWNARROW ESCAPE PAGEUP 


void restore_profile(uint8_t profile_id)
{
  printf("AAA\n");
  load_profile(profile_id);
  printf("BBB\n");
  print_legend();
  printf("CCC\n");
  f_closedir(&dir);
  printf("DDD\n");
  f_close(&sd_file);
  printf("AAA\n");
}
  printf("k: 0x%x, use_mod: %d\n", k, use_mod);
  print_kb_buf();
 curr = goto_next_arg(curr, msg_end);
      p_cache.bg_color[0] = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      p_cache.bg_color[1] = atoi(curr);
      curr = goto_next_arg(curr, msg_end);
      p_cache.bg_color[2] = atoi(curr);
      
      uint8_t key_num = atoi(read_buffer + strlen(cmd_SWCOLOR));
      printf("key_num: %d\n", key_num);
      curr = goto_next_arg(curr, msg_end);
      printf("red: %d ", atoi(curr));
      curr = goto_next_arg(curr, msg_end);
      printf("green: %d ", atoi(curr));
      curr = goto_next_arg(curr, msg_end);
      printf("blue: %d\n", atoi(curr));
void handle_keypress(uint8_t keynum, but_status* b_status)
{
  uint8_t is_repeat = 1;
  while(1)
  {
    keypress_wrap(keynum);
    if(is_repeat == 0 || b_status->button_state == BUTTON_RELEASED)
      return;
    osDelay(66);
  }
}

  if(p_cache.key_fn[keynum][0] == nonexistent_keyname[0])
    return;


delaying 2 seconds...
donedelaying 2 seconds...
done
void keydown_anime_start(uint8_t idx)
{
  led_start_animation(&neo_anime[idx], keydown_color, ANIMATION_CROSS_FADE, 3);
  osDelay(99);
  led_start_animation(&neo_anime[idx], bg_color, ANIMATION_CROSS_FADE, 50);
}
        printf("%d\n", last_keypress);
        
void save_last_profile(uint8_t profile_id)
{
  printf("pid: %d\n", profile_id);
  sd_fresult = f_open(&sd_file, "last_profile.kbd", FA_CREATE_ALWAYS | FA_WRITE);
  printf("%d\n", sd_fresult);
  memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "%d", profile_id);
  sd_fresult = f_write(&sd_file, temp_buf, strlen(temp_buf), &ignore_this);
  printf("%d\n", sd_fresult);
  f_close(&sd_file);
}

  // printf("temp_buf:\n");
  // for (int i = 0; i < PATH_SIZE; ++i)
  //   printf("%d ", temp_buf[i]);

  test(next_profile);
void test(uint8_t profile_id)
{
  printf("pid: %d\n", profile_id);
}


memset(temp_buf, 0, PATH_SIZE);
  sprintf(temp_buf, "/%s/config.txt", pf_fn);

  ret = f_open(&sd_file, temp_buf, FA_READ);
  printf("lc: %d\n", ret);
  if(ret != 0)
    goto color_end;

typedef struct
{
  uint32_t last_pinchange;
  uint32_t last_rotation_dir;
  uint32_t last_rotation_ts;
  uint32_t A_status;
  uint32_t B_status;
} re_status;

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




for (int i = 0; i < 3; ++i)
      {
        curr = goto_next_arg(curr, msg_end);
        printf("%d\n", atoi(curr));
      }

  for (int i = 0; i < 5; ++i)
  {
    ret = f_open(&sd_file, temp_buf, FA_READ);
    if(ret == 4)
    {
      f_close(&sd_file);
      osDelay(10);
      continue;
    }
    else
      break;
  }

  if(ret != 0)
    goto color_end;
  printf("%d\n", ret);
  for (int i = 0; i < 3; ++i)
    printf("%d\n", bg_color[i]);
Program Size: Code=39636 RO-data=4132 RW-data=588 ZI-data=15308  
void shuffle(uint8_t *array, uint8_t array_size)
{
  osDelay(2);
  uint16_t rrrr = htim7.Instance->CNT;
  printf("t16: %d\n", rrrr);
  srand(rrrr);
  for (uint8_t i = 0; i < array_size; i++) 
  {   
    uint8_t j = rand() % array_size; 
    uint8_t t = array[j];
    array[j] = array[i];
    array[i] = t;
  }
}
  // printf("%s\n", temp_buf);

ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
ssd1306_command(0x80);                                  // the suggested ratio 0x80
ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
ssd1306_command(SSD1306_LCDHEIGHT - 1);
ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
ssd1306_command(0x0);                                   // no offset
ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
ssd1306_command(0x14);
ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
ssd1306_command(0x00);                                  // 0x0 act like ks0108
ssd1306_command(SSD1306_SEGREMAP | 0x1);
ssd1306_command(SSD1306_COMSCANDEC);
ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
ssd1306_command(0x12);
ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
ssd1306_command(0xCF);
ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
ssd1306_command(0xF1);
ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
ssd1306_command(0x40);
ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

uint8_t ssd1306_Init(void)
{ 
  // Even wachten zodat het scherm zeker opgestart is
  HAL_Delay(100);
  
  /* Init LCD */
  ssd1306_WriteCommand(0xAE); //display off
  ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode   
  ssd1306_WriteCommand(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
  ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
  ssd1306_WriteCommand(0x00); //---set low column address
  ssd1306_WriteCommand(0x10); //---set high column address
  ssd1306_WriteCommand(0x40); //--set start line address
  ssd1306_WriteCommand(0x81); //--set contrast control register
  ssd1306_WriteCommand(0xFF);
  ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127
  ssd1306_WriteCommand(0xA6); //--set normal display
  ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64)
  ssd1306_WriteCommand(0x3F); //
  ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  ssd1306_WriteCommand(0xD3); //-set display offset
  ssd1306_WriteCommand(0x00); //-not offset
  ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
  ssd1306_WriteCommand(0xF0); //--set divide ratio
  ssd1306_WriteCommand(0xD9); //--set pre-charge period
  ssd1306_WriteCommand(0x22); //
  ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration
  ssd1306_WriteCommand(0x12);
  ssd1306_WriteCommand(0xDB); //--set vcomh
  ssd1306_WriteCommand(0x20); //0x20,0.77xVcc
  ssd1306_WriteCommand(0x8D); //--set DC-DC enable
  ssd1306_WriteCommand(0x14); //
  ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel
  
  /* Clearen scherm */
  ssd1306_Fill(Black);
  
  /* Update screen */
  ssd1306_UpdateScreen();
  
  /* Set default values */
  SSD1306.CurrentX = 0;
  SSD1306.CurrentY = 0;
  
  /* Initialized OK */
  SSD1306.Initialized = 1;
  
  /* Return OK */
  return 1;
}






char* line_end = line + strlen(line);
  char spk = parse_special_key(line);
uint8_t mod_combo(char* line, uint8_t key)
{
  uint8_t spk1, spk2;
  char *arg1, *arg2;
  char* line_end = line + strlen(line);
  arg1 = goto_next_arg(line, line_end);
  if(arg1 == NULL)
    return PARSE_ERROR;
  arg2 = goto_next_arg(arg1, line_end);

  spk1 = pecial_key(arg1);
  if(spk1 == 0)
    spk1 = arg1[0];
  spk2 = parse_special_key(arg2);
  if(spk2 == 0)
    spk2 = arg1[0];
      spk2 = arg1[0];
printf("arg1: %s\n", arg1);
printf("arg1: %s\n", arg1);
  keyboard_press(key, 1);
  osDelay(char_delay);
  keyboard_press(spk1, 0);
  osDelay(char_delay);
  if(arg2 != NULL)
  {
    keyboard_press(spk2, 0);
    osDelay(char_delay);
  }
  keyboard_release_all();
  osDelay(char_delay);
  
  return PARSE_OK;
}
  if(spk != 0)
  {
    keyboard_press(spk, 1);
    osDelay(char_delay);
    keyboard_release_all();
    osDelay(char_delay);
  }

else if(strncmp(cmd_GUI, line, strlen(cmd_GUI)) == 0)
  {
    if(mod_combo(line, KEY_LEFT_GUI) == PARSE_ERROR)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
  }
  else if(strncmp(cmd_CONTROL, line, strlen(cmd_CONTROL)) == 0)
  {
    if(mod_combo(line, KEY_LEFT_CTRL) == PARSE_ERROR)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
  }
  else if(strncmp(cmd_SHIFT, line, strlen(cmd_SHIFT)) == 0)
  {
    if(mod_combo(line, KEY_LEFT_SHIFT) == PARSE_ERROR)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
  }
  else if(strncmp(cmd_ALT, line, strlen(cmd_ALT)) == 0)
  {
    if(mod_combo(line, KEY_LEFT_ALT) == PARSE_ERROR)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
  }




  printf("1: %s\n2:%s\n", arg1, arg2);
  printf("1: %d\n2:%d\n", spk1, spk2);

else if(strncmp(cmd_DELAY, line, strlen(cmd_DELAY)) == 0)
  {
    char* arg = goto_next_arg(line, line_end);
    if(arg == NULL)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    uint16_t delay = atoi(arg);
    if(delay == 0)
    {
      result = PARSE_ERROR;
      goto parse_end;
    }
    osDelay(delay);
  }
  // printf("1: %s\n2:%s", arg1, arg2);

    printf("new char_delay is: %d\n", argg);

uint16_t get_arg(char* line)
{
  char* arg = goto_next_arg(line, strlen(line));
  if(arg == NULL)
    return PARSE_ERROR;
  return atoi(arg);
}


uint8_t bg_color[THREE] = {16, 64, 128};
uint8_t bg_color[THREE] = {80, 0, 200};

void get_random_unique_arr(void)
{
  memset(rand_order_buf, 0, 255);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    uint8_t exist = 0;
    while(1)
    {
      uint8_t this_rand = rand() % NEOPIXEL_COUNT;
      for (int jjj = 0; jjj < NEOPIXEL_COUNT; ++jjj)
        if(this_rand == rand_order_buf[jjj])
        {
          exist = 1;
          break;
        }
    }
    

  }
}


void animation_task_start(void const * argument)
{
  uint32_t once = 0;
  anime_init();
  for(;;)
  {
    if(once == 0)
    {
      boot_animation();
      once = 1;
    }
    led_animation_handler();
    osDelay(33);
  }  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    rand_order_buf[i] = i;
  shuffle(rand_order_buf, NEOPIXEL_COUNT);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    printf("%d: %d\n", i, rand_order_buf[i]);
}uint8_t is_used[NEOPIXEL_COUNT];

void get_random_unique_arr(void)
{
  srand(HAL_GetTick());
  memset(is_used, 0, 0);
  memset(rand_order_buf, 0, 0);
  uint8_t im = 0;
  for (int in = 0; in < NEOPIXEL_COUNT && im < NEOPIXEL_COUNT; ++in) 
  {
    uint8_t r = rand() % (in + 1); /* generate a random number 'r' */

    if (is_used[r])
      /* we already have 'r' */
      r = in; /* use 'in' instead of the generated number */

    rand_order_buf[im++] = r; /* +1 since your range begins from 1 */
    is_used[r] = 1;
  }
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    printf("%d: %d\n", i, rand_order_buf[i]);
}
void get_random_unique_arr(void)
{
  uint8_t in;
  uint8_t im = 0;
  for (in = 0; in < NEOPIXEL_COUNT && im < NEOPIXEL_COUNT; ++in)
  {
    int rn = NEOPIXEL_COUNT - in;
    int rm = NEOPIXEL_COUNT - im;
    if (rand() % rn < rm)    
      rand_order_buf[im++] = in;
  }
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    printf("%d: %d\n", i, rand_order_buf[i]);
}
void keypress_anime_handler(uint8_t idx)
{
  uint8_t colors[THREE] = {255, 0, 0};
  // for (int i = 0; i < THREE; ++i)
  //   colors[i] = 255 - bg_color[i];
  led_start_animation(&neo_anime[idx], colors, ANIMATION_CROSS_FADE, 3);
  osDelay(90);
  led_start_animation(&neo_anime[idx], bg_color, ANIMATION_CROSS_FADE, 20);
}

void animation_test(void)
{
  ;
}

void randcolor(uint8_t* red, uint8_t* blue, uint8_t* green)
{
  *red = rand() % 256;
  *blue = rand() % (256 - *red);
  *green = 256 - *red - *blue;
}

if(frame_counter % (ANIME_FPS * 5) == 0)
  {
    randcolor(&bg_color[0], &bg_color[1], &bg_color[2]);
    for (int i = 0; i < NEOPIXEL_COUNT; ++i)
      led_start_animation(&neo_anime[i], bg_color, ANIMATION_CROSS_FADE, (ANIME_FPS * 5));
  }
  void anime_init(void)
{
  uint8_t colors[THREE];
  randcolor(&colors[0], &colors[1], &colors[2]);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    led_animation_init(&neo_anime[i], i);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    led_start_animation(&neo_anime[i], colors, ANIMATION_CROSS_FADE, (ANIME_FPS * 2));
}
void animation_test(void)
{
  uint8_t colors[THREE];
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    led_start_animation(&neo_anime[i], colors, ANIMATION_MY_RAND, 30);
}
void randcolor(uint8_t* red, uint8_t* blue, uint8_t* green)
{
  while(1) 
  {
    *red = rand() % 256;
    *blue = rand() % (256 - *red);
    *green = 256 - *red - *blue;
    if(*red <= 50 || *green <= 50 || *blue <= 50)
      return;
  }
}
void animation_task_start(void const * argument)
{
  uint32_t now = HAL_GetTick();
  uint8_t ddddd = 0;
  anime_init();
  for(;;)
  {
    if(ddddd == 0 && HAL_GetTick() - now > 2000)
    {
      animation_test();
      ddddd = 1;
    }
    led_animation_handler();
    osDelay(30);
  }
}



uint8_t action[THREE];
#define ANIMATION_MY_RAND 3
#define ANIME_INCREASE 0
#define ANIME_DECREASE 1
#define ANIME_HOLD 2
else if(neo_anime[idx].animation_type == ANIMATION_MY_RAND)
    {
      if(current_frame % (ANIME_FPS * 5) == 0)
        for (int i = 0; i < THREE; ++i)
          action[i] = rand() % 2;
      for (int i = 0; i < THREE; ++i)
      {
        if(action[i] == ANIME_INCREASE && neo_anime[idx].target_color[i] < 255)
          neo_anime[idx].target_color[i]+=1;
        else if(action[i] == ANIME_DECREASE && neo_anime[idx].target_color[i] > 1)
          neo_anime[idx].target_color[i]-=1;
      }
      set_pixel_index(neo_anime[idx].index, neo_anime[idx].target_color[0], neo_anime[idx].target_color[1], neo_anime[idx].target_color[2]);
    }



void led_animation_handler(void)
{
  frame_counter++;
  for (int hhhhhh = 0; hhhhhh < NEOPIXEL_COUNT; ++hhhhhh)
  {
    uint32_t current_frame = frame_counter - neo_anime[hhhhhh].animation_start;
    if(neo_anime[hhhhhh].animation_type == ANIMATION_NO_ANIMATION)
      continue;
    else if(neo_anime[hhhhhh].animation_type == ANIMATION_FULLY_ON)
      set_pixel_index(neo_anime[hhhhhh].index, neo_anime[hhhhhh].target_color[0], neo_anime[hhhhhh].target_color[1], neo_anime[hhhhhh].target_color[2]);
    else if(neo_anime[hhhhhh].animation_type == ANIMATION_CROSS_FADE)
    {
      if(current_frame <= neo_anime[hhhhhh].animation_duration)
      {
        for (int i = 0; i < THREE; ++i)
        {
          neo_anime[hhhhhh].current_color[i] += neo_anime[hhhhhh].step[i];
          if(neo_anime[hhhhhh].current_color[i] > 255)
            neo_anime[hhhhhh].current_color[i] = 255;
          if(neo_anime[hhhhhh].current_color[i] < 0)
            neo_anime[hhhhhh].current_color[i] = 0;
        }
      }
      else
      {
         for (int i = 0; i < THREE; ++i)
          neo_anime[hhhhhh].current_color[i] = neo_anime[hhhhhh].target_color[i];
      }
      set_pixel_index(neo_anime[hhhhhh].index, neo_anime[hhhhhh].current_color[0], neo_anime[hhhhhh].current_color[1], neo_anime[hhhhhh].current_color[2]);
    }
    else if(neo_anime[hhhhhh].animation_type == ANIMATION_MY_RAND)
    {
      if(current_frame % (ANIME_FPS * 5) == 0)
        for (int i = 0; i < THREE; ++i)
          neo_anime[hhhhhh].action[i] = rand() % 3;
      for (int i = 0; i < THREE; ++i)
      {
        if(neo_anime[hhhhhh].action[i] == ANIME_INCREASE && neo_anime[hhhhhh].target_color[i] < 255)
          neo_anime[hhhhhh].target_color[i]++;
        else if(neo_anime[hhhhhh].action[i] == ANIME_DECREASE && neo_anime[hhhhhh].target_color[i] > 0)
          neo_anime[hhhhhh].target_color[i]--;
      }
      set_pixel_index(neo_anime[hhhhhh].index, neo_anime[hhhhhh].target_color[0], neo_anime[hhhhhh].target_color[1], neo_anime[hhhhhh].target_color[2]);
    }
  }
  taskENTER_CRITICAL();
  neopixel_show(red_buf, green_buf, blue_buf);
  taskEXIT_CRITICAL();
}
void anime_init(void)
{
  uint8_t colors[THREE] = {64, 64, 64};
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    led_animation_init(&neo_anime[i], i);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
    led_start_animation(&neo_anime[i], colors, ANIMATION_MY_RAND, 0);
}


void animation_test(void)
{
  uint8_t colors[THREE];
  randcolor(&colors[0], &colors[1], &colors[2]);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    led_start_animation(&neo_anime[i], colors, ANIMATION_CROSS_FADE, ANIME_FPS * 5);
    // osDelay(50);
  }
}

// this runs every single frame
void led_animation_handler(void)
{
  frame_counter++;
  for (int kkkk = 0; kkkk < NEOPIXEL_COUNT; ++kkkk)
  {
    printf("kkkk %d\n", kkkk);
    uint32_t current_frame = frame_counter - neo_anime[kkkk].animation_start;
    if(neo_anime[kkkk].animation_type == ANIMATION_NO_ANIMATION)
      return;
    else if(neo_anime[kkkk].animation_type == ANIMATION_FULLY_ON)
      set_pixel_index(neo_anime[kkkk].index, neo_anime[kkkk].target_color[0], neo_anime[kkkk].target_color[1], neo_anime[kkkk].target_color[2]);
    else if(neo_anime[kkkk].animation_type == ANIMATION_CROSS_FADE)
    {
      if(current_frame <= neo_anime[kkkk].animation_duration)
      {
        for (int i = 0; i < THREE; ++i)
        {
          neo_anime[kkkk].current_color[i] += neo_anime[kkkk].step[i];
          if(neo_anime[kkkk].current_color[i] > 255)
            neo_anime[kkkk].current_color[i] = 255;
          if(neo_anime[kkkk].current_color[i] < 0)
            neo_anime[kkkk].current_color[i] = 0;
        }
      }
      else
      {
        for (int i = 0; i < THREE; ++i)
          neo_anime[kkkk].current_color[i] = neo_anime[kkkk].target_color[i];
      }
      set_pixel_index(neo_anime[kkkk].index, neo_anime[kkkk].current_color[0], neo_anime[kkkk].current_color[1], neo_anime[kkkk].current_color[2]);
    }
  }

  taskENTER_CRITICAL();
  neopixel_show(red_buf, green_buf, blue_buf);
  taskEXIT_CRITICAL();
}
void randcolor(uint8_t* red, uint8_t* blue, uint8_t* green)
{
  while(1) 
  {
    *red = rand() % 256;
    *blue = rand() % (256 - *red);
    *green = 256 - *red - *blue;
    // if(*red <= 200 && *green <= 200 && *blue <= 200)
      return;
  }
}
void animation_test(void)
{
  uint8_t colors[THREE];
  randcolor(&colors[0], &colors[1], &colors[2]);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    led_start_animation(&neo_anime[i], colors, ANIMATION_CROSS_FADE, 30);
    osDelay(rand() % 200);
  }
}

void randcolor(uint8_t* red, uint8_t* blue, uint8_t* green)
{
  *red = rand() % 256;
  *blue = rand() % (256 - *red);
  *green = 256 - *red - *blue;
}

void animation_test(void)
{
  uint8_t red, green, blue;
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    randcolor(&red, &green, &blue);
    set_pixel_index(i, red, green, blue);
  }
  taskENTER_CRITICAL();
  neopixel_show(red_buf, green_buf, blue_buf);
  taskEXIT_CRITICAL();
  printf("fc: %d\n", frame_counter);
}


  // printf("rgb %d %d %d\n", *red, *blue, *green);
// use this to draw 2D heat map thing
// (0, 0) top left, (2, 0) top right
// (0, 4) bottom left, (2, 4) bottom right
void set_pixel_xy(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t index = x + y * 3;
  red_buf[pixel_map[index]] = r;
  green_buf[pixel_map[index]] = g;
  blue_buf[pixel_map[index]] = b;
}


void rand256(uint8_t* rrrr, uint8_t* gggg, uint8_t* bbbb)
void animation_test(void)
{
  count = (count + 1) % NEOPIXEL_COUNT;
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    if(i == count)
      set_pixel_index(i, 128, 128, 128);
    else
      set_pixel_index(i, 16, 32, 128);
  }
  taskENTER_CRITICAL();
  neopixel_show(red_buf, green_buf, blue_buf);
  taskEXIT_CRITICAL();
}


void animation_show(void)
{
  ;
}
void animation_test(void)
{
  count = (count + 1) % NEOPIXEL_COUNT;
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    if(i == count)
      set_pixel_index(i, 128, 128, 128);
    else
      set_pixel_index(i, 16, 32, 128);
  }
  neopixel_show(red_buf, green_buf, blue_buf);
}
uint8_t red_io[NEOPIXEL_COUNT];
uint8_t green_io[NEOPIXEL_COUNT];
uint8_t blue_io[NEOPIXEL_COUNT];
void animation_test(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; i+=3)
  {
    set_pixel(i, 128, 0, 0);
    set_pixel(i+1, 0, 128, 0);
    set_pixel(i+2, 0, 0, 128);
  }
  neopixel_show(red_buf, green_buf, blue_buf);
}


typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} onepixel;
onepixel pixel_list[NEOPIXEL_COUNT];
void set_pixel(uint8_t which, uint8_t r, uint8_t g, uint8_t b)
{
  pixel_list[which].red = r;
  pixel_list[which].green = g;
  pixel_list[which].blue = b;
}

uint8_t red_test[NEOPIXEL_COUNT];
uint8_t green_test[NEOPIXEL_COUNT];
uint8_t blue_test[NEOPIXEL_COUNT];
memset(red_test, 0, NEOPIXEL_COUNT);
  memset(green_test, 32, NEOPIXEL_COUNT);
  memset(blue_test, 64, NEOPIXEL_COUNT);
  neopixel_show(red_test, green_test, blue_test);
  neopixel_show(red_test, green_test, blue_test);
  else if(strncmp(cmd_ENTER, line, strlen(cmd_ENTER)) == 0)
  {
    keyboard_press(KEY_RETURN, 1);
    osDelay(char_delay);
    keyboard_release_all();
    osDelay(char_delay);
  }
Code=37588 RO-data=4096 RW-data=552 ZI-data=14224  
else if(strncmp(cmd_ESC, line, strlen(cmd_ESC)) == 0)
  {
    keyboard_press(KEY_ESC, 1);
    osDelay(char_delay);
    keyboard_release_all();
    osDelay(char_delay);
  }
char* find_profile(uint8_t pid)
{
  char* profile_fn;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_SIZE - 1;

  if (f_opendir(&dir, "/") != FR_OK)
    return NULL;

  memset(temp_buf, 0, FILENAME_SIZE);
  sprintf(temp_buf, "profile%d_", pid);
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      profile_fn = fno.lfname[0] ? fno.lfname : fno.fname;
      if(strncmp(temp_buf, profile_fn, strlen(temp_buf)) == 0)
        return profile_fn;
    }
  }
  f_closedir(&dir);
  return NULL;
}
LUL LUL TriHard ANELE 
void parser_test(void)
{
  scan_profiles(MAX_PROFILES, available_profile);
  for (int i = 0; i < MAX_PROFILES; ++i)
    if(available_profile[i])
      printf("profile %d\n", i);
}


void parser_test(void)
{
  char* ppppp = find_profile(3);
  if(ppppp == NULL)
    return;
  memset(p_cache.profile_fn, 0, FILENAME_SIZE);
  strcpy(p_cache.profile_fn, ppppp);
  printf("%s\n", p_cache.profile_fn);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    memset(p_cache.key_fn[i], 0, KEYNAME_SIZE);
    strcpy(p_cache.key_fn[i], get_keyname(p_cache.profile_fn, i));
  }
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    printf("%d: %s\n", i, get_keyname(p_cache.profile_fn, i));
}


void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(10);
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
      if(is_fresh_pressed(&button_status[i]))
      {
        printf("%d\n", i);
        if(i < 15)
          handle_keypress(i);
        else if(i == 21) // -
          change_profile(PREV_PROFILE);
        else if(i == 22) // +
          change_profile(NEXT_PROFILE);
        service_fresh_press(&button_status[i]);
      }
    osDelay(30);
  }
}


for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    printf("%d: %s\n", i, get_keyname(p_cache.profile_fn, i));
  }
Program Size: Code=28544 RO-data=3436 RW-data=508 ZI-data=10228  
Program Size: Code=32416 RO-data=3576 RW-data=528 ZI-data=12440  

void handle_keypress(uint8_t keynum)
{
  char* profile_name = find_profile(current_profile);
  if(profile_name == NULL)
    return;
  char* keyfile_name = find_key(profile_name, keynum + 1);
  if(keyfile_name == NULL)
    return;
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s/%s", profile_name, keyfile_name);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    goto end;
  printf("opened %s\n", temp_buf);
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    parse_line(read_buffer);
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  end:
  f_close(&sd_file);
  return;
}




void change_profile(uint8_t dir)
{
  uint16_t count = 0;
  while(1)
  {
    count++;
    if(dir == NEXT_PROFILE)
      current_profile++;
    else
      current_profile--;
    if(current_profile > 16)
      current_profile = 0;
    char* ddddd = find_profile(current_profile);
    if(ddddd != NULL)
    {
      print_legend(current_profile, ddddd);
      return;
    }
    else if(count > 16)
    {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("no valid profiles!",Font_6x10,White);
      ssd1306_UpdateScreen();
      return;
    }
  }
}
void change_profile(uint8_t dir)
{
  uint16_t count = 0;
  while(1)
  {
    count++;
    if(dir == NEXT_PROFILE)
      current_profile++;
    else
      current_profile--;
    char* ddddd = find_profile(current_profile);
    if(ddddd != NULL)
    {
      print_legend(current_profile, ddddd);
      return;
    }
    else if(count > 255)
    {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("no valid profiles!",Font_6x10,White);
      ssd1306_UpdateScreen();
      return;
    }
  }
}
void parser_test(void)
{
  for (int i = 0; i < 4; ++i)
  {
    printf(">>>profile%d\n", i);
    char* ddddd = find_profile(i);
    if(ddddd == NULL)
      continue;
    for (int j = 1; j <= 15; ++j)
      printf("%s\n", get_keyname(ddddd, j));
  }
}

char* get_keyname(char* pf_fn, uint8_t keynum)
{
  char* key_fn;
  char* start;
  char* end;
  char* ret = nonexistent_keyname;
  if(pf_fn == NULL)
    return ret;

  printf("pffn: %s\n", pf_fn);
  fno.lfname = key_lfn_buf; 
  fno.lfsize = LFN_SIZE - 1;
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s", pf_fn);
  if (f_opendir(&dir, temp_buf) != FR_OK)
    return ret;
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "key%d_", keynum);
  printf("needle: %s\n", temp_buf);
  while(1)
  {
    memset(key_lfn_buf, 0, LFN_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    key_fn = fno.lfname[0] ? fno.lfname : fno.fname;
    printf("%s\n", key_fn);
    if(strncmp(temp_buf, key_fn, strlen(temp_buf)) == 0)
    {
      start = key_fn + strlen(temp_buf);
      end = strstr(start, ".txt");
      memset(key_name_buf, 0, LFN_SIZE);
      strncpy(key_name_buf, start, end - start);
      ret = key_name_buf;
      // printf("ret %s\n", ret);
      break;
    }
  }
  f_closedir(&dir);
  return ret;
}

void parser_test(void)
{
  for (int i = 0; i < 4; ++i)
  {
    char* ddddd = find_profile(i);
    if(ddddd == NULL)
      continue;
    for (int j = 1; j <= 15; ++j)
      get_keyname(ddddd, j);
  }
}

char* get_keyname(char* pf_fn, uint8_t keynum)
{
  if(pf_fn == NULL)
    return nonexistent_keyname;
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s/key%d.txt", pf_fn, keynum + 1);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    f_close(&sd_file);
    return nonexistent_keyname;
  }
  memset(read_buffer, 0, READ_BUF_SIZE);
  f_gets(read_buffer, READ_BUF_SIZE, &sd_file);
  f_close(&sd_file);
  for (int i = 0; i < READ_BUF_SIZE; ++i)
    if(read_buffer[i] == '\r' || read_buffer[i] == '\n')
      read_buffer[i] = 0;
  if(strncmp(syntax_NAME, read_buffer, strlen(syntax_NAME)) == 0)
    return read_buffer + strlen(syntax_NAME);
  return unnamed_keyname;
}
  printf("%s\n", temp_buf);

char* find_profile(uint8_t pid)
{
  fno.lfname = lfn_buf; 
  fno.lfsize = LFN_SIZE - 1;

  if (f_opendir(&dir, "/") != FR_OK)
    return NULL;

  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "profile%d_", pid);
  while(1)
  {
    memset(lfn_buf, 0, LFN_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      profile_fn = fno.lfname[0] ? fno.lfname : fno.fname;
      if(strncmp(temp_buf, profile_fn, strlen(temp_buf)) == 0)
        return profile_fn;
    }
  }
  f_closedir(&dir);
  return NULL;
}

void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(10);
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
      if(is_fresh_pressed(&button_status[i]))
      {
        printf("%d\n", i);
        if(i < 15)
          handle_keypress(i);
        else if(i == 21) // -
          change_profile(PREV_PROFILE);
        else if(i == 22) // +
          change_profile(NEXT_PROFILE);
        service_fresh_press(&button_status[i]);
      }
    osDelay(30);
  }
}
void handle_keypress(uint8_t keynum)
{
  char* profile_name = find_profile(current_profile);
  if(profile_name == NULL)
    return;
  char* keyfile_name = find_key(profile_name, keynum + 1);
  if(keyfile_name == NULL)
    return;
  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s/%s", profile_name, keyfile_name);
  printf("%s\n", temp_buf);
  uint8_t bbbbb = f_open(&sd_file, temp_buf, FA_READ);
  if(bbbbb != 0)
  {
    printf("bbbbb %d\n", bbbbb);
    goto end;
  }
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file) != NULL)
  {
    parse_line(read_buffer);
    memset(read_buffer, 0, READ_BUF_SIZE);
  }
  end:
  f_close(&sd_file);
  return;
}




const char syntax_GUI[] = "GUI ";
const char syntax_SHIFT[] = "SHIFT ";
const char syntax_ALT[] = "ALT ";
const char syntax_CONTROL[] = "CONTROL ";
void kb_test(void)
{
  keyboard_press(KEY_LEFT_GUI);
  HAL_Delay(50);
  keyboard_press('d');
  HAL_Delay(50);
  release_all();
}


char* goto_next_arg(char* buf)
{
  char* curr = buf;
  if(curr == NULL)
    return NULL;
  char* buf_end = curr + strlen(curr);
  if(curr >= buf_end)
    return NULL;
  while(curr < buf_end && *curr != ' ')
      curr++;
  while(curr < buf_end && *curr == ' ')
      curr++;
  if(curr >= buf_end)
    return NULL;
  return curr;
}
void keypress_task_start(void const * argument)
{
  while(init_complete == 0)
    osDelay(10);
  for(;;)
  {
    for (int i = 0; i < KEY_COUNT; ++i)
      if(is_fresh_pressed(&button_status[i]))
      {
        printf("%d\n", i);
        if(i == 0)
          kb_test();
        else if(i == 7)
          parser_test();
        else if(i == 21) // -
          change_profile(PREV_PROFILE);
        else if(i == 22) // +
          change_profile(NEXT_PROFILE);
        service_fresh_press(&button_status[i]);
      }
    osDelay(30);
  }
}



  printf("pf_name %s\n", pf_name);
  printf("%s\n", temp_buf);
Program Size: Code=29790 RO-data=3646 RW-data=444 ZI-data=7396  
void print_keyname(char* keyname, uint8_t keynum)
{
  if(strlen(keyname) > 7)
    keyname[7] = 0;
  uint8_t row = keynum / 3;
  uint8_t col = keynum - row * 3;
  uint8_t x_start = col_lookup[strlen(keyname) - 1][col];
  printf("pk %d %d %d %d %s\n", keynum, col, row, x_start, keyname);
  ssd1306_SetCursor(x_start, row * 10);
  ssd1306_WriteString(keyname, Font_6x10,White);
}
void parser_test(void)
{
  char* ddddd = find_profile(3);
  print_legend(3, ddddd);
}

uint8_t find_keys(char* pf_fn, uint8_t keynum)
{
  if(pf_fn == NULL)
    return PARSE_ERROR;

  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "/%s/key%d.txt", pf_fn, keynum);
  if(f_open(&sd_file, temp_buf, FA_READ) != 0)
  {
    f_close(&sd_file);
    return PARSE_ERROR;
  }
  printf("%s\n", temp_buf);
  memset(read_buffer, 0, READ_BUF_SIZE);
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file))
    printf(">>>>%s\n", read_buffer);
  f_close(&sd_file);
  return PARSE_OK;
}


void key_display_test(void)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(COL_1_X, ROW_1_Y);
  ssd1306_WriteString("key001",Font_6x10,White);
  ssd1306_SetCursor(COL_2_X, ROW_1_Y);
  ssd1306_WriteString("key002",Font_6x10,White);
  ssd1306_SetCursor(COL_3_X, ROW_1_Y);
  ssd1306_WriteString("key003",Font_6x10,White);

  ssd1306_SetCursor(COL_1_X, ROW_2_Y);
  ssd1306_WriteString("key004",Font_6x10,White);
  ssd1306_SetCursor(COL_2_X, ROW_2_Y);
  ssd1306_WriteString("key005",Font_6x10,White);
  ssd1306_SetCursor(COL_3_X, ROW_2_Y);
  ssd1306_WriteString("key006",Font_6x10,White);

  ssd1306_SetCursor(COL_1_X, ROW_3_Y);
  ssd1306_WriteString("key007",Font_6x10,White);
  ssd1306_SetCursor(COL_2_X, ROW_3_Y);
  ssd1306_WriteString("key008",Font_6x10,White);
  ssd1306_SetCursor(COL_3_X, ROW_3_Y);
  ssd1306_WriteString("key009",Font_6x10,White);

  ssd1306_SetCursor(COL_1_X, ROW_4_Y);
  ssd1306_WriteString("key010",Font_6x10,White);
  ssd1306_SetCursor(COL_2_X, ROW_4_Y);
  ssd1306_WriteString("key011",Font_6x10,White);
  ssd1306_SetCursor(COL_3_X, ROW_4_Y);
  ssd1306_WriteString("key012",Font_6x10,White);

  ssd1306_SetCursor(COL_1_X, ROW_5_Y);
  ssd1306_WriteString("key013",Font_6x10,White);
  ssd1306_SetCursor(COL_2_X, ROW_5_Y);
  ssd1306_WriteString("key014",Font_6x10,White);
  ssd1306_SetCursor(COL_3_X, ROW_5_Y);
  ssd1306_WriteString("key015",Font_6x10,White);
  ssd1306_UpdateScreen();
}


 
    // printf("key %d not found\n", keynum);
  // printf("key %d has no name\n", keynum);
    // printf("%d: %s\n", i, keyname);

uint8_t get_keyname(char* pf_fn, uint8_t key_num)
{
  if(pf_fn == NULL)
    return PARSE_ERROR;
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  {
    memset(temp_buf, 0, LFN_SIZE);
    sprintf(temp_buf, "/%s/key%d.txt", pf_fn, i+1);
    if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    {
      f_close(&sd_file);
      continue;
    }
    printf("%s\n", temp_buf);
    memset(read_buffer, 0, READ_BUF_SIZE);
    f_gets(read_buffer, READ_BUF_SIZE, &sd_file);
    printf(">>>>%s\n", read_buffer);
    f_close(&sd_file);
  }
  return PARSE_OK;
}

void parser_test(void)
{
  char* ddddd = find_profile(1);
  // for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
  //   find_keys(ddddd, i);
  print_legend(ddddd);
}
  
  


uint8_t print_legend(char* pf_fn)
{
  if(pf_fn == NULL)
    return PARSE_ERROR;
  for (int i = 1; i <= MAPPABLE_KEY_COUNT; ++i)
  {
    memset(temp_buf, 0, LFN_SIZE);
    sprintf(temp_buf, "/%s/key%d.txt", pf_fn, i);
    printf("%s\n", temp_buf);
    if(f_open(&sd_file, temp_buf, FA_READ) != 0)
    {
      f_close(&sd_file);
      continue;
    }
    memset(read_buffer, 0, READ_BUF_SIZE);
    f_gets(read_buffer, READ_BUF_SIZE, &sd_file);
    printf(">>>>%s\n", read_buffer);
    f_close(&sd_file);
  }
  return PARSE_OK;
}
void parser_test(void)
{
  char* ddddd = find_profile(1);
  find_keys(ddddd, 1);
  find_keys(ddddd, 2);
  find_keys(ddddd, 3);
  find_keys(ddddd, 4);
  find_keys(ddddd, 5);
  for (int i = 0; i < MAPPABLE_KEY_COUNT; ++i)
    find_keys(ddddd, i);
}

char ffffff[] = "/profile1_twitch/key1.txt";

void find_keys(char* pf_fn)
{
  uint32_t now = HAL_GetTick();
  for (int i = 1; i <= MAPPABLE_KEY_COUNT; ++i)
  {
    memset(temp_buf, 0, LFN_SIZE);
    sprintf(temp_buf, "/%s/key%d.txt", pf_fn, i);
    if(f_open(&sd_file, temp_buf, FA_READ) != 0)
      continue;
    printf("%s\n", temp_buf);
    memset(read_buffer, 0, READ_BUF_SIZE);
    while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file))
      printf(">>>>%s\n", read_buffer);
  }
  printf("took %dms\n", HAL_GetTick() - now);
}
char ffffff[] = "/profile1_twitch/key1.txt";

find_keys(find_profile(1));
void find_keys(char* pf_fn)
{
  if(pf_fn == NULL)
    return;
  uint32_t now = HAL_GetTick();
  for (int i = 1; i <= MAPPABLE_KEY_COUNT; ++i)
  {
    memset(temp_buf, 0, LFN_SIZE);
    sprintf(temp_buf, "/%s/key%d.txt", pf_fn, i);
    if(f_open(&sd_file, temp_buf, FA_READ) != 0)
      continue;
    printf("%s\n", temp_buf);
    memset(read_buffer, 0, READ_BUF_SIZE);
    f_gets(read_buffer, READ_BUF_SIZE, &sd_file);
    printf(">>>>%s\n", read_buffer);
  }
  printf("took %dms\n", HAL_GetTick() - now);
}

void find_keys(char* pf_fn, uint8_t key_num)
{
  if(pf_fn == NULL)
    return;
  if(f_open(&sd_file, ffffff, FA_READ) != 0)
      return;
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file))
    printf(">>>>%s\n", read_buffer);
}

void parser_test(void)
{
  
  // for (int i = 0; i < 8; ++i)
  // {
  //   char* result = find_profile(i);
  //   if(result == NULL)
  //     continue;
  //   printf("%s\n", result);
  //   find_keys(result);
  // }

  if(f_open(&sd_file, ffffff, FA_READ) != 0)
      return;
  while(f_gets(read_buffer, READ_BUF_SIZE, &sd_file))
    printf(">>>>%s\n", read_buffer);
}
  
  



void parser_test(void)
{
  for (int i = 0; i < 16; ++i)
  {
    char* result = find_profile(i);
    if(result == NULL)
      printf("profile %d not found\n", i);
    else
      printf("%s\n", result);
  }
}
  
void parser_test(void)
{
  if (f_opendir(&dir, "/") != FR_OK)
    return;
  char* result
  uint32_t now = HAL_GetTick();
  for (int i = 0; i < 255; ++i)
    find_profile(i);
  f_closedir(&dir);
  printf("took %dms\n", HAL_GetTick() - now);
}
  void parser_test(void)
{
  if (f_opendir(&dir, "/") != FR_OK)
    return;
  for (int i = 0; i < 255; ++i)
  {
    char* result = find_profile(i);
  }
  f_closedir(&dir);
}
void parser_test(void)
{
  uint32_t now = HAL_GetTick();
  for (int i = 0; i < 32; ++i)
    find_profile(i);
  printf("took %dms\n", HAL_GetTick() - now);
}
  
char* find_profile(uint8_t pid)
{
  fno.lfname = lfn_buf; 
  fno.lfsize = LFN_SIZE - 1;

  if (f_opendir(&dir, "/") != FR_OK)
    return NULL;

  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "profile%d_", pid);
  while(1)
  {
    memset(lfn_buf, 0, LFN_SIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      filename = fno.lfname[0] ? fno.lfname : fno.fname;
      if(strncmp(temp_buf, filename, strlen(temp_buf)) == 0)
      {
        printf("%s\n", filename);
        return filename;
      }
    }
  }
  f_closedir(&dir);
  return NULL;
}
  
uint8_t find_profile(uint8_t pid)
{
  fno.lfname = lfn_buf; 
  fno.lfsize = LFN_SIZE - 1;

  sd_fresult = f_opendir(&dir, "/");
  if (sd_fresult != FR_OK)
    return sd_fresult;

  memset(temp_buf, 0, LFN_SIZE);
  sprintf(temp_buf, "profile%d_", pid);
  while(1)
  {
    memset(lfn_buf, 0, LFN_SIZE);
    sd_fresult = f_readdir(&dir, &fno);
    if (sd_fresult != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
    {
      filename = fno.lfname[0] ? fno.lfname : fno.fname;
      printf("%s\n", filename);
      if(strncmp(temp_buf, filename, strlen(temp_buf)) == 0)
        printf("found\n");
    }
  }
  f_closedir(&dir);
  return sd_fresult;
}


void parser_test(void)
{
  DIR dj;
  FILINFO fno;
  sd_fresult = f_findfirst(&dj, &fno, "", "profile1_*");
  if(sd_fresult == 0 && fno.fname[0])
    printf("%s\n", fno.fname);
}
char filename[] = "/profile1/key1.txt";

WCHAR ff_convert (WCHAR wch, UINT dir) 
{ 
    if (wch < 0x80)
      return wch;
    return 0; 
}

WCHAR ff_wtoupper (WCHAR wch) 
{ 
    if (wch >= 0x80)
      return 0; 

  if (wch >= 'a' && wch <= 'z')
    wch &= ~0x20; 
  return wch; 
}

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





mount_result = f_mount(&sd_fs, "", 1);
  if(mount_result != 0)
    spi_set_speed_neopixel();

void parser_test(void)
{
  if(mount_result != 0)
    return;
  printf("looking for profile1 folder...\n");
  sd_fresult = f_stat("profile1", NULL);
  if(sd_fresult != 0)
    return;
  printf("entering profile1 folder...\n");
  f_chdir("/dir1");
  printf("sd_fresult: %d\n", sd_fresult);
}


printf("\nmounting SD card...\n");
  int32_t mount_result = f_mount(&sd_fs, "", 1);
  printf("result: %d\n", mount_result);

void keyboard_release(uint8_t k)
{
  if(k > 127)
    return;
  uint8_t usage_id = _asciimap[k];
  if(usage_id & 0x80)
    kb_buf[1] &= ~(0x02);
  usage_id = usage_id & 0x7f;

  for (int i = 2; i < KB_BUF_SIZE; ++i)
    if(kb_buf[i] == usage_id)
      kb_buf[i] = 0;

  // print_kb_buf();
  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, KB_BUF_SIZE);
}


printf("usage_id: 0x%x\n", usage_id);
  keyboard_press('a');
  HAL_Delay(50);
  keyboard_press('H');
  HAL_Delay(50);
  keyboard_release('a');
  HAL_Delay(50);
  keyboard_release('H');
  hello UPPERCASE
  // release_all();hello worldhello world
void keyboard_release(uint8_t k)
{
  if(k == 0)
    return;

  if(k & 0x80)
  {
    kb_buf[1] &= ~(0x02);
    k &= 0x7f;
  }
  for (int i = 2; i < 5; ++i)
    if(kb_buf[i] == k)
      kb_buf[i] = 0;
  USBD_HID_SendReport(&hUsbDeviceFS, kb_buf, 5);
}
hello UPPERCASE


    keyboard_update();

    if(is_fresh_pressed(&button_status[KEY_ROT2A_CW]))
    {
      printf("rot2 cw\n");
      service_fresh_press(&button_status[KEY_ROT2A_CW]);
    }

    if(is_fresh_pressed(&button_status[KEY_ROT2B_CCW]))
    {
      printf("rot2 ccw\n");
      service_fresh_press(&button_status[KEY_ROT2B_CCW]);
    }

    if(is_fresh_pressed(&button_status[KEY_0]))
    {
      printf("key0\n");
      service_fresh_press(&button_status[KEY_0]);
    }

    HAL_Delay(30);

button_status[KEY_0].button_state = 1 - HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
  if(button_status[KEY_0].prev_state == BUTTON_RELEASED && button_status[KEY_0].button_state == BUTTON_PRESSED)
    fresh_pressed(&button_status[KEY_0]);
  else if(button_status[KEY_0].prev_state == BUTTON_PRESSED && button_status[KEY_0].button_state == BUTTON_RELEASED)
    button_status[KEY_0].service_status = BUTTON_SERVICE_UNSERVICED;

<<<<<<< HEAD
if(button_event[KEY_ROT2A_CW])
    {
      printf("rot2 cw\n");
      button_event[KEY_ROT2A_CW] = 0;
    }

    if(button_event[KEY_ROT2B_CCW])
    {
      printf("rot2 ccw\n");
      button_event[KEY_ROT2B_CCW] = 0;
    }


=======
interrupt A on both edges, on high save timestamp, on low if idled long enough trigger event
save timestamp on both edge, trigger event if last ts was mre than  20ms ago
>>>>>>> 9b77f36630b4bb392fecc9a195f6ead5bb813dc4

CW:
A: 0 B:1
A: 0 B:0
A: 1 B:0
A: 1 B:1
kkkkkkkk
falling edge on A, if B is low CW, otherwise ccw
delay 20ms

HAL_Delay(500);
    printf("CARD_PRESENT: %d\n", HAL_GPIO_ReadPin(CARD_PRESENT_GPIO_Port, CARD_PRESENT_Pin));  
uint8_t aaa = HAL_GPIO_ReadPin(E2A_GPIO_Port, E2A_Pin);
    uint8_t bbb = HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin);

    printf("A: %d   B:%d\n", HAL_GPIO_ReadPin(E2A_GPIO_Port, E2A_Pin), HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin));
    HAL_Delay(30);
hhhhhhhhhhhhhhhhhhh
printf("A: %d B:%d\n", aaa, bbb);

  last_activity = HAL_GetTick();
  if(HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin) == GPIO_PIN_SET)
    printf("ccw\n");
  else
    printf("cw\n");

typedef struct
{
  int32_t last_recv;
  int32_t curr_index;
  int32_t buf_size;
  uint8_t* buf;
} button_status;
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0,20);
  ssd1306_WriteString("Profile 1",Font_11x18,White);
  ssd1306_UpdateScreen();
  HAL_Delay(1000);
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0,0);
  ssd1306_WriteString("The quick brown fox",Font_7x10,White);
  ssd1306_SetCursor(0,10);
  ssd1306_WriteString("jumps over the lazy",Font_7x10,White);
  ssd1306_SetCursor(0,21);
  ssd1306_WriteString("doge much better",Font_7x10,White);
  ssd1306_SetCursor(0,32);
  ssd1306_WriteString("1234 1234 1234 1234 5678",Font_7x10,White);
  ssd1306_UpdateScreen();
  led_test();

void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    uint32_t head = 24 * i;
    for (int i = 0; i < 8; ++i)
    {
      if (red[i] )
      {
        /* code */
      }
      ws_spi_buf[head + i] = ;
    }
  }
}