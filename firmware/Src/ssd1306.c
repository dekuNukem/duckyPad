#include "ssd1306.h"


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
// Databuffer voor het scherm
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Een scherm-object om lokaal in te werken
static SSD1306_t SSD1306;

uint8_t i2c_status;

//
//	Een byte sturen naar het commando register
//	Kan niet gebruikt worden buiten deze file
//
static void ssd1306_WriteCommand(uint8_t command)
{
	if(i2c_status == HAL_OK)
		HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x00,1,&command,1,1);
}

void ssd1306_dim(uint8_t is_dim)
{
	if(is_dim)
	{
		ssd1306_WriteCommand(SSD1306_SETCONTRAST);
		ssd1306_WriteCommand(0x0);
	}
	else
	{
		ssd1306_WriteCommand(SSD1306_SETCONTRAST);
		ssd1306_WriteCommand(0x70);
	}
	
}
//
//	Het scherm initialiseren voor gebruik
//
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

//
//	We zetten de hele buffer op een bepaalde kleur
// 	color 	=> de kleur waarin alles moet
//
void ssd1306_Fill(SSD1306_COLOR color) 
{
	/* Set memory */
	uint32_t i;

	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

//
//	Alle weizigingen in de buffer naar het scherm sturen
//
void ssd1306_UpdateScreen(void) 
{
	uint8_t i;
	
	for (i = 0; i < 8; i++) {
		ssd1306_WriteCommand(0xB0 + i);
		ssd1306_WriteCommand(0x00);
		ssd1306_WriteCommand(0x10);

		// We schrijven alles map per map weg
		if(i2c_status == HAL_OK)
			HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x40,1,&SSD1306_Buffer[SSD1306_WIDTH * i],SSD1306_WIDTH,100);
	}
}

//
//	1 pixel op het scherm tekenen
//	X => X coordinaat
//	Y => Y coordinaat
//	color => kleur die pixel moet krijgen
//
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	x += X_OFFSET;
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
	{
		// We gaan niet buiten het scherm schrijven
		return;
	}
	
	// Kijken of de pixel geinverteerd moet worden
	if (SSD1306.Inverted) 
	{
		color = (SSD1306_COLOR)!color;
	}
	
	// We zetten de juiste kleur voor de pixel
	if (color == White)
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} 
	else 
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

//
//	We willen 1 char naar het scherm sturen
//	ch 		=> char om weg te schrijven
//	Font 	=> Font waarmee we gaan schrijven
//	color 	=> Black or White
//
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;
	
	// Kijken of er nog plaats is op deze lijn
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
	{
		// Er is geen plaats meer
		return 0;
	}
	
	// We gaan door het font
	for (i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
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
	
	// De huidige positie is nu verplaatst
	SSD1306.CurrentX += Font.FontWidth;
	
	// We geven het geschreven char terug voor validatie
	return ch;
}

//
//	Functie voor het wegschrijven van een hele string
// 	str => string om op het scherm te schrijven
//	Font => Het font dat gebruikt moet worden
//	color => Black or White
//
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
	// We schrijven alle char tot een nulbyte
	while (*str) 
	{
		if (ssd1306_WriteChar(*str, Font, color) != *str)
		{
			// Het karakter is niet juist weggeschreven
			return *str;
		}
		
		// Volgende char
		str++;
	}
	
	// Alles gelukt, we sturen dus 0 terug
	return *str;
}

//
//	Zet de cursor op een coordinaat
//
void ssd1306_SetCursor(uint8_t x, uint8_t y) 
{
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}
