 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#ifndef ACTUATOREXAMPLE_H_
#define ACTUATOREXAMPLE_H_

#include <Stream.h>

#include "Device.h"

class ActuatorExample : public Device
{
  public:
    ActuatorExample(char* name, Stream* stream);
    void Update();

  private:
    uint32_t _value;
    uint32_t _oldValue;
    Stream* _stream;
};

#endif
