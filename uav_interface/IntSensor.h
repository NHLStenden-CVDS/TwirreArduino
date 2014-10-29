#ifndef INTSENSOR_H_
#define INTSENSOR_H_

#include "Sensor.h"

class IntSensor : public Sensor {
public:
    void GetData();
    virtual void Init() = 0;
    void Update();

    virtual int GetValue() = 0;

//private:
    int _value;

};

#endif
