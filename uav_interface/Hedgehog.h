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

#ifndef HEDGEHOG_H_
#define HEDGEHOG_H_

#include "Device.h"

#define HEDGEHOG_CS_PIN 52
#define HEDGEHOG_BUF_SIZE 32 
#define HEDGEHOG_PACKET_SIZE 29

class Hedgehog: public Device
{
public:
	Hedgehog(const char *name);
	void init();
	void Update();
private:
	void loop_hedgehog();
	void hedgehog_set_crc16(uint8_t *buf, uint8_t size);

	float _pos_x, _pos_y, _pos_z;
	uint32_t _pos_timestamp;
	uint8_t hedgehog_spi_buf[HEDGEHOG_BUF_SIZE];
	int8_t _lastfail;
	uint8_t _failbyte;
	int32_t _failcount;

	const int hedgehog_packet_header[5] =
	{ 0xff, 0x47, 0x01, 0x00, 0x10 };
	typedef union
	{
		uint8_t b[2];
		unsigned int w;
	} uni_8x2_16;

	unsigned long lastUpdate = 0;

	bool hedgehog_pos_updated;

};

#endif
