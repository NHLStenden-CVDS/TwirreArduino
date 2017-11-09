/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
