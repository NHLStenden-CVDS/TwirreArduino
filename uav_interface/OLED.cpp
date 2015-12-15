#include "OLED.h"
#include "Arduino.h"

/*
OLED* OLED::_instance = nullptr;


OLED::OLED(char* name) : Device(name, "OLED Display")
{
  _AddVariable("cursorX", &_cursorX);
  _AddVariable("cursorY", &_cursorY);
  _AddVariable("drawString", (int8_t*)_drawString, &_drawStringSize);

  oled = new MicroOLED(PIN_RESET, DC_JUMPER);
  
  oled->begin();     // Initialize the OLED
  oled->clear(PAGE); // Clear the display's internal memory
  oled->clear(ALL);  // Clear the library's display buffer
  oled->print(1337);
  oled->display();   // Display what's in the buffer (splashscreen)

  //delay(100);
  //oled->setCursor(0, 0);
  //oled->print("Hello world");
 // oled->display();
}


OLED* OLED::Initialize(char* name)
{
  if(_instance == nullptr)
  {
    _instance = new OLED(name);
  }
  return _instance;    
}

OLED* OLED::Instance()
{
    return _instance;
}


void OLED::ValuesChanged()
{/*
  if(_drawStringSize>0)
  {
      oled.setCursor(_cursorX, _cursorY);
      _drawString[_drawStringSize] = 0;
      oled.print((const char*)_drawString);
      oled.display();
      _drawStringSize = 0;
  }*/
//}

