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

#ifndef NAZA_H_
#define NAZA_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>
#include "GR12.h"

//generates PWM signals for controlling DJI NAZA flight controller. See cpp for more info.
class Naza: public Device
{
public:
	Naza(const char* name);
	Naza(const char* name, GR12 *gr12);
	void Update();
	void ValuesChanged();

	static Naza* Instance();
	static Naza* Initialize(const char* name);
	static Naza* Initialize(const char* name, GR12 *gr12);
	uint32_t * getTimeout();
	void writeDefaultStickValues();
	void writeStickValues();
private:
	static Naza* _instance;
	float _pitch;
	float _yaw;
	float _roll;
	float _gaz;

	uint32_t _timeout;

	uint8_t _auto_pitch = 0;
	uint8_t _auto_yaw = 0;
	uint8_t _auto_gaz = 0;
	uint8_t _auto_roll = 0;

	float _pitch_deadzone = 0;
	float _yaw_deadzone = 0;
	float _roll_deadzone = 0;
	float _gaz_deadzone = 0;

	float _targetPitch, _targetRoll, _targetYaw, _targetGaz;
	float _PWMPitch, _PWMRoll, _PWMYaw, _PWMGaz;

	int deadzone_pwm_counter = 0;
	bool deadzone_pwm_toggle = false;

	GR12 *_gr12 = nullptr;

	void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency);
	void StorePWMValue(float in, float &target, float &pwm);
};

#endif
