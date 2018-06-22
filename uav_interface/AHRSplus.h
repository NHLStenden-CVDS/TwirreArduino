/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef AHRSPLUS_H_
#define AHRSPLUS_H_

#include "Device.h"

#pragma pack(push, 1)
struct AHRSdata
{
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
	int16_t magX;
	int16_t magY;
	int16_t magZ;
	int16_t temp;
	int16_t roll;
	int16_t pitch;
	int16_t yaw;
	//int16_t quaternionX;
	//int16_t quaternionY;
	//int16_t quaternionZ;
	//int16_t quaternionW;
};

struct AHRSinfo
{
	uint8_t whoami;
	uint8_t rev_major;
	uint8_t rev_minor;
	uint8_t status;
};

struct AHRSdataNormalized
{
	float accX;
	float accY;
	float accZ;
	float gyroX;
	float gyroY;
	float gyroZ;
	float magX;
	float magY;
	float magZ;
	float temp;
	float roll;
	float pitch;
	float yaw;
	//float quaternionX;
	//float quaternionY;
	//float quaternionZ;
	//float quaternionW;
};

#pragma pack(pop)

class AHRSplus: public Device
{
public:
	AHRSplus(const char* name);
	virtual void OnRequest();

private:
	AHRSdata _data;
	AHRSinfo _info;
	AHRSdataNormalized _normalizedData;
	uint16_t readBytes(uint8_t* dest, uint8_t regAddress, int count);
	void dataToNormalizedData();
};

#endif
