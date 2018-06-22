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

#include "SRFSonar.h"
#include <cstring>
#include <Wire.h>
#include <cstdlib>

const float SPEED_OF_SOUND = 343.0;  //m/s
const float SPEED_OF_SOUND_US = SPEED_OF_SOUND / 1000000.0;  // m / µs

SRFSonar::SRFSonar(const char* name, uint8_t I2CAddress, SRFType type, uint8_t gain, uint8_t range) :
		Device(name, "This is a ultrasonic sensor. It can be used to measure distances."), _gain(gain), _range(range), _retransmitCtr(1), _type(type)
{
	_I2CAddress = I2CAddress;

	// reserve space for the raw values
	switch (type)
	{
	case SRF08:
		_lastReadingRawLength = SRF08_READ_VALUES;
		_startAddress = SRF08_START_ADDR;
		_nrOfBytesToRead = SRF08_READ_BYTES;
		_hasLightSensor = true;
		break;
	case SRF02:
		_lastReadingRawLength = SRF02_READ_VALUES;
		_startAddress = SRF02_START_ADDR;
		_nrOfBytesToRead = SRF02_READ_BYTES;
		_hasLightSensor = false;
		break;
	}

	_lastReadingRaw = (uint16_t *) malloc(sizeof(uint16_t) * _lastReadingRawLength);
	_lastReadingsBuffer = (uint16_t *) malloc(sizeof(uint16_t) * FILTER_WINDOW_SIZE);
	_timestamp = 0;

	_AddVariable("firstDistance", &_firstDistance);
	_AddVariable("firstDistanceCorrected", &_firstDistanceCorrected);
	_AddVariable("distanceValues", _lastReadingRaw, &_lastReadingRawLength);
	_AddVariable("timestamp", &_timestamp);

	if (_hasLightSensor)
	{
		_AddVariable("lightSensor", &_lightSensorValue);
	}

	startUltrasonicRanging();
}

void SRFSonar::Update()
{
	//Read register 0 (version reg) of sonar. If the sonar is currently ranging this read is specified to return 0xFF.
	Wire1.beginTransmission(_I2CAddress);
	Wire1.write(byte(0x00));                 //set register pointer to reg 0
	Wire1.endTransmission();

	Wire1.requestFrom((int) _I2CAddress, 1);
	unsigned char complete = Wire1.read();   //read register
	// Serial.println(complete);

	//check if sonar is idle
	if (complete != 0xFF)
	{
		//experimental serial comm detection
		int avail;
		do
		{
			avail = SerialUSB.available();
			//read most recent data from sonar
			readUltrasonicSensorValue();
		}
		while (SerialUSB.available() != avail);

		//start new ranging
		startUltrasonicRanging();
	}
}

// Function which gives the ultrasonic sensor the instruction to start a measurement 
void SRFSonar::startUltrasonicRanging()
{
	//SRF08: setup gain and range
	if (_type == SRF08)
	{
		_retransmitCtr--;
		if (_retransmitCtr == 0)
		{
			Wire1.beginTransmission(_I2CAddress);  // transmit to SRF08
			Wire1.write(byte(0x01));      // sets register pointer to gain register

			//write gain and range settings to sonar
			Wire1.write(byte(_gain));
			Wire1.write(byte(_range));

			Wire1.endTransmission();      // stop transmitting

			//reset counter
			_retransmitCtr = SRF08_SETTINGS_RETRANSMIT;
		}
	}

	// step 1: instruct sensor to read echoes
	Wire1.beginTransmission(_I2CAddress);  //transmit to sonar address
	Wire1.write(byte(0x00));      // sets register pointer to the command register (0x00)
	Wire1.write(byte(0x52));      // command sensor to measure in "centimeters" (0x51)
								  // use 0x50 for inches
								  // use 0x52 for ping microseconds
	Wire1.endTransmission();      // stop transmitting
}

void SRFSonar::changeAddress(uint8_t address)
{
	Wire1.beginTransmission(_I2CAddress);
	Wire1.write(byte(0x00));
	Wire1.write(byte(0xA0));
	Wire1.endTransmission();

	Wire1.beginTransmission(_I2CAddress);
	Wire1.write(byte(0x00));
	Wire1.write(byte(0xAA));
	Wire1.endTransmission();

	Wire1.beginTransmission(_I2CAddress);
	Wire1.write(byte(0x00));
	Wire1.write(byte(0xA5));
	Wire1.endTransmission();

	Wire1.beginTransmission(_I2CAddress);
	Wire1.write(byte(0x00));
	Wire1.write(address);
	Wire1.endTransmission();

	_I2CAddress = address;
}

static int compare_ui16(const void * a, const void * b)
{
	return (*(uint16_t*) a - *(uint16_t*) b);
}

// Function which reads the value from the ultrasonic sensor.
void SRFSonar::readUltrasonicSensorValue()
{
	// step 3: instruct sensor to return a particular echo reading
	Wire1.beginTransmission(_I2CAddress);  // transmit to sonar
	Wire1.write(byte(_startAddress));      // sets register pointer to required register
	Wire1.endTransmission();      // stop transmitting

	// step 4: request reading from sensor
	Wire1.requestFrom(_I2CAddress, (uint8_t) _nrOfBytesToRead);    // request 2 bytes from slave device #112

	// Wire.read();
	// step 5: receive reading from sensor
	if (_nrOfBytesToRead <= Wire1.available())    // if two bytes were received
	{
		if (_hasLightSensor)
		{
			_lightSensorValue = Wire1.read();
		}

		for (int i = 0; i < _lastReadingRawLength; i++)
		{
			_lastReadingRaw[i] = Wire1.read();
			_lastReadingRaw[i] = _lastReadingRaw[i] << 8;
			_lastReadingRaw[i] |= Wire1.read();
		}
		_timestamp = micros();

		// median filtering
#if FILTER_WINDOW_SIZE > 1
		uint16_t sortedValues[FILTER_WINDOW_SIZE];
		_lastReadingsBuffer[_lastReadingPointer] = _lastReadingRaw[0];
		_lastReadingPointer = (_lastReadingPointer + 1) % FILTER_WINDOW_SIZE;
		memcpy(sortedValues, _lastReadingsBuffer, sizeof(uint16_t) * FILTER_WINDOW_SIZE);
		qsort(sortedValues, FILTER_WINDOW_SIZE, sizeof(uint16_t), compare_ui16 );
		_firstDistance = sortedValues[FILTER_WINDOW_SIZE / 2];
#else
		_firstDistance = _lastReadingRaw[0];
#endif

		_firstDistanceCorrected = static_cast<float>(_firstDistance) * (SPEED_OF_SOUND_US / 2);  //sound has to travel twice the distance (to the object and back again)
	}
}
