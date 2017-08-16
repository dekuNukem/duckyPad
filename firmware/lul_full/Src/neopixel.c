#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shared.h"
#include "neopixel.h"

uint8_t ws_reset_buf[NEOPIXEL_RESET_BUF_SIZE];
uint8_t ws_spi_buf[WS_SPI_BUF_SIZE];

// make sure spi speed is between 8MHz and 10MHz
void neopixel_show(uint8_t* red, uint8_t* green, uint8_t* blue)
{
  HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, ws_reset_buf, NEOPIXEL_RESET_BUF_SIZE, 500);
  for (int i = 0; i < NEOPIXEL_COUNT; ++i)
  {
    uint32_t head = 24 * i;
    uint8_t this_byte;
    for (int j = 0; j < 8; ++j)
    {
      if((uint8_t)(green[i] & (1 << (7 - j))) != 0)
        this_byte = WS_BIT_1;
      else
        this_byte = WS_BIT_0;
      ws_spi_buf[head + j] = this_byte;

      if((uint8_t)(red[i] & (1 << (7 - j))) != 0)
        this_byte = WS_BIT_1;
      else
        this_byte = WS_BIT_0;
      ws_spi_buf[head + 8 + j] = this_byte;

      if((uint8_t)(blue[i] & (1 << (7 - j))) != 0)
        this_byte = WS_BIT_1;
      else
        this_byte = WS_BIT_0;
      ws_spi_buf[head + 16 + j] = this_byte;
    }
  }
  HAL_SPI_Transmit(&hspi1, ws_spi_buf, WS_SPI_BUF_SIZE, 500);
  HAL_SPI_Transmit(&hspi1, ws_reset_buf, NEOPIXEL_RESET_BUF_SIZE, 500);
  HAL_GPIO_WritePin(LED_DATA_EN_GPIO_Port, LED_DATA_EN_Pin, GPIO_PIN_RESET);
}

