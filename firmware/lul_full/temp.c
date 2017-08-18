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

falling edge on A, if B is low CW, otherwise ccw
delay 20ms

HAL_Delay(500);
    printf("CARD_PRESENT: %d\n", HAL_GPIO_ReadPin(CARD_PRESENT_GPIO_Port, CARD_PRESENT_Pin));  
uint8_t aaa = HAL_GPIO_ReadPin(E2A_GPIO_Port, E2A_Pin);
    uint8_t bbb = HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin);

    printf("A: %d   B:%d\n", HAL_GPIO_ReadPin(E2A_GPIO_Port, E2A_Pin), HAL_GPIO_ReadPin(E2B_GPIO_Port, E2B_Pin));
    HAL_Delay(30);

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