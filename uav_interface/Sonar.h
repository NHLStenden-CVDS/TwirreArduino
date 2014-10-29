#ifndef SONAR_H_
#define SONAR_H_

#include "IntSensor.h"

class Sonar: public IntSensor {
public:
    void Init();
    int GetValue();
};

#endif
