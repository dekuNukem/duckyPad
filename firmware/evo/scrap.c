  uint8_t mount_result = f_mount(&sd_fs, "", 1);

  if(mount_result == 0)
  {
    printf("USB INIT\n");
    MX_USB_DEVICE_Init();
  }


/* USER CODE BEGIN IOCTL */
  switch (cmd)
  {
    case CTRL_SYNC:
      return RES_OK;
    case GET_SECTOR_SIZE:
      *(WORD*)buff = 512;
      return RES_OK;
    case GET_SECTOR_COUNT:
      *(DWORD*)buff = hw.sectors;
      return RES_OK;
    case GET_BLOCK_SIZE:
      *(DWORD*)buff = hw.erase_sectors;
      return RES_OK;
  }
  DRESULT res = RES_ERROR;
  return res;
  /* USER CODE END IOCTL */