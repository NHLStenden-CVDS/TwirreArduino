/*#ifndef ACTUATOR_H_
#define ACTUATOR_H_

class Actuator
{
public:
    Actuator(std::string name);
    virtual uint16_t Input(uint8_t nrParam, char* paramData) = 0;
    virtual void InitializeActuator() = 0;
    virtual void UpdateActuator() = 0;

    std::string actuatorName;
    std::string inputFormat;
    std::string actuatorDescription;
};

#endif*/
