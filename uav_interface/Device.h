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
    
    /**
     * This method gives a pointer to the variable required.
     * @return Returns a DeviceVariable which refers to the variable specified in VariableID.
     */
    DeviceVariable GetVariable(uint8_t variableID);
    
    /**
     * This method sets the variable, specified with the given ID, with the data that contains the pointer.
     * @return Returns the number of bytes read from that pointer.
     */
    uint16_t SetVariable(uint8_t variableID, void* variable);
    
    /**
     * Gives size information (element byte size, number of elements and a boolean that states if it is an array or not).
     * @return Returns a DeviceVariableSize with the size information of the variable specified in VariableID.
     */
    DeviceVariableSize GetVariableSize(uint8_t variableID);
    
    /**
     * Returns the number of variables that can be set or got from this device has.
     * @return Returns a uint8_t with the number of variables.
     */
    uint8_t GetNumberOfVariables();
    
    /**
     * Gives the name of the device.
     * @return Returns a char pointer which refers to a null terminated string.
     */
    char* GetName();
    
    /**
     * Gives the description of the device.
     * @return Returns a char pointer which refers to a null terminated string.
     */
    char* GetDescription();
    
    /**
     * Gives the format string for the variables of the device. A unique pointer is returned in order to automatically delete the object after being used.
     * @return Returns a unique pointer to a char pointer. This refers to a null terminated string.
     */
    std::unique_ptr<char> GetVariablesFormatString();
    
    /**
     * This is the main method of the device. It will set the variables (read from a sensor) or use them to actuate.
     * Every sensor or actuator has to implement this method. It will be called continuously in the main loop.
     */
    virtual void Update() = 0;

  protected:
    /**
     * It adds a variable to the variable list in order to be able to set or get it from the main board side.
     * It uses _AddVariableToVariableList() with the correct type. If arraySize is not nullptr, the variable that is being added is an array.
     * arraySize then will be a pointer to a uint16_t that contains the number of elements of the array variable.
     * The variables will be added consecutively to a list. Later, they will be accessed with an index (ID).
     */
    void _AddVariable(char* variableName, int8_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, uint8_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, int16_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, uint16_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, int32_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, uint32_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, int64_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, uint64_t* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, float* variable, uint16_t* arraySize = nullptr);
    void _AddVariable(char* variableName, double* variable, uint16_t* arraySize = nullptr);

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

      inline bool IsArray();
    };

    /**
     * It creates an object Variable with the information provided as parameters.
     * If arraySize is not nullptr, the variable that is being added is an array.
     * arraySize then will be a pointer to a uint16_t that contains the number of elements of the array variable.
     * The variables will be added consecutively to a list. Later, they will be accessed with an index (ID).
     */
    void _AddVariableToVariableList(char* variableName, void* variable, VariableType type, uint16_t* arraySize = nullptr);
    
    /**
     * Gives the size of every type in VariableType in bytes.
     * @return Returns a uint16_t.
     */
    uint16_t _GetVariableTypeSize(VariableType type);

    Variable* _variableList;
    uint8_t _variableListSize;
};

#endif
