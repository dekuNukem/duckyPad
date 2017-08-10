uint8_t other[24*3] = {
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,

    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,

    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1,
    WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1, WS_BIT_1
  };  

  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
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
  printf("ready\n");

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0,20);
    ssd1306_WriteString("first",Font_11x18,White);
    ssd1306_UpdateScreen();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0,20);
    ssd1306_WriteString("second",Font_11x18,White);
    ssd1306_UpdateScreen();

  uint8_t result = 0;
  
  uint8_t data = 0x69;

  struct mouseHID_t {
      uint8_t buttons;
      int8_t x;
      int8_t y;
      int8_t wheel;
  };

  struct mouseHID_t mouseHID;
  mouseHID.buttons = 0;
  mouseHID.x = 10;
  mouseHID.y = 0;
  mouseHID.wheel = 0;

uint8_t buf[4] = {0, 10, 0, 0};
      // uint8_t result = USBD_HID_SendReport(&hUsbDeviceFS, buf, 4);
    // printf("result: %d\n", result);


    // ssd1306_Init();
  // HAL_Delay(1000);
  // ssd1306_Fill(White);
  // ssd1306_UpdateScreen();

  // HAL_Delay(1000);

  // ssd1306_SetCursor(23,23);
  // ssd1306_WriteString("Olivier",Font_11x18,Black);

  // ssd1306_UpdateScreen();


HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    // result = HAL_I2C_IsDeviceReady(&hi2c1, 0x78, 1, 100);
    result = HAL_I2C_Mem_Write(&hi2c1,0x78,0x00,1,&data,1,10);
    printf("result: %d\n", result);
    HAL_Delay(800);

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A


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


uint8_t ssd1306_Init(void)
{ 
  ssd1306_WriteCommand(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_WriteCommand(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_WriteCommand(0x80);                                  // the suggested ratio 0x80

  ssd1306_WriteCommand(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_WriteCommand(SSD1306_HEIGHT - 1);

  ssd1306_WriteCommand(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_WriteCommand(0x0);                                   // no offset
  ssd1306_WriteCommand(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_WriteCommand(SSD1306_CHARGEPUMP);                    // 0x8D
  ssd1306_WriteCommand(0x14);
  ssd1306_WriteCommand(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_WriteCommand(0x00);                                  // 0x0 act like ks0108
  ssd1306_WriteCommand(SSD1306_SEGREMAP | 0x1);
  ssd1306_WriteCommand(SSD1306_COMSCANDEC);

  ssd1306_WriteCommand(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_WriteCommand(0x12);
  ssd1306_WriteCommand(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_WriteCommand(0xCF);
  ssd1306_WriteCommand(SSD1306_SETPRECHARGE);                  // 0xd9
  ssd1306_WriteCommand(0xF1);
  ssd1306_WriteCommand(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_WriteCommand(0x40);
  ssd1306_WriteCommand(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_WriteCommand(SSD1306_NORMALDISPLAY);                 // 0xA6
  ssd1306_WriteCommand(SSD1306_DEACTIVATE_SCROLL);
  ssd1306_WriteCommand(SSD1306_DISPLAYON);
  return 1;
}


HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  ssd1306_Init();
  ssd1306_Fill(Black);
  // actually set the pixel
  ssd1306_SetCursor(0,0);
  ssd1306_WriteString("0",Font_7x10,White);
  ssd1306_SetCursor(20,0);
  ssd1306_WriteString("1",Font_7x10,White);
  ssd1306_SetCursor(20,20);
  ssd1306_WriteString("2",Font_7x10,White);
  ssd1306_UpdateScreen();