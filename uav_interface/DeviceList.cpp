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

#include "DeviceList.h"
#include <cstdlib>

DeviceList::DeviceList()
{
	_length = 0;
	_elements = nullptr;
}

DeviceList::~DeviceList()
{
	if (_elements != nullptr)
	{
		free(_elements);
	}
}

void DeviceList::Add(Device* device)
{
	_length++;
	_elements = (Device**) realloc(_elements, _length * sizeof(Device*));
	_elements[_length - 1] = device;
}

void DeviceList::UpdateAll()
{
	for (uint8_t i = 0; i < _length; ++i)
	{
		_elements[i]->Update();
	}
}

uint8_t DeviceList::GetLength()
{
	return _length;
}

Device* DeviceList::Get(uint8_t id)
{
	if (id < _length)
	{
		return _elements[id];
	}
	else
	{
		return nullptr;
	}
}
