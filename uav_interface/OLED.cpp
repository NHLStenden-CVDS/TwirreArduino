/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Wire.h>

#include "OLED.h"
#include "Arduino.h"

#ifndef _BV    
#define _BV(bit) (1 << (bit))
#endif

#define TOTALFONTS    2
#include "font/font5x7.h"
#include "font/font8x16.h"
// Add the font name as declared in the header file.  Remove as many as possible to conserve FLASH memory.
const unsigned char *OLED::fontsPointer[] =
{ font5x7, font8x16 };

static uint8_t screenmemory[] =
{

		//SparkFun Electronics LOGO

		// ROW0, BYTE0 to BYTE63
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// ROW1, BYTE64 to BYTE127
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// ROW2, BYTE128 to BYTE191
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// ROW3, BYTE192 to BYTE255
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// ROW4, BYTE256 to BYTE319
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// ROW5, BYTE320 to BYTE383
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

OLED::OLED(const char* name) :
		Device(name, "OLED Display")
{
	//_printstring = reinterpret_cast<uint8_t*>(malloc(sizeof(uint8_t) * (_printstringLength + 1)));
	_AddVariable("cursorx", &_cursorX);
	_AddVariable("cursory", &_cursorY);
	_AddVariable("font", &_font);
	_AddVariable("printchar", &_printchar);
	_AddVariable("printflag", &_printflag);
	_AddVariable("clearflag", &_clearflag);
	//_AddVariable("printstring", _printstring, &_printstringLength);

	pinMode(PIN_RESET, OUTPUT);  // Set RST pin as OUTPUT
	digitalWrite(PIN_RESET, HIGH);  // Initially set RST HIGH
	delay(5);  // VDD (3.3V) goes high at start, lets just chill for 5 ms
	digitalWrite(PIN_RESET, LOW);  // Bring RST low, reset the display
	delay(10);  // wait 10ms
	digitalWrite(PIN_RESET, HIGH);  // Set RST HIGH, bring out of reset

	setColor(WHITE);
	setDrawMode(NORM);

	// Display Init sequence for 64x48 OLED module
	command(DISPLAYOFF);      // 0xAE

	command(SETDISPLAYCLOCKDIV);  // 0xD5
	command(0x80);          // the suggested ratio 0x80

	command(SETMULTIPLEX);      // 0xA8
	command(0x2F);

	command(SETDISPLAYOFFSET);    // 0xD3
	command(0x0);         // no offset

	command(SETSTARTLINE | 0x0);  // line #0

	command(CHARGEPUMP);      // enable charge pump
	command(0x14);

	command(NORMALDISPLAY);     // 0xA6
	command(DISPLAYALLONRESUME);  // 0xA4

	command(SEGREMAP | 0x1);
	command(COMSCANDEC);

	command(SETCOMPINS);      // 0xDA
	command(0x12);

	command(SETCONTRAST);     // 0x81
	command(0x8F);

	command(SETPRECHARGE);      // 0xd9
	command(0xF1);

	command(SETVCOMDESELECT);     // 0xDB
	command(0x40);

	command(DISPLAYON);       //--turn on oled panel
	clear(ALL);           // Erase hardware memory inside the OLED controller to avoid random data in memory.

	/*
	 for(int i = 0; i < 40 ; i++){
	 pixel(i,i);
	 pixel(40-i,i);
	 }
	 */
	setCursor(1, 2);
	setFontType(1);

	print("Twirre V2");

	display();
}

void OLED::ValuesChanged()
{
	//setFontType(0);
	//setCursor(0,0);
	//print(String("") + _printstringLength);
	//display();

	if (_clearflag)
	{
		clear(PAGE);
		_clearflag = 0;
	}

	if (_printflag)
	{
		//_printstring[_printstringLength] = 0;
		setFontType(_font);

		if (_cursorX < 255 && _cursorY < 255)
		{
			setCursor(_cursorX, _cursorY);
			_cursorX = 255;
			_cursorY = 255;
		}

		write(_printchar);
		//print(reinterpret_cast<const char*>(_printstring));
		_printflag = 0;
		display();
	}
}

void OLED::command(uint8_t c)
{
	Wire1.beginTransmission(I2CADDRESS);
	Wire1.write(0x00);
	Wire1.write(c);
	Wire1.endTransmission();
}

void OLED::data(uint8_t c)
{
	Wire1.beginTransmission(I2CADDRESS);
	Wire1.write(0x40);
	Wire1.write(c);
	Wire1.endTransmission();
}

void OLED::setPageAddress(uint8_t add)
{
	add = 0xb0 | add;
	command(add);
	return;
}

void OLED::setColumnAddress(uint8_t add)
{
	command((0x10 | (add >> 4)) + 0x02);
	command((0x0f & add));
	return;
}

void OLED::clear(uint8_t mode)
{
	//  uint8_t page=6, col=0x40;
	if (mode == ALL)
	{
		for (int i = 0; i < 8; i++)
		{
			setPageAddress(i);
			setColumnAddress(0);
			for (int j = 0; j < 0x80; j++)
			{
				data(0);
			}
		}
	}
	else
	{
		memset(screenmemory, 0, 384);     // (64 x 48) / 8 = 384
		//display();
	}
}

void OLED::setCursor(uint8_t x, uint8_t y)
{
	cursorX = x;
	cursorY = y;
}

void OLED::pixel(uint8_t x, uint8_t y)
{
	pixel(x, y, foreColor, drawMode);
}

void OLED::pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t mode)
{
	if (/*(x < 0) ||*/ (x >= LCDWIDTH) /* || (y < 0)*/ || (y >= LCDHEIGHT)) return;

	if (mode == XOR)
	{
		if (color == WHITE) screenmemory[x + (y / 8) * LCDWIDTH] ^= _BV((y & 0x7));
	}
	else
	{
		if (color == WHITE)
			screenmemory[x + (y / 8) * LCDWIDTH] |= _BV((y & 0x7));
		else
			screenmemory[x + (y / 8) * LCDWIDTH] &= ~_BV((y & 0x7));
	}
}

void OLED::display(void)
{
	uint8_t i, j;

	for (i = 0; i < 6; i++)
	{
		setPageAddress(i);
		setColumnAddress(0);
		for (j = 0; j < 0x40; j++)
		{
			data(screenmemory[i * 0x40 + j]);
		}
	}
}

void OLED::setColor(uint8_t color)
{
	foreColor = color;
}

/** \brief Set draw mode.
 Set current draw mode with NORM or XOR.
 */
void OLED::setDrawMode(uint8_t mode)
{
	drawMode = mode;
}

uint8_t OLED::setFontType(uint8_t type)
{
	if ((type >= TOTALFONTS) /*|| (type < 0)*/) return false;

	fontType = type;
	fontWidth = pgm_read_byte(fontsPointer[fontType] + 0);
	fontHeight = pgm_read_byte(fontsPointer[fontType] + 1);
	fontStartChar = pgm_read_byte(fontsPointer[fontType] + 2);
	fontTotalChar = pgm_read_byte(fontsPointer[fontType] + 3);
	fontMapWidth = (pgm_read_byte(fontsPointer[fontType]+4) * 100) + pgm_read_byte(fontsPointer[fontType] + 5);  // two bytes values into integer 16
	return true;
}

size_t OLED::write(uint8_t c)
{
	if (c == '\n')
	{
		cursorY += fontHeight;
		cursorX = 0;
	}
	else if (c == '\r')
	{
		// skip
	}
	else
	{
		drawChar(cursorX, cursorY, c, foreColor, drawMode);
		cursorX += fontWidth + 1;
		if ((cursorX > (LCDWIDTH - fontWidth)))
		{
			cursorY += fontHeight;
			cursorX = 0;
		}
	}

	return 1;
}

void OLED::drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t mode)
{
	// TODO - New routine to take font of any height, at the moment limited to font height in multiple of 8 pixels

	uint8_t rowsToDraw, row, tempC;
	uint8_t i, j, temp;
	uint16_t charPerBitmapRow, charColPositionOnBitmap, charRowPositionOnBitmap, charBitmapStartPosition;

	if ((c < fontStartChar) || (c > (fontStartChar + fontTotalChar - 1)))   // no bitmap for the required c
		return;

	tempC = c - fontStartChar;

	// each row (in datasheet is call page) is 8 bits high, 16 bit high character will have 2 rows to be drawn
	rowsToDraw = fontHeight / 8;  // 8 is LCD's page size, see SSD1306 datasheet
	if (rowsToDraw <= 1) rowsToDraw = 1;

	// the following draw function can draw anywhere on the screen, but SLOW pixel by pixel draw
	if (rowsToDraw == 1)
	{
		for (i = 0; i < fontWidth + 1; i++)
		{
			if (i == fontWidth)  // this is done in a weird way because for 5x7 font, there is no margin, this code add a margin after col 5
				temp = 0;
			else
				temp = pgm_read_byte(fontsPointer[fontType]+FONTHEADERSIZE+(tempC*fontWidth)+i);

			for (j = 0; j < 8; j++)
			{     // 8 is the LCD's page height (see datasheet for explanation)
				if (temp & 0x1)
				{
					pixel(x + i, y + j, color, mode);
				}
				else
				{
					pixel(x + i, y + j, !color, mode);
				}

				temp >>= 1;
			}
		}
		return;
	}

	// font height over 8 bit
	// take character "0" ASCII 48 as example
	charPerBitmapRow = fontMapWidth / fontWidth;  // 256/8 =32 char per row
	charColPositionOnBitmap = tempC % charPerBitmapRow;  // =16
	charRowPositionOnBitmap = int(tempC / charPerBitmapRow);  // =1
	charBitmapStartPosition = (charRowPositionOnBitmap * fontMapWidth * (fontHeight / 8)) + (charColPositionOnBitmap * fontWidth);

	// each row on LCD is 8 bit height (see datasheet for explanation)
	for (row = 0; row < rowsToDraw; row++)
	{
		for (i = 0; i < fontWidth; i++)
		{
			temp = pgm_read_byte(fontsPointer[fontType]+FONTHEADERSIZE+(charBitmapStartPosition+i+(row*fontMapWidth)));
			for (j = 0; j < 8; j++)
			{     // 8 is the LCD's page height (see datasheet for explanation)
				if (temp & 0x1)
				{
					pixel(x + i, y + j + (row * 8), color, mode);
				}
				else
				{
					pixel(x + i, y + j + (row * 8), !color, mode);
				}
				temp >>= 1;
			}
		}
	}

}
