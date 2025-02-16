#include <stdlib.h>
#include "ssd1306.h"
#include "main.h"
#include "stdint.h"

#define SSD1306_LCDWIDTH 128
#define SSD1306_LCDHEIGHT 64
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
#define SSD1306_DEACTIVATE_SCROLL 0x2E

static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static SSD1306_t SSD1306;

uint8_t i2c_status;

static void ssd1306_WriteCommand(uint8_t command)
{
	if(i2c_status == HAL_OK)
		HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x00,1,&command,1,1);
}

uint8_t ssd1306_Init(void)
{	
	i2c_status = HAL_I2C_IsDeviceReady(&hi2c1, SSD1306_I2C_ADDR, 1, 100);
	/* Init LCD */
	ssd1306_WriteCommand(SSD1306_DISPLAYOFF);                    // 0xAE
	ssd1306_WriteCommand(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	ssd1306_WriteCommand(0x80);                                  // the suggested ratio 0x80
	ssd1306_WriteCommand(SSD1306_SETMULTIPLEX);                  // 0xA8
	ssd1306_WriteCommand(SSD1306_LCDHEIGHT - 1);
	ssd1306_WriteCommand(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	ssd1306_WriteCommand(0x0);                                   // no offset
	ssd1306_WriteCommand(SSD1306_SETSTARTLINE | 0x0);            // line #0
	ssd1306_WriteCommand(SSD1306_CHARGEPUMP);                    // 0x8D
	HAL_Delay(150);
	ssd1306_WriteCommand(0x14);
	ssd1306_WriteCommand(SSD1306_MEMORYMODE);                    // 0x20
	ssd1306_WriteCommand(0x00);                                  // 0x0 act like ks0108
	ssd1306_WriteCommand(SSD1306_SEGREMAP | 0x1);
	ssd1306_WriteCommand(SSD1306_COMSCANDEC);
	ssd1306_WriteCommand(SSD1306_SETCOMPINS);                    // 0xDA
	ssd1306_WriteCommand(0x12);
	ssd1306_WriteCommand(SSD1306_SETCONTRAST);                   // 0x81
	ssd1306_WriteCommand(0x70); // CF 70
	ssd1306_WriteCommand(SSD1306_SETPRECHARGE);                  // 0xd9
	ssd1306_WriteCommand(0x22); // F1, try 22?
	ssd1306_WriteCommand(SSD1306_SETVCOMDETECT);                 // 0xDB
	ssd1306_WriteCommand(0x40);
	ssd1306_WriteCommand(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	ssd1306_WriteCommand(SSD1306_NORMALDISPLAY);                 // 0xA6
	ssd1306_WriteCommand(SSD1306_DEACTIVATE_SCROLL);
	ssd1306_WriteCommand(SSD1306_DISPLAYON);//--turn on oled panel
	HAL_Delay(10);

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

void ssd1306_Fill(SSD1306_COLOR color) 
{
	for(uint32_t i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

void ssd1306_UpdateScreen(void) 
{
	// 29ms total at 400KHz I2C clock
	for (uint8_t i = 0; i < 8; i++) {
		ssd1306_WriteCommand(0xB0 + i);
		ssd1306_WriteCommand(0x00);
		ssd1306_WriteCommand(0x10);
		if(i2c_status == HAL_OK)
			HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x40,1,&SSD1306_Buffer[SSD1306_WIDTH * i],SSD1306_WIDTH,100);
	}
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	x += X_OFFSET;
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
		return;
	
	if (SSD1306.Inverted) 
		color = (SSD1306_COLOR)!color;

	
	if (color == White)
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	else 
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
}

char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;
	
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
	{
		return 0;
	}
	
	for (i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		b = b << 8;
		for (j = 0; j < Font.FontWidth; j++)
		{
			if ((b << j) & 0x8000) 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			} 
			else 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
			}
		}
	}
	
	SSD1306.CurrentX += Font.FontWidth;
	
	return ch;
}


char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
	while (*str) 
	{
		if (ssd1306_WriteChar(*str, Font, color) != *str)
		{
			return *str;
		}
		
		str++;
	}
	return *str;
}

void ssd1306_SetCursor(uint8_t x, uint8_t y) 
{
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

void oled_reset(void)
{
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(20);
}

/* Draw line by Bresenhem's algorithm */
void ssd1306_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
{
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;
    
    ssd1306_DrawPixel(x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(x1, y1, color);
        error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        
        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
    return;
}

uint8_t current_contrast;
void ssd1306_SetContrast(const uint8_t value)
{
    if(value == current_contrast)
        return;
    ssd1306_WriteCommand(SSD1306_SETCONTRAST);
    ssd1306_WriteCommand(value);
    current_contrast = value;
}

/* Draw circle by Bresenhem's algorithm */
void ssd1306_DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT) {
        return;
    }

    do {
        ssd1306_DrawPixel(par_x - x, par_y + y, par_color);
        ssd1306_DrawPixel(par_x + x, par_y + y, par_color);
        ssd1306_DrawPixel(par_x + x, par_y - y, par_color);
        ssd1306_DrawPixel(par_x - x, par_y - y, par_color);
        e2 = err;

        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

/* Draw filled circle. Pixel positions calculated using Bresenham's algorithm */
void ssd1306_FillCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT) {
        return;
    }

    do {
        for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--) {
            for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--) {
                ssd1306_DrawPixel(_x, _y, par_color);
            }
        }

        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if (-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

/* Draw a rectangle */
void ssd1306_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    ssd1306_Line(x1,y1,x2,y1,color);
    ssd1306_Line(x2,y1,x2,y2,color);
    ssd1306_Line(x2,y2,x1,y2,color);
    ssd1306_Line(x1,y2,x1,y1,color);
    return;
}

/* Draw a filled rectangle */
void ssd1306_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    uint8_t x_start = ((x1<=x2) ? x1 : x2);
    uint8_t x_end   = ((x1<=x2) ? x2 : x1);
    uint8_t y_start = ((y1<=y2) ? y1 : y2);
    uint8_t y_end   = ((y1<=y2) ? y2 : y1);

    for (uint8_t y= y_start; (y<= y_end)&&(y<SSD1306_HEIGHT); y++) {
        for (uint8_t x= x_start; (x<= x_end)&&(x<SSD1306_WIDTH); x++) {
            ssd1306_DrawPixel(x, y, color);
        }
    }
    return;
}
