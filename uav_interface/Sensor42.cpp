#include "Sensor42.h"
#include <cstring>

uint32_t _value;
uint32_t* _valueArray;
uint16_t _valueArraySize;

Sensor42::Sensor42(char* name) : Sensor(name)
{
	sensorDescription = "This sensor is used for testing purposes. It returns the parameter 'value' which is fixed to 42.";
	//outputFormat = "value=I32";
        addParam("value",&_value);
        addArrayParam("valueArray",_valueArray,&_valueArraySize);
}

/*SensorData Sensor42::Output(Payload payload)
{
	SensorData sensorData;
	sensorData.size = 4;
	char* data = new char[sensorData.size];
	memcpy(data, &_value, sensorData.size);
	sensorData.data = std::unique_ptr<char>(data);
	return sensorData;
}*/

void Sensor42::InitializeSensor()
{
	_value = 0;
}

void Sensor42::UpdateSensor()
{
        _value = 42;
}

