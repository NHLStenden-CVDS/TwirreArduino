#ifndef STATUSLED_H_
#define STATUSLED_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>

#define LEDR 48
#define LEDG 47
#define LEDB 46

class StatusLED : public Device
{
  public:
    StatusLED(char* name);
    void ValuesChanged();
  private:
    int8_t _red = 0, _green = 0, _blue = 0;
};

#endif
