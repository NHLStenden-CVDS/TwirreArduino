#ifndef SENSOR42_H_
#define SENSOR42_H_

#include "Sensor.h"

class Sensor42 : public Sensor
{
public:
    Sensor42(char* name);
    void UpdateSensor();
    
private:
    uint32_t _value;
    uint8_t* _array;
    uint16_t _numberOfElements;
};

#endif
