#ifndef ACTUATOREXAMPLE_H_
#define ACTUATOREXAMPLE_H_

#include "Device.h"
#include <Stream.h>

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
