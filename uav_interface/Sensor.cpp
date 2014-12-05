#include "Sensor.h"

SensorList::SensorList(uint8_t len)
{
  length = len;
  elements = new Sensor*[length];
}

SensorList::~SensorList()
{
  delete elements;
}

Sensor::Sensor(char* name)
{
  _sensorName = name;

  _outputFormat = nullptr;
  _valueListSize = 0;
  _valueList = nullptr;
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
  return sensorData;
}

char* Sensor::GetOutputFormatString()
{
  uint16_t outputFormatLength = 0; 
  for(uint16_t i=0; i < _valueListSize; ++i)
  {
    outputFormatLength += strlen(_valueList[i].name) + 10; //+10 for the '=type,' it can result to 'name=A:UI64,'
  }
  
  //create string for the outputformat. Its size depends on the number of values that the sensor has
  char* outputFormat = (char*)malloc(outputFormatLength + 1); //+1 for the null terminator
  outputFormat[0] = NULL; //We set it as an empty string so we will be able to use strcat
  
  //for every value we will have a type which will be of maximum 4 chars
  char* typeString = (char*)malloc(5); //space for 4 chars + NULL terminator
  for(uint16_t i=0; i < _valueListSize; ++i)
  {
    switch (_valueList[i].type)
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
    if(i>0)
    {
      //if it is not the first one, we should add a coma as a separation
      strcat(outputFormat, ",");
    }
    strcat(outputFormat, _valueList[i].name);
    strcat(outputFormat, "=");
    //if it is an array, we will put A: in front of the type, resulting to A:I8 for example
    if(_valueList[i].arraySize == nullptr)
    {
      strcat(outputFormat, "A:");
    }
    strcat(outputFormat, typeString);
  }
  free(typeString);
  
  return outputFormat;
}

char* 

void Sensor::_AddValueToValueList(char* valueName, void* value, ValueType type, uint16_t* arraySize=nullptr);
{
  //make the list longer allocating memory for the new SensorValue
  _valueListSize++;
  SensorValue* _valueList = (SensorValue*)realloc(_valueList, sizeof(SensorValue));
  
  //reserve space for the length of the string + NULL terminator
  _valueList[_valueListSize - 1].name = (char*)malloc(strlen(valueName) + 1);
  
  //copy the name
  strcpy
  
  //store the pointer to the value
  _valueList[_valueListSize - 1].value = value;
  
  //store the size of the value in bytes
  _valueList[_valueListSize - 1].type = type;
  
  //store the pointer to the variable that determines the number of elements. If it is not an array this has to point to NULL
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
 
void Sensor::_AddValue(char* valueName, uint16_t* value, uint16_t* arraySize=nullptr);
{
  _AddValueToValueList(valueName, (void*) value, ValueType::UI16, arraySize);
}
