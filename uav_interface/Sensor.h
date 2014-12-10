#ifndef SENSOR_H_
#define SENSOR_H_

#include <cstdint>

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
};

struct SensorValueSize
{
  uint16_t elementSize = 0;
  uint16_t numberOfElements = 0;
  bool isArray = false;
};

struct SensorValue
{
  void* value = nullptr;
  SensorValueSize valueSize;
};

class Sensor
{
public:
  Sensor(char* name, char* description);
  virtual ~Sensor();
  SensorValue GetValue(uint8_t valueID);
  SensorValueSize GetValueSize(uint8_t valueID);
  uint8_t GetNumberOfValues();
  char* GetSensorName();
  char* GetSensorDescription();
  char* GetOutputFormatString();
  virtual void UpdateSensor() = 0;

protected:
  void _AddValue(char* valueName, int8_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, uint8_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, int16_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, uint16_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, int32_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, uint32_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, int64_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, uint64_t* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, float* value, uint16_t* arraySize=nullptr);
  void _AddValue(char* valueName, double* value, uint16_t* arraySize=nullptr);
  
private:
  struct Value
  {
    void* value = nullptr;
    uint16_t* arraySize = nullptr;
    char* name;
    ValueType type;
  };

  void _AddValueToValueList(char* valueName, void* value, ValueType type, uint16_t* arraySize=nullptr);
  uint16_t _GetValueTypeSize(ValueType type);

  char* _sensorName;
  char* _sensorDescription;

  Value* _valueList;
  uint8_t _valueListSize;
};

#endif
