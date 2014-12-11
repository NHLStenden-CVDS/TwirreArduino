#ifndef DEVICELIST_H_
#define DEVICELIST_H_

#include "Device.h"

class DeviceList
{
public:
  DeviceList();
  ~DeviceList();
  
  void Add(Device* device);
  uint8_t GetLength();
  Device* Get(uint8_t id);
  void UpdateAll();
  
private:
  uint8_t _length;
  Device** _elements;
};

#endif
