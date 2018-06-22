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

#include <Wire.h>

#include "AHRSplus.h"

#define ADDRESS 0x20

AHRSplus::AHRSplus(const char* name) :
		Device(name, "This is the myAHRS+ sensor. It is a fancy IMU or 'attitude heading reference system'")
{
//  _AddVariable("whoami", &_info.whoami);
//  _AddVariable("rev_major", &_info.rev_major);
//  _AddVariable("rev_minor", &_info.rev_minor);
//  _AddVariable("status", &_info.status);
//  
	_AddVariable("accX", &_normalizedData.accX);
	_AddVariable("accY", &_normalizedData.accY);
	_AddVariable("accZ", &_normalizedData.accZ);
//  _AddVariable("gyroX", &_normalizedData.gyroX);
//  _AddVariable("gyroY", &_normalizedData.gyroY);
//  _AddVariable("gyroZ", &_normalizedData.gyroZ);
//  _AddVariable("magX", &_normalizedData.magX);
//  _AddVariable("magY", &_normalizedData.magY);
//  _AddVariable("magZ", &_normalizedData.magZ);
	_AddVariable("temp", &_normalizedData.temp);
	_AddVariable("roll", &_normalizedData.roll);
	_AddVariable("pitch", &_normalizedData.pitch);
	_AddVariable("yaw", &_normalizedData.yaw);
//  _AddVariable("quaternionX", &_normalizedData.quaternionX);
//  _AddVariable("quaternionY", &_normalizedData.quaternionY);
//  _AddVariable("quaternionZ", &_normalizedData.quaternionZ);
//  _AddVariable("quaternionW", &_normalizedData.quaternionW);
//  
	readBytes(reinterpret_cast<uint8_t*>(&_info), 0x01, 4);
}

void AHRSplus::OnRequest()
{
	int recv = readBytes(reinterpret_cast<uint8_t*>(&_data), 0x22, sizeof(AHRSdata));
	if (recv == sizeof(AHRSdata)) dataToNormalizedData();
}

void AHRSplus::dataToNormalizedData()
{
	_normalizedData.accX = (16.0f / 32767.0f) * static_cast<float>(_data.accX);
	_normalizedData.accY = (16.0f / 32767.0f) * static_cast<float>(_data.accY);
	_normalizedData.accZ = (16.0f / 32767.0f) * static_cast<float>(_data.accZ);
	_normalizedData.gyroX = (2000.0f / 32767.0f) * static_cast<float>(_data.gyroX);
	_normalizedData.gyroY = (2000.0f / 32767.0f) * static_cast<float>(_data.gyroY);
	_normalizedData.gyroZ = (2000.0f / 32767.0f) * static_cast<float>(_data.gyroZ);
	_normalizedData.magX = static_cast<float>(_data.magX);
	_normalizedData.magY = static_cast<float>(_data.magY);
	_normalizedData.magZ = static_cast<float>(_data.magZ);
	_normalizedData.temp = (200.0f / 32767.0f) * static_cast<float>(_data.temp);
	_normalizedData.roll = (180.0f / 32767.0f) * static_cast<float>(_data.roll);
	_normalizedData.pitch = (180.0f / 32767.0f) * static_cast<float>(_data.pitch);
	_normalizedData.yaw = (180.0f / 32767.0f) * static_cast<float>(_data.yaw);
	//_normalizedData.quaternionX = static_cast<float>(_data.quaternionX) / 32767.0f;
	//_normalizedData.quaternionY = static_cast<float>(_data.quaternionY) / 32767.0f;
	//_normalizedData.quaternionZ = static_cast<float>(_data.quaternionZ) / 32767.0f;
	//_normalizedData.quaternionW = static_cast<float>(_data.quaternionW) / 32767.0f;
}

uint16_t AHRSplus::readBytes(uint8_t* dest, uint8_t regAddress, int count)
{
	int serAvailable = SerialUSB.available();

	TWI_StartRead(WIRE_INTERFACE, ADDRESS, regAddress, 1);

	//read all bytes except last one
	uint_fast16_t stopAmnt = (count - 1);
	for (uint_fast16_t i = 0; i < stopAmnt; ++i)
	{
		//wait for byte received
		while ((WIRE_INTERFACE->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY)
			;
		//read byte
		dest[i] = WIRE_INTERFACE->TWI_RHR;
	}

	//set stop condition before reading final byte
	TWI_SendSTOPCondition(WIRE_INTERFACE);
	while ((WIRE_INTERFACE->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY)
		;
	dest[count - 1] = WIRE_INTERFACE->TWI_RHR;

	//wait for completion of transfer
	while (((WIRE_INTERFACE->TWI_SR & TWI_SR_TXCOMP) != TWI_SR_TXCOMP))
		;

	//check for serial transfers (which can disrupt the i2c transaction)
	if (SerialUSB.available() != serAvailable) return 0;

	return count;
}
