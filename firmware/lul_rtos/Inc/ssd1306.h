
#include "stm32f0xx_hal.h"
#include "fonts.h"

//	Deze library is door Olivier Van den Eede 2016 geschreven en aangepast voor gebruik met
//	Stm32 microcontrollers en maakt gebruik van de HAL-i2c library's.
//
//	Deze library is gemaakt om gebruik te kunnen maken van een ssd1306 gestuurd oled display.
//	Voor het gebruik moeten zeker de onderstaande defines juist ingesteld worden.
//	Zoals de gebruikte i2c poort en de groote van het scherm.
//
//	De library maakt gebruik van 2 files (fonts.c/h) Hierin staan 3 fonts beschreven.
//	Deze fonts kunnen zo gebruikt worden: 	- Font_7x10
//											- Font_11x18
//											- Font_16x26

#ifndef ssd1306
#define ssd1306

#define X_OFFSET 0

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

//	De i2c poort staat in de main
extern I2C_HandleTypeDef SSD1306_I2C_PORT;

//
//	De functies definities van de functies die gebruikt kunnen worden
//
uint8_t ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);

static void ssd1306_WriteCommand(uint8_t command);

#endif
