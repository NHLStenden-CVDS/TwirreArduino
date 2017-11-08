 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#ifndef VSENSE_H
#define VSENSE_H

#include "Device.h"

#define LIPOP4 A0
#define LIPOP3 A1
#define LIPOP2 A2
#define LIPOP1 A3

//TODO: CALIBRATE REF AND RATIO VALUES
#define REFVOLTAGE 3.3

#define CELL1RATIO 7.825
#define CELL2RATIO 7.825
#define CELL3RATIO 7.80
#define CELL4RATIO 7.80

#define RESOLUTION 4096.0

class VSense: public Device
{
  public:
    VSense(const char* name);
    virtual void OnRequest();
    
  private:
    double _voltage;

    double _raw1;
    double _raw2;
    double _raw3;
    double _raw4;
    
    double _vcell1;
    double _vcell2;
    double _vcell3;
    double _vcell4;
};

#endif
