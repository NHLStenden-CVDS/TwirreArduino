#ifndef SENSOR_H_
#define SENSOR_H_

#include <memory>
#include "RequestHandler.h"

class Sensor;

struct SensorList
{
  Sensor** elements;
  uint8_t length;
  explicit SensorList(uint8_t len);
  ~SensorList();
};

enum ValueType
{
  I8,
  UI8,
  I16,
  UI16,
  I32,
  UI32,
  I64,
  UI64,
  F,
  D
}


class Sensor
{
public:
  struct Data
  {
    void* data = nullptr;
    uint16_t size = 0;
  };

  Sensor(char* name, char* description);
  Data GetValue(uint8_t valueID);
  char* GetOutputFormatString();
  virtual void UpdateSensor() = 0;

protected:
  void _AddValue(char* valueName, uint16_t* value);

private:
  struct Value
  {
    char* name;
    void* value = nullptr;
    ValueType type;
  };

  void _AddValueToValueList(char valueName, void* value, ValueType type);
  uint16_t _GetValueTypeSize(ValueType type);

  char* _sensorName;
  char* _sensorDescription;

  SensorValue* _valueList;
  uint8_t _valueListSize;
};

#endif
