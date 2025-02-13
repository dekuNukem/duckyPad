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