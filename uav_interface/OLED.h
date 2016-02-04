/*
#ifndef OLED_H_
#define OLED_H_

#include "Device.h"
#include "Arduino.h"
#include <SFE_MicroOLED.h>
#undef swap
#include <inttypes.h>

#define PIN_RESET 45
#define DC_JUMPER 1

class OLED : public Device
{
  public:
    OLED(char* name);
    void Update();
    void ValuesChanged();

    static OLED* Instance();
    static OLED* Initialize(char* name);
  private:
    static OLED* _instance;
  
    MicroOLED * oled; 
    byte _cursorX = 0, _cursorY = 0;
    int8_t _drawString[50];  
    uint16_t _drawStringSize;
};



#endif
*/
