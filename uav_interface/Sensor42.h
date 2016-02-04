#ifndef SENSOR42_H_
#define SENSOR42_H_

#include "Device.h"

class Sensor42 : public Device
{
  public:
    Sensor42(char* name);
    void OnRequest();

  private:
    uint32_t _value;
    uint8_t* _array;
    uint32_t _numberOfElements;
};

#endif
