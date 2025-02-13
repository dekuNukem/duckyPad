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