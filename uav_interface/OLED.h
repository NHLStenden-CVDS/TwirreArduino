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

#ifndef OLED_H_
#define OLED_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>


#define I2CADDRESS 0b0111101
#define PIN_RESET 45

#define BLACK 0
#define WHITE 1

#define LCDWIDTH      64
#define LCDHEIGHT     48
#define FONTHEADERSIZE    6

#define NORM        0
#define XOR         1

#define PAGE        0
#define ALL         1

#define WIDGETSTYLE0      0
#define WIDGETSTYLE1      1
#define WIDGETSTYLE2      2

#define SETCONTRAST     0x81
#define DISPLAYALLONRESUME  0xA4
#define DISPLAYALLON    0xA5
#define NORMALDISPLAY     0xA6
#define INVERTDISPLAY     0xA7
#define DISPLAYOFF      0xAE
#define DISPLAYON       0xAF
#define SETDISPLAYOFFSET  0xD3
#define SETCOMPINS      0xDA
#define SETVCOMDESELECT   0xDB
#define SETDISPLAYCLOCKDIV  0xD5
#define SETPRECHARGE    0xD9
#define SETMULTIPLEX    0xA8
#define SETLOWCOLUMN    0x00
#define SETHIGHCOLUMN     0x10
#define SETSTARTLINE    0x40
#define MEMORYMODE      0x20
#define COMSCANINC      0xC0
#define COMSCANDEC      0xC8
#define SEGREMAP      0xA0
#define CHARGEPUMP      0x8D
#define EXTERNALVCC     0x01
#define SWITCHCAPVCC    0x02




class OLED : public Device, public Print
{
  public:
    OLED(const char* name);
    void ValuesChanged();   
  private:  
    void command(uint8_t c);
    void data(uint8_t c);
    void display(void);

    void setPageAddress(uint8_t add);    
    void setColumnAddress(uint8_t add);    
    void clear(uint8_t mode);

    void setCursor(uint8_t x, uint8_t y);
    void pixel(uint8_t x, uint8_t y);
    void pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t mode);

    void setColor(uint8_t color);
    void setDrawMode(uint8_t mode);

    uint8_t _cursorX = 0, _cursorY = 0, _font = 0, _printchar = 0, _printflag = 0, _clearflag = 0;
    uint8_t* _printstring;
    uint32_t _printstringLength = 64;

    uint8_t setFontType(uint8_t type);
    size_t write(uint8_t c);
    void drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t mode);
    
    uint8_t foreColor,drawMode,fontWidth, fontHeight, fontType, fontStartChar, fontTotalChar, cursorX, cursorY;
    uint16_t fontMapWidth;
    static const unsigned char *fontsPointer[];
};



#endif

