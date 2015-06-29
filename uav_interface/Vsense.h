#ifndef VSENSE_H
#define VSENSE_H

#include "Device.h"

class VSense: public Device
{
  public:
    VSense(int pin, double Vmin, double Vmax, char* name);
    virtual void OnRequest();
    
  private:
    int _pin;
    double _voltage;
    double _vmin;
    double _vmax;
    double _dV;
};

#endif
