#include "Sensor.h"
#include <cstring>
#include <cstdlib>

SensorList::SensorList(uint8_t len)
{
  length = len;
  elements = new Sensor*[length];
  //TODO: change to vector, or make it in a way that length changes when adding an element
}

SensorList::~SensorList()
{
  delete elements;
}

Sensor::Sensor(char* name, char* description)
{
  _sensorName = (char*)malloc(strlen(name)+1);//+1 for the NULL terminator
  strcpy(_sensorName, name);
  _sensorDescription = (char*)malloc(strlen(description)+1);//+1 for the NULL terminator
  strcpy(_sensorDescription, description);

  _valueListSize = 0;
  _valueList = nullptr;
}

Sensor::~Sensor()
{
  free(_sensorName);
  free(_sensorDescription);
}

uint8_t Sensor::GetNumberOfValues()
{
  return _valueListSize;
}

Sensor::Data Sensor::GetValue(uint8_t valueID)
{
  Sensor::Data sensorData;
  if(valueID < _valueListSize)
  {
    sensorData.data = _valueList[valueID].value;
    uint16_t nrOfElements = 1; //1 in case it is not an array, which we check next
    if(_valueList[valueID].arraySize != nullptr)
    {
      nrOfElements = *(_valueList[valueID].arraySize);
    }
    sensorData.size = _GetValueTypeSize(_valueList[valueID].type) * nrOfElements;
  }
  else
  {
    sensorData.data = nullptr;
    sensorData.size = 0;
  }
  return sensorData;
}

char* Sensor::GetSensorName()
{
  return _sensorName;
}

char* Sensor::GetSensorDescription()
{
  return _sensorDescription;
}

char* Sensor::GetOutputFormatString()
{
  char* outputFormat;
  uint16_t outputFormatLength = 0; 
  for(uint16_t i=0; i < _valueListSize; ++i)
  {
    outputFormatLength += strlen(_valueList[i].name) + 10; //+10 for the '=type,' it can result to 'name=A:UI64,'
  }
  
  if(outputFormatLength > 0)
  {
    //create string for the outputformat. Its size depends on the number of values that the sensor has
    outputFormat = (char*)malloc(outputFormatLength + 1); //+1 for the null terminator
    outputFormat[0] = NULL; //We set it as an empty string so we will be able to use strcat
    
    //for every value we will have a type which will be of maximum 4 chars
    char* typeString = (char*)malloc(5); //space for 4 chars + NULL terminator
    for(uint16_t i=0; i < _valueListSize; ++i)
    {
      switch (_valueList[i].type)
      {
        case ValueType::I8:
          strcpy(typeString, "I8");
          break;
        case ValueType::UI8:
          strcpy(typeString, "UI8");
          break;
        case ValueType::I16:
          strcpy(typeString, "I16");
          break;
        case ValueType::UI16:
          strcpy(typeString, "UI16");
          break;
        case ValueType::I32:
          strcpy(typeString, "I32");
          break;
        case ValueType::UI32:
          strcpy(typeString, "UI32");
          break;
        case ValueType::I64:
          strcpy(typeString, "I64");
          break;
        case ValueType::UI64:
          strcpy(typeString, "UI64");
          break;
        case ValueType::F:
          strcpy(typeString, "F");
          break;
        case ValueType::D:
          strcpy(typeString, "D");
          break;
      }
      if(i>0)
      {
        //if it is not the first one, we should add a coma as a separation
        strcat(outputFormat, ",");
      }
      strcat(outputFormat, _valueList[i].name);
      strcat(outputFormat, "=");
      //if it is an array, we will put A: in front of the type, resulting to A:I8 for example
      if(_valueList[i].arraySize != nullptr)
      {
        strcat(outputFormat, "A:");
      }
      strcat(outputFormat, typeString);
    }
    free(typeString);
  }
  else
  {
    //Empty string
    outputFormat = (char*)malloc(1);
    *outputFormat = NULL;
  }
  return outputFormat;
}

void Sensor::_AddValueToValueList(char* valueName, void* value, ValueType type, uint16_t* arraySize)
{
  //make the list longer allocating memory for the new SensorValue
  _valueListSize++;
  _valueList = (Value*)realloc(_valueList, _valueListSize*sizeof(Value));
  
  //reserve space for the length of the string + NULL terminator
  _valueList[_valueListSize - 1].name = (char*)malloc(strlen(valueName) + 1);
  //copy the name
  strcpy(_valueList[_valueListSize - 1].name, valueName);
  
  //store the pointer to the value
  _valueList[_valueListSize - 1].value = value;
  
  //store the type of the value
  _valueList[_valueListSize - 1].type = type;
  
  //store the pointer to the variable that determines the number of elements. If it is not an array this has to be a nullptr
  _valueList[_valueListSize - 1].arraySize = arraySize;
}

uint16_t Sensor::_GetValueTypeSize(ValueType type)
{
    switch (type)
  {
    case ValueType::I8:
    case ValueType::UI8:
      return 1;
    case ValueType::I16:
    case ValueType::UI16:
      return 2;
    case ValueType::I32:
    case ValueType::UI32:
    case ValueType::F:
      return 4;
    case ValueType::I64:
    case ValueType::UI64:
    case ValueType::D:
      return 8;
  }
}


/*
 * Different AddValue functions for the different types
 */
 
void Sensor::_AddValue(char* valueName, int8_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::I8, arraySize);
}

void Sensor::_AddValue(char* valueName, uint8_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::UI8, arraySize);
}

void Sensor::_AddValue(char* valueName, int16_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::I16, arraySize);
}

void Sensor::_AddValue(char* valueName, uint16_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::UI16, arraySize);
}

void Sensor::_AddValue(char* valueName, int32_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::I32, arraySize);
}

void Sensor::_AddValue(char* valueName, uint32_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::UI32, arraySize);
}

void Sensor::_AddValue(char* valueName, int64_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::I64, arraySize);
}

void Sensor::_AddValue(char* valueName, uint64_t* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::UI64, arraySize);
}

void Sensor::_AddValue(char* valueName, float* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::F, arraySize);
}

void Sensor::_AddValue(char* valueName, double* value, uint16_t* arraySize)
{
  _AddValueToValueList(valueName, (void*) value, ValueType::D, arraySize);
}
