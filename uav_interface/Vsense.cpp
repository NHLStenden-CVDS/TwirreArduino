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
