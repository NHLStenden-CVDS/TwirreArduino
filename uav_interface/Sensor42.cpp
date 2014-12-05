#include "Sensor42.h"
#include <cstring>
#include <cstdlib>

uint32_t _value;
uint32_t* _valueArray;
uint16_t _valueArraySize;

Sensor42::Sensor42(char* name) : Sensor(name, "This sensor is used for testing purposes. It returns the parameter 'value' which is fixed to 42.")
{
  _AddValue("value", &_value);
  _array = (uint8_t*)malloc(sizeof(uint8_t));
  _numberOfElements = 1;
  *_array = 'A';
  _AddValue("array", _array, &_numberOfElements);
}

void Sensor42::UpdateSensor()
{
  _value = 42;
  _numberOfElements = rand() % 5 + 1; //from 1 to 5 elements
  _array = (uint8_t*)realloc(_array, sizeof(uint8_t)); //change array to new size
  for(uint16_t i = 0; i<_numberOfElements; ++i)
  {
    _array[i]= rand() % 26 + 65; //random capital letter
  }
}

