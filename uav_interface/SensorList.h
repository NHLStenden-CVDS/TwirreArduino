#ifndef SENSORLIST_H_
#define SENSORLIST_H_

#include "Sensor.h"

class SensorList
{
public:
  SensorList();
  ~SensorList();
  
  void AddSensor(Sensor* sensor);
  uint8_t GetLength();
  Sensor* GetSensor(uint8_t id);
  
private:
  uint8_t _length;
  Sensor** _elements;
};


#endif
