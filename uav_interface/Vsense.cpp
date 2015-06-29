#include "VSense.h"
#include <arduino.h>

VSense::VSense(int pin, double Vmin, double Vmax, char* name) : Device(name, "Voltage sensor"), _pin(pin), _voltage(0), _vmin(Vmin), _vmax(Vmax), _dV(Vmax - Vmin)
{
  //setup stuff
  _AddVariable("voltage", &_voltage);
  _AddVariable("vmin", &_vmin);
  _AddVariable("vmax", &_vmax);
  analogReadResolution(12);
}

void VSense::OnRequest()
{
  double v = static_cast<double>(analogRead(_pin));
  _voltage = _vmin + v * (_dV / 4095.0);
}
