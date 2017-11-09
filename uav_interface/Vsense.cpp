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

#include "Vsense.h"
#include <Arduino.h>

VSense::VSense(const char* name) : Device(name, "Voltage sensor")
{
  //setup stuff
  _AddVariable("voltage", &_voltage);
  
  _AddVariable("raw1", &_raw1);
  _AddVariable("raw2", &_raw2);
  _AddVariable("raw3", &_raw3);
  _AddVariable("raw4", &_raw4);
  
  _AddVariable("vcell1", &_vcell1);
  _AddVariable("vcell2", &_vcell2);
  _AddVariable("vcell3", &_vcell3);
  _AddVariable("vcell4", &_vcell4);
  analogReadResolution(12);
}

void VSense::OnRequest()
{
  uint16_t m1 = analogRead(LIPOP1);
  uint16_t m2 = analogRead(LIPOP2);
  uint16_t m3 = analogRead(LIPOP3);
  uint16_t m4 = analogRead(LIPOP4);

  _raw1 = ((m1/RESOLUTION)) * REFVOLTAGE * CELL1RATIO;
  _raw2 = ((m2/RESOLUTION)) * REFVOLTAGE * CELL2RATIO;
  _raw3 = ((m3/RESOLUTION)) * REFVOLTAGE * CELL3RATIO;
  _raw4 = ((m4/RESOLUTION)) * REFVOLTAGE * CELL4RATIO;
  _voltage = _raw1;

  _vcell1=_raw1-_raw2;
  _vcell2=_raw2-_raw3;
  _vcell3=_raw3-_raw4;
  _vcell4=_raw4;
}
