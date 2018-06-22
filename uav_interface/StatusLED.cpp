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

#include "StatusLED.h"
#include "Arduino.h"

StatusLED::StatusLED(const char* name) :
		Device(name, "With this actuator you can control the RGB Status LED")
{
	pinMode(LEDR, OUTPUT);
	pinMode(LEDG, OUTPUT);
	pinMode(LEDB, OUTPUT);

	digitalWrite(LEDR, LOW);
	digitalWrite(LEDG, LOW);
	digitalWrite(LEDB, LOW);

	_AddVariable("red", &_red);
	_AddVariable("green", &_green);
	_AddVariable("blue", &_blue);
}

void StatusLED::ValuesChanged()
{
	digitalWrite(LEDR, _red);
	digitalWrite(LEDG, _green);
	digitalWrite(LEDB, _blue);
}

void StatusLED::SetValue(int8_t red, int8_t green, int8_t blue)
{
	_red = red;
	_green = green;
	_blue = blue;

	ValuesChanged();
}

