#include "Sensor42.h"
#include <cstring>

uint32_t _value;
uint32_t* _valueArray;
uint16_t _valueArraySize;

Sensor42::Sensor42(char* name) : Sensor(name, "This sensor is used for testing purposes. It returns the parameter 'value' which is fixed to 42.")
{
  AddParam("value", &_value);
  addArrayParam("valueArray", _valueArray, &_valueArraySize);
}

void Sensor42::UpdateSensor()
{
  _value = 42;
}

