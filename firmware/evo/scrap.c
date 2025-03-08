
uint8_t md5File(char* file_path, uint8_t *result)
{
    size_t input_size = 0;
    MD5Context ctx;
    md5Init(&ctx);

    if(f_open(&sd_file, file_path, FA_READ) != 0)
        return 10;
    memset(bin_buf, 0, BIN_BUF_SIZE);

    while(1)
    {
        if(f_read(&sd_file, bin_buf, BIN_BUF_SIZE, &input_size) != FR_OK)
        {
            f_close(&sd_file);
            return 20;
        }
        if(input_size == 0)
            break;
        md5Update(&ctx, bin_buf, input_size);
    }
    f_close(&sd_file);
    md5Finalize(&ctx);
    memcpy(result, ctx.digest, 16);
    return 0;
}

void md5_test(void)
{
  uint8_t result[16];
  uint8_t fggg = md5File("/profile_Welcome/key1.dsb", result);
  printf("fggg %d\n", fggg);
  print_hash(result);
}



#define DUMP_STATE_IDLE 0
#define DUMP_STATE_NEW_PROFILE_DIR 1
#define DUMP_STATE_NEW_FILE 2
#define DUMP_STATE_DATA_TX 3
uint8_t sd_dump_state;
uint8_t dump_state_current_profile_number;
char* dump_state_current_file_path;
char* this_file_name;
volatile uint8_t is_in_file_access_mode;

void sd_walk(void)
{
  if(sd_dump_state == DUMP_STATE_IDLE)
  {
    sd_dump_state = DUMP_STATE_NEW_PROFILE_DIR;
    // hid reply: ack
    dump_state_current_profile_number = find_first_profile();
    if(dump_state_current_profile_number == PROFILE_OVERFLOW)
      draw_fatal_error(10);
    return;
  }

  if(sd_dump_state == DUMP_STATE_NEW_PROFILE_DIR)
  {
    if(dump_state_current_profile_number == PROFILE_OVERFLOW)
    {
      printf("all done!"); // exit exclusive access mode, HID send EOT
      sd_dump_state = DUMP_STATE_IDLE;
      return;
    }
    CLEAR_TEMP_BUF();
    sprintf(temp_buf, "/profile_%s", profile_name_list[dump_state_current_profile_number]);
    printf("Create dir: %s\n", temp_buf);
    // open dir, go to next state
    if(f_opendir(&dir, temp_buf))
      draw_fatal_error(20);
    fno.lfname = lfn_buf; 
    fno.lfsize = FILENAME_BUFSIZE - 1;
    sd_dump_state = DUMP_STATE_NEW_FILE;
    return;
  }

  if(sd_dump_state == DUMP_STATE_NEW_FILE)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    while(1)
    {
      memset(lfn_buf, 0, FILENAME_BUFSIZE);
      sd_fresult = f_readdir(&dir, &fno);
      if (fno.fattrib & AM_DIR)
        continue;
      if (sd_fresult != FR_OK || fno.fname[0] == 0)
      {
        // done with this dir, time for next
        sd_dump_state = DUMP_STATE_NEW_PROFILE_DIR;
        dump_state_current_profile_number = find_next_profile(dump_state_current_profile_number);
        f_closedir(&dir);
        printf("this profile done\n");
        return;
      }
      break;
    }
    // we found the next file
    this_file_name = fno.lfname[0] ? fno.lfname : fno.fname;
    CLEAR_TEMP_BUF();
    sprintf(temp_buf, "/profile_%s/%s", profile_name_list[dump_state_current_profile_number], this_file_name);
    current_bank = 255;
    uint32_t current_addr = 0;
    while(1)
    {
      char this_byte;
      if(read_byte_with_error(temp_buf, current_addr, &this_byte))
        draw_fatal_error(40);

      printf("%c", this_byte);
      current_addr++;

      if(current_addr >= this_dsb_file_size)
      {
        printf("EOF: %d\n", current_addr);
        break;
      }
    }
    // printf("new file: %s %dB\n", temp_buf, f_size(&sd_file));
    // sd_dump_state = DUMP_STATE_DATA_TX;
    return;
  }
  // if(sd_dump_state == DUMP_STATE_DATA_TX)
  // {
  //   printf("dumping data!\n");
  //   return;
  // }
}
void list_files(char* path)
{
  if(f_opendir(&dir, path))
    return;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_BUFSIZE - 1;
  while(1)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    sd_fresult = f_readdir(&dir, &fno);
    if (sd_fresult != FR_OK || fno.fname[0] == 0)
      break;
    if (fno.fattrib & AM_DIR)
      continue;
    char* file_name = fno.lfname[0] ? fno.lfname : fno.fname;
    printf("\t%s\n", file_name);
  }
  f_closedir(&dir);
}

void sd_walk(void)
{
  for (size_t pf_num = 0; pf_num < MAX_PROFILES; pf_num++)
  {
    if(strlen(profile_name_list[pf_num]) == 0)
      continue;
    memset(temp_buf, 0, TEMP_BUFSIZE);
    sprintf(temp_buf, "/profile_%s", profile_name_list[pf_num]);
    printf("%s\n", temp_buf);
    list_files(temp_buf);
  }
}


  printf("%ld HID RES:\n", millis());
  for (size_t i = 0; i < HID_TX_BUF_SIZE-1; i++)
    printf("%02x ", hid_cmdbuf[i]);
  printf("\n--------\n");

  printf("%ld HID MSG:\n", millis());
  for (size_t i = 0; i < USBD_CUSTOMHID_OUTREPORT_BUF_SIZE-1; i++)
    printf("%02x ", this_msg[i]);
  printf("\n--------\n");


    // uint32_t ke_start = millis();
    // play_keydown_animation(4);
    // der_init(&this_exe);
    // run_dsb(&this_exe, 18, "/profile_Numpad/key18.dsb");
    // play_keyup_animation(4);
    
    // uint32_t ke_start = millis();
    handle_sw_event(&sw_event);
    // printf("took %ldms\n", millis() - ke_start);

uint8_t read_byte(uint16_t addr, const char* dsb_path)
{
  // uint8_t bs_result = switch_bank(addr,dsb_path);
  // if(bs_result != DSB_OK)
  // {
  //   printf("BANK: %d\n", bs_result);
  //   keyboard_release_all();
  //   mouse_release_all();
  //   HAL_Delay(1000);
  //   NVIC_SystemReset();
  // }

  while(1)
  {
    uint8_t bs_result = switch_bank(addr,dsb_path);
    if(bs_result == DSB_OK)
      break;
    printf("BANK: %d\n", bs_result);
    keyboard_release_all();
    mouse_release_all();
    HAL_Delay(1000);
  }

  return bin_buf[addr%BIN_BUF_SIZE];
}


    is_busy = 1;
    // handle_sw_event(&sw_event);
    ds3_exe_result this_exe;
    der_init(&this_exe);
    run_dsb(&this_exe, 6, test_dsb_path);
    printf("result: %d\n", this_exe.result);
    is_busy = 0;

uint8_t ls = load_settings(&dp_settings);
  printf("ls: %d\n", ls);
  ls = save_settings(&dp_settings);
  printf("ls: %d\n", ls);
  ls = load_keymap_by_name("dpkm_German_macOS.txt");
  printf("ls: %d\n", ls);
  memset(temp_buf, 0, TEMP_BUFSIZE);
  ls = get_first_keymap(temp_buf);
  printf("ls: %d %s\n", ls, temp_buf);



  printf("sleep_index: %d\n", dps->sleep_index);
  printf("last_used_profile: %d\n", dps->last_used_profile);
  printf("brightness_index: %d\n", dps->brightness_index);
  printf("current_kb_layout: %s\n", dps->current_kb_layout);


uint8_t load_dsb(char* dsb_path)
{
  FILE *sd_file = fopen(dsb_path, "r");
  if(sd_file == NULL)
    return EXE_DSB_FOPEN_FAIL;
  memset(bin_buf, 0, BIN_BUF_SIZE);
  if(fread(bin_buf, 1, BIN_BUF_SIZE, sd_file) == 0)
    return EXE_DSB_FREAD_ERROR;
  fclose(sd_file);
  if(bin_buf[0] != OP_VMINFO)
    return EXE_DSB_INCOMPATIBLE_VERSION;
  if(bin_buf[2] != dsvm_version)
    return EXE_DSB_INCOMPATIBLE_VERSION;
  return EXE_OK;
}

  uint32_t start = millis();
printf("took %ldms\n", millis()-start);


/*
  this caches profile name, and printable keynames
*/
typedef struct
{
  uint8_t is_loaded;
  char dir_path[FILENAME_BUFSIZE];
  char* pf_name;
  char sw_name_firstline[MECH_OBSW_COUNT][KEYNAME_SIZE];
  uint8_t sw_color[MECH_OBSW_COUNT][THREE]; // only mechanical keys have RGB LEDs
  uint8_t sw_activation_color[MECH_OBSW_COUNT][THREE];
  uint8_t keypress_count[MAX_TOTAL_SW_COUNT];
  uint8_t dim_unused_keys;
} profile_info;


  for (size_t i = 0; i < MECH_OBSW_COUNT; i++)
  {
    printf("rr %s", curr_pf_info.sw_name[i]);
  }
void load_profile_info(void)
{
  char* dirname;
  fno.lfname = lfn_buf; 
  fno.lfsize = FILENAME_BUFSIZE - 1;
  if (f_opendir(&dir, "/") != FR_OK)
    return;

  while (1)
  {
    memset(lfn_buf, 0, FILENAME_BUFSIZE);
    if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
      break;

    if (!(fno.fattrib & AM_DIR))  // Skip non-directories
      continue;
    
    dirname = fno.lfname[0] ? fno.lfname : fno.fname;
  
    if (!is_profile_dir(dirname))  // Check if it's a valid profile directory
      continue;

    // uint8_t this_profile_number = atoi(fno.fname + strlen(profile_dir_prefix));
    // if (this_profile_number >= MAX_PROFILES)
    //   continue;

    printf("%s\n", dirname);
  }
  f_closedir(&dir);
}