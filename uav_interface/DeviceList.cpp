 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#include "DeviceList.h"
#include <cstdlib>

DeviceList::DeviceList()
{
  _length = 0;
  _elements = nullptr;
}

DeviceList::~DeviceList()
{
  if (_elements != nullptr)
  {
    free(_elements);
  }
}

void DeviceList::Add(Device* device)
{
  _length++;
  _elements = (Device**)realloc(_elements, _length * sizeof(Device*));
  _elements[_length - 1] = device;
}

void DeviceList::UpdateAll()
{
  for (uint8_t i = 0; i < _length; ++i)
  {
    _elements[i]->Update();
  }
}

uint8_t DeviceList::GetLength()
{
  return _length;
}

Device* DeviceList::Get(uint8_t id)
{
  if (id < _length)
  {
    return _elements[id];
  }
  else
  {
    return nullptr;
  }
}
