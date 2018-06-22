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

#ifndef GR12_H_
#define GR12_H_

#include "Arduino.h"
#include "Device.h"

#define HANDLE_CHANGE_PORT(PORT_NR)                                        \
volatile uint64_t lastPulseTime##PORT_NR;                                  \
volatile uint64_t lastPulseDuration##PORT_NR;                              \
                                                                           \
void handleChangePort##PORT_NR ()                                          \
{                                                                          \
  if((micros() - lastPulseTime##PORT_NR ) > 10000)                         \
  {                                                                        \
    lastPulseTime##PORT_NR = micros();                                     \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    lastPulseDuration##PORT_NR = micros() - lastPulseTime##PORT_NR;        \
    lastPulseTime##PORT_NR = micros();                                     \
  }                                                                        \
}  

class GR12: public Device
{
public:
	GR12(const char* name);
	void Update();
	double getPitch();
	double getYaw();
	double getGaz();
	double getRoll();

private:
	double _pitch;
	double _yaw;
	double _gaz;
	double _roll;
	uint8_t _isAutonomous;

	double dutyCycleToStickValue(uint64_t pulseTime, double minPulseTime, double maxPulseTime);
	//virtual void OnRequest();
};

#endif
