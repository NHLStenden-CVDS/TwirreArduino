#ifndef SENSOR_H_
#define SENSOR_H_

#include <memory>
#include "RequestHandler.h"

class Sensor;

struct SensorValue
{
    void* value = nullptr;
    uint16_t size = 0;
};

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
    Sensor(char* name);
    virtual SensorData Output(Payload payload) = 0;
    virtual void InitializeSensor() = 0;
    virtual void UpdateSensor() = 0;

private:
    char* _sensorName;
    char* _outputFormat;
    char* _sensorDescription;
    
    void _AddValueToValueList(void* value, uint16_t valueSize);
    
    SensorValue* _valueList;
    uint8_t _valueListSize;
    
protected:
    void _AddValue();
};
	


	
#endif
