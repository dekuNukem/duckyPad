#ifndef ssd1306
#define ssd1306

#include "stm32f0xx_hal.h"
#include "fonts.h"

#define X_OFFSET 2

// i2c port naam in main programma gegenereerd door cube
#define SSD1306_I2C_PORT		hi2c1
// I2C address 
#define SSD1306_I2C_ADDR        0x78
// SSD1306 width in pixels
#define SSD1306_WIDTH           (128 + X_OFFSET)
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT          64


//
//	Enum voor de kleuren van het scherm Black en White
//
typedef enum {
	Black = 0x00, /*!< Black color, no pixel */
	White = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR;

//
//	Struct om wijzigingen bij te houden
//
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

extern I2C_HandleTypeDef SSD1306_I2C_PORT;

uint8_t ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_dim(uint8_t is_dim);

static void ssd1306_WriteCommand(uint8_t command);
void oled_reset(void);

#endif
