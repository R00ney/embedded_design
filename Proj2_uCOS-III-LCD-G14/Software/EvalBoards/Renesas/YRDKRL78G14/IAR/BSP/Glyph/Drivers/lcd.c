//#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <includes.h>
#include "glyph_api.h"
#include "ST7579_LCD.h"
#include "lcd.h"
#include "ior5f104pj.h"
#include "r_cg_userdefine.h"

/*-------------------------------------------------------------------------*
 * Globals
 *-------------------------------------------------------------------------*/
static  T_glyphHandle  G_lcd;

uint8_t G_CharWidth = 5u;
uint8_t G_CharHeight = 7u;

void LCDInit (void)
{
    T_glyphError  err;

    err = GlyphOpen(&G_lcd, 0);
    if (err == GLYPH_ERROR_NONE) {
        GlyphNormalScreen(G_lcd);
        GlyphClearScreen(G_lcd);
		LCDFont(FONT_LARGE);
    }
}


void LCDClearScreen (void)
{
    GlyphClearScreen(G_lcd);
}


void LCDFont (LCDFONT font)
{
    switch (font) {
    case FONT_LOGOS:
		GlyphSetFont(G_lcd, GLYPH_FONT_LOGOS);
		G_CharWidth = 16u;
		G_CharHeight = 16u;
       	break;

    case FONT_LARGE:
       	GlyphSetFont(G_lcd, GLYPH_FONT_8_BY_8);
		G_CharWidth = 8u;
		G_CharHeight = 8u;
       	break;

    case FONT_SMALL:
       	GlyphSetFont(G_lcd, GLYPH_FONT_5_BY_7);
		G_CharWidth = 5u;
		G_CharHeight = 7u;
       	break;
    }
}

void LCDClearLine (uint8_t aLine)
{
    uint8_t y = aLine * G_CharHeight;
    GlyphEraseBlock(G_lcd, 0u, y, 95u, y + 7u);
}

void LCDString (const char *aStr, uint8_t aX, uint8_t aY)
{
    GlyphSetXY (G_lcd, aX, aY);
    GlyphString(G_lcd, (uint8_t *)aStr, strlen(aStr));
}

void LCDStringPosLine (const char *aStr, uint8_t aPos, uint8_t aLine)
{
    uint8_t  x = aPos  * G_CharWidth;
	uint8_t  y = aLine * G_CharHeight;

    LCDString(aStr, x, y);
}

void LCDChar(char aChar, uint8_t aX, uint8_t aY)
{
	GlyphSetXY (G_lcd, aX, aY);
    GlyphChar(G_lcd, aChar);
}

void LCDCharPosLine(char aChar, uint8_t aPos, uint8_t aLine)
{
    uint8_t  x;
    uint8_t  y;

    x = aPos  * G_CharWidth;
    y = aLine * G_CharHeight;

	LCDChar(aChar, x, y);
}

void  LCDPrintf(uint8_t aLine, uint8_t aPos, char *aFormat, ...)
{
    uint8_t  x, y;
    char buffer[100];
    va_list marker;

	x = aPos * G_CharWidth;
	y = aLine * G_CharHeight;

    // Convert to a string for output
    va_start(marker, aFormat);
    vsprintf(buffer, aFormat, marker);
    va_end(marker);

    LCDString(buffer, x, y);
}

void  LCDInvert()
{
	GlyphInvertScreen(G_lcd);
}
