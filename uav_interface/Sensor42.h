#ifndef SENSOR42_H_
#define SENSOR42_H_

#include "Sensor.h"

class Sensor42 : public Sensor
{
public:
    Sensor42(char* name);
    SensorData Output(uint8_t payloadSize, char* payload);
    void InitializeSensor();
    void UpdateSensor();
    
private:
    uint32_t _value;
};

#endif
