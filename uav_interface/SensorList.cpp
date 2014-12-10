#include "SensorList.h"
#include <cstdlib>

SensorList::SensorList()
{
  _length = 0;
  _elements = nullptr;  
}

SensorList::~SensorList()
{
  if(_elements != nullptr)
  {
    free(_elements);
  }
}

void SensorList::AddSensor(Sensor* sensor)
{
  _length++;
  _elements = (Sensor**)realloc(_elements, _length * sizeof(Sensor*));
  _elements[_length - 1] = sensor;
}

void SensorList::UpdateAll()
{
  for(uint8_t i=0; i<_length; ++i)
  {
    _elements[i]->UpdateSensor();
  }
}

uint8_t SensorList::GetLength()
{
  return _length;
}

Sensor* SensorList::GetSensor(uint8_t id)
{
  if(id < _length)
  {
    return _elements[id];
  }
  else
  {
    return nullptr;
  }
}
