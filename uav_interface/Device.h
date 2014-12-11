#ifndef DEVICE_H_
#define DEVICE_H_

#include <cstdint>
#include <memory>

struct DeviceVariableSize
{
  uint16_t elementSize = 0;
  uint16_t numberOfElements = 0;
  bool isArray = false;
};

struct DeviceVariable
{
  void* variable = nullptr;
  DeviceVariableSize variableSize;
};

class Device
{
public:
  Device(char* name, char* description);
  virtual ~Device();
  DeviceVariable GetVariable(uint8_t variableID);
  void SetVariable(uint8_t variableID, void* variable);
  DeviceVariableSize GetVariableSize(uint8_t variableID);
  uint8_t GetNumberOfVariables();
  char* GetName();
  char* GetDescription();
  std::unique_ptr<char> GetVariablesFormatString();
  virtual void Update() = 0;

protected:
  void _AddVariable(char* variableName, int8_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, uint8_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, int16_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, uint16_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, int32_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, uint32_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, int64_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, uint64_t* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, float* variable, uint16_t* arraySize=nullptr);
  void _AddVariable(char* variableName, double* variable, uint16_t* arraySize=nullptr);
  
  char* _name;
  char* _description;
  
private:
  enum VariableType
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

  struct Variable
  {
    void* variable = nullptr;
    uint16_t* arraySize = nullptr;
    char* name;
    VariableType type;
  };

  void _AddVariableToVariableList(char* variableName, void* variable, VariableType type, uint16_t* arraySize=nullptr);
  uint16_t _GetVariableTypeSize(VariableType type);
  
  Variable* _variableList;
  uint8_t _variableListSize;
};

#endif
