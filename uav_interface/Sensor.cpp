#include "Sensor.h"

Sensor::Sensor(char* name)
{
  _sensorName = name;

  _valueListSize = 0;
  _auxValueList = nullptr;
}

SensorList::SensorList(uint8_t len)
{
  length = len;
  elements = new Sensor*[length];
}

SensorList::~SensorList()
{
  delete elements;
}

SensorData Sensor::ProcessParameterID(uint8_t parameterID)
{
  SensorData sensorData;
  switch (parameterID)
  {
    case 0:
      sensorData = parameterToSensorData(&_value, sizeof(value));
      break;
  }
  return sensorData;
}

updateParam(void* value, )
(char)

SensorData Sensor::parameterToSensorData(void* value, uint8_t sizeOfData)
{
  SensorData sensorData;
  sensorData.size = sizeOfData;
  char* data = new char[sensorData.size];
  memcpy(data, value, sensorData.size);
  sensorData.data = std::unique_ptr<char>(data);
  return sensorData;
}


void Sensor::_AddValueToValueList(void* value, uint16_t valueSize);
{
  //make the list longer allocating memory for the new SensorValue
  _valueListSize++;
  SensorValue* _valueList = (SensorValue*)realloc(_valueList, sizeof(SensorValue));
  _valueList[_valueListSize - 1].value = value;
  _valueList[_valueListSize - 1].size = valueSize;
}

void Sensor::_AddValueToOutputFormat(char* valueName, ValueType valueType)
{
  //make buffer string longer? -> realloc
  strcpy();

  char* typeString = (char*)malloc(sizeof(char)*10); //space for 9 chars + NULL terminator
  //put the correct type string depending on the ValueType enum.
  switch (valueType)
  {
    case ValueType::I8:
      typeString = "I8";
      break;
    case ValueType::UI8:
      typeString = "UI8";
      break;
    case ValueType::I16:
      typeString = "I16";
      break;
    case ValueType::UI16:
      typeString = "UI16";
      break;
    case ValueType::I32:
      typeString = "I32";
      break;
    case ValueType::UI32:
      typeString = "UI32";
      break;
    case ValueType::I64:
      typeString = "I64";
      break;
    case ValueType::UI64:
      typeString = "UI64";
      break;
    case ValueType::F:
      typeString = "F";
      break;
    case ValueType::D:
      typeString = "D";
      break;
  }

}

void Sensor::_AddValue(char* valueName, uint16_t* value);
{
  _AddValueToValueList((void*) value, 2);
  //add name and type (depending on every addParam) to the outputformat string
  //add name and A:type (depending on every addParam) to the outputformat string IF ISARRAY IS SET
}


_AddArrayParam("valueArray", _valueArray, &_valueArraySize)
{
  // bool isArray = false
  //store pointer in a list
  //add name and A:type (depending on every addParam) to the outputformat string
}
