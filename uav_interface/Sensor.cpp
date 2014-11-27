#include "Sensor.h"

Sensor::Sensor(char* name)
{
    sensorName = name;
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
