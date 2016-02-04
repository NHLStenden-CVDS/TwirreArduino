#include <cstring>
#include <cstdlib>

#include "Device.h"

Device::Device(const char* name, const char* description)
{
  _name = name;//(char*)malloc(strlen(name) + 1); //+1 for the NULL terminator
  //strcpy(_name, name);
  _description = description;//(char*)malloc(strlen(description) + 1); //+1 for the NULL terminator
  //strcpy(_description, description);

  _variableListSize = 0;
  _variableList = nullptr;
}

Device::~Device()
{
  //free(_name);
  //free(_description);
}

uint8_t Device::GetNumberOfVariables()
{
  return _variableListSize;
}

DeviceVariable Device::GetVariable(uint8_t variableID)
{
  DeviceVariable deviceVariable;
  if (variableID < _variableListSize)
  {
    deviceVariable.variable = _variableList[variableID].variable;
    if (_variableList[variableID].IsArray())
    {
      deviceVariable.variableSize.numberOfElements = *(_variableList[variableID].arraySize);
      deviceVariable.variableSize.isArray = true;
    }
    else
    {
      deviceVariable.variableSize.numberOfElements = 1; //1 in case it is not an array, which we check next
    }
    deviceVariable.variableSize.elementSize = _GetVariableTypeSize(_variableList[variableID].type);
  }
  else
  {
    deviceVariable.variable = nullptr;
    deviceVariable.variableSize.elementSize = 0;
    deviceVariable.variableSize.numberOfElements = 0;
  }
  return deviceVariable;
}

//Returns number of bytes set
uint16_t Device::SetVariable(uint8_t variableID, void* variable)
{
  if (variableID < _variableListSize)
  {
    uint32_t totalSize = 0;
    uint16_t elementSize = _GetVariableTypeSize(_variableList[variableID].type);
    uint32_t nrOfElements = 1; //we assume now that it is not an array, an if statement might change this later
    unsigned char* currentBufferPosition = (unsigned char*)variable;

    //if it is an array we store the array size
    if (_variableList[variableID].IsArray())
    {
      nrOfElements = *((uint32_t*)currentBufferPosition);
      uint32_t size = sizeof(uint32_t);
      totalSize += size;
      currentBufferPosition += size;
      *_variableList[variableID].arraySize = nrOfElements; //store how many elements the array has
    }

    uint32_t size = elementSize * nrOfElements;
    memcpy(_variableList[variableID].variable, currentBufferPosition, size);
    totalSize += size;

    return totalSize;
  }
  else
  {
    return 0;
  }
}

DeviceVariableSize Device::GetVariableSize(uint8_t variableID)
{
  DeviceVariableSize variableSize;
  if (variableID < _variableListSize)
  {
    if (_variableList[variableID].IsArray()) //it is an array
    {
      variableSize.numberOfElements = *(_variableList[variableID].arraySize);
      variableSize.isArray = true;
    }
    else
    {
      variableSize.numberOfElements = 1; //1 in case it is not an array, which we check next
    }
    variableSize.elementSize = _GetVariableTypeSize(_variableList[variableID].type);
  }
  else
  {
    //Error
    variableSize.elementSize = 0;
    variableSize.numberOfElements = 0;
  }
  return variableSize;
}

const char* Device::GetName()
{
  return _name;
}

const char* Device::GetDescription()
{
  return _description;
}

std::unique_ptr<char> Device::GetVariablesFormatString()
{
  char* format;
  uint16_t formatLength = 0;
  for (uint16_t i = 0; i < _variableListSize; ++i)
  {
    formatLength += strlen(_variableList[i].name) + 10; //+10 for the '=type,' it can result to 'name=A:UI64,'
  }

  if (formatLength > 0)
  {
    //create string for the outputformat. Its size depends on the number of values that the sensor has
    format = (char*)malloc(formatLength + 1); //+1 for the null terminator
    format[0] = 0; //We set it as an empty string so we will be able to use strcat

    //for every value we will have a type which will be of maximum 4 chars
    char* typeString = (char*)malloc(5); //space for 4 chars + NULL terminator
    for (uint16_t i = 0; i < _variableListSize; ++i)
    {
      switch (_variableList[i].type)
      {
        case VariableType::I8:
          strcpy(typeString, "I8");
          break;
        case VariableType::UI8:
          strcpy(typeString, "UI8");
          break;
        case VariableType::I16:
          strcpy(typeString, "I16");
          break;
        case VariableType::UI16:
          strcpy(typeString, "UI16");
          break;
        case VariableType::I32:
          strcpy(typeString, "I32");
          break;
        case VariableType::UI32:
          strcpy(typeString, "UI32");
          break;
        case VariableType::I64:
          strcpy(typeString, "I64");
          break;
        case VariableType::UI64:
          strcpy(typeString, "UI64");
          break;
        case VariableType::F:
          strcpy(typeString, "F");
          break;
        case VariableType::D:
          strcpy(typeString, "D");
          break;
      }
      if (i > 0)
      {
        //if it is not the first one, we should add a comma as a separation
        strcat(format, ",");
      }
      strcat(format, _variableList[i].name);
      strcat(format, "=");
      //if it is an array, we will put A: in front of the type, resulting to A:I8 for example
      if (_variableList[i].IsArray())
      {
        strcat(format, "A:");
      }
      strcat(format, typeString);
    }
    free(typeString);
  }
  else
  {
    //Empty string
    format = (char*)malloc(1);
    *format = 0;
  }
  return std::unique_ptr<char>(format);
}

void Device::_AddVariableToVariableList(const char* variableName, void* variable, VariableType type, uint32_t* arraySize)
{
  //make the list longer allocating memory for the new SensorValue
  _variableListSize++;
  _variableList = (Variable*)realloc(_variableList, _variableListSize * sizeof(Variable));

  _variableList[_variableListSize - 1].name = variableName;
  //reserve space for the length of the string + NULL terminator
  //_variableList[_variableListSize - 1].name = (char*)malloc(strlen(variableName) + 1);
  //copy the name
  //strcpy(_variableList[_variableListSize - 1].name, variableName);

  //store the pointer to the value
  _variableList[_variableListSize - 1].variable = variable;

  //store the type of the value
  _variableList[_variableListSize - 1].type = type;

  //store the pointer to the variable that determines the number of elements. If it is not an array this has to be a nullptr
  _variableList[_variableListSize - 1].arraySize = arraySize;
}

uint16_t Device::_GetVariableTypeSize(VariableType type)
{
  switch (type)
  {
    case VariableType::I8:
    case VariableType::UI8:
      return 1;
    case VariableType::I16:
    case VariableType::UI16:
      return 2;
    case VariableType::I32:
    case VariableType::UI32:
    case VariableType::F:
      return 4;
    case VariableType::I64:
    case VariableType::UI64:
    case VariableType::D:
      return 8;
  }
}

inline bool Device::Variable::IsArray()
{
  return (arraySize != nullptr);
}

/*
 * Different AddValue functions for the different types
 */

void Device::_AddVariable(const char* variableName, int8_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::I8, arraySize);
}

void Device::_AddVariable(const char* variableName, uint8_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::UI8, arraySize);
}

void Device::_AddVariable(const char* variableName, int16_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::I16, arraySize);
}

void Device::_AddVariable(const char* variableName, uint16_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::UI16, arraySize);
}

void Device::_AddVariable(const char* variableName, int32_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::I32, arraySize);
}

void Device::_AddVariable(const char* variableName, uint32_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::UI32, arraySize);
}

void Device::_AddVariable(const char* variableName, int64_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::I64, arraySize);
}

void Device::_AddVariable(const char* variableName, uint64_t* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::UI64, arraySize);
}

void Device::_AddVariable(const char* variableName, float* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::F, arraySize);
}

void Device::_AddVariable(const char* variableName, double* variable, uint32_t* arraySize)
{
  _AddVariableToVariableList(variableName, (void*) variable, VariableType::D, arraySize);
}
