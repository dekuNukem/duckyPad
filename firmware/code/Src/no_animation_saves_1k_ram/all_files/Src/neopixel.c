#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

// make sure spi speed is between 8MHz and 10MHz
void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue)
{
  taskENTER_CRITICAL();
  HAL_SPI_Transmit(&hspi1, ws_padding_buf, NEOPIXEL_PADDING_BUF_SIZE, 5);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    memset(ws_spi_buf, 0, WS_SPI_BUF_SIZE);
    for (int j = 0; j < 8; ++j)
    {
      if((uint8_t)(green[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[j] = WS_BIT_1;
      else
        ws_spi_buf[j] = WS_BIT_0;

      if((uint8_t)(red[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[8 + j] = WS_BIT_1;
      else
        ws_spi_buf[8 + j] = WS_BIT_0;

      if((uint8_t)(blue[i] & (1 << (7 - j))) != 0)
        ws_spi_buf[16 + j] = WS_BIT_1;
      else
        ws_spi_buf[16 + j] = WS_BIT_0;
    }
    HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_SET);
    spi_fastwrite_buf_size_even(ws_spi_buf, WS_SPI_BUF_SIZE);
    spi_fastwrite_buf_size_even(ws_padding_buf, NEOPIXEL_PADDING_BUF_SIZE);
    HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_RESET);
  }
  taskEXIT_CRITICAL();
}
