#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "shared.h"
#include "neopixel.h"

uint8_t ws_padding_buf[NEOPIXEL_PADDING_BUF_SIZE];
uint8_t ws_spi_buf[WS_SPI_BUF_SIZE];

void spi_fastwrite_buf_size_even(uint8_t *pData, int count)
{
  while(count > 0)
  {
    while(!__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE))
      ;
    hspi1.Instance->DR = *((uint16_t *)pData);
    pData += sizeof(uint16_t);
    count -= 2;
  }
}

uint8_t red_after_brightness[NEOPIXEL_COUNT];
uint8_t green_after_brightness[NEOPIXEL_COUNT];
uint8_t blue_after_brightness[NEOPIXEL_COUNT];

// make sure spi speed is between 8MHz and 10MHz
void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t brightness)
{
  float brightness_percent = (float)brightness/100;
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    red_after_brightness[i] = (float)red[i] * brightness_percent;
    green_after_brightness[i] = (float)green[i] * brightness_percent;
    blue_after_brightness[i] = (float)blue[i] * brightness_percent;
  }

  __disable_irq();
  HAL_SPI_Transmit(&hspi1, ws_padding_buf, NEOPIXEL_PADDING_BUF_SIZE, 5);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    memset(ws_spi_buf, 0, WS_SPI_BUF_SIZE);
    for (int j = 0; j < 8; ++j)
    {
      if((uint8_t)(green_after_brightness[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[j] = WS_BIT_1;
      else
        ws_spi_buf[j] = WS_BIT_0;

      if((uint8_t)(red_after_brightness[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[8 + j] = WS_BIT_1;
      else
        ws_spi_buf[8 + j] = WS_BIT_0;

      if((uint8_t)(blue_after_brightness[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[16 + j] = WS_BIT_1;
      else
        ws_spi_buf[16 + j] = WS_BIT_0;
    }
    HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_SET);
    spi_fastwrite_buf_size_even(ws_spi_buf, WS_SPI_BUF_SIZE);
    spi_fastwrite_buf_size_even(ws_padding_buf, NEOPIXEL_PADDING_BUF_SIZE);
    HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_RESET);
  }
  __enable_irq();
}

//---------------- animation ----------------

uint8_t pixel_map[NEOPIXEL_COUNT] = {2, 1, 0, 3, 4, 5, 8, 7, 6, 9, 10, 11, 14, 13, 12};
uint8_t red_buf[NEOPIXEL_COUNT];
uint8_t green_buf[NEOPIXEL_COUNT];
uint8_t blue_buf[NEOPIXEL_COUNT];

void set_pixel_3color(uint8_t which, uint8_t r, uint8_t g, uint8_t b)
{
  red_buf[pixel_map[which]] = r;
  green_buf[pixel_map[which]] = g;
  blue_buf[pixel_map[which]] = b;
}

void redraw_bg(void)
{
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    set_pixel_3color(i, 127, 25, 25);
  }
  neopixel_show(red_buf, green_buf, blue_buf, 100);
}
