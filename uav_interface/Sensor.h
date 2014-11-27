#ifndef SENSOR_H_
#define SENSOR_H_

#include <memory>

class Sensor;

struct SensorData
{
		std::unique_ptr<char> data;
		uint16_t size;
};

struct SensorList
{
	Sensor** elements;
	uint8_t length;
	explicit SensorList(uint8_t len);
	~SensorList();
};

class Sensor
{
public:
    Sensor(char* name);
    virtual SensorData Output(uint8_t payloadSize, char* payloadBuffer) = 0;
    virtual void InitializeSensor() = 0;
    virtual void UpdateSensor() = 0;

    char* sensorName;
    char* outputFormat;
    char* sensorDescription;
};
	


	
#endif
