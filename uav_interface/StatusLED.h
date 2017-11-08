 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#ifndef STATUSLED_H_
#define STATUSLED_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>

#define LEDR 48
#define LEDG 46
#define LEDB 47

class StatusLED : public Device
{
  public:
    StatusLED(const char* name);
    void ValuesChanged();
    void SetValue(int8_t _red,int8_t green, int8_t blue);
  private:
    int8_t _red = 0, _green = 0, _blue = 0;
};

#endif
