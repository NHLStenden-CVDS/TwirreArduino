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

#ifndef BOAT_H_
#define BOAT_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>

//generates PWM signals for controlling the main propulsion, bow screw and rudder

class Boat : public Device
{
  public:
    Boat(const char* name);
    void Update();
    
    signed int _MAIN_DIR = 0;
    uint8_t _MAIN_SPEED = 0;
    signed int _BOW_DIR = 0;
    signed int _RUDDER_ANGLE = 0;
    
  private:
    uint16_t last_update; 
    int state = 0;
	int old_dir = 0;
	int duty_cycle = 0;

    int InitDuty(int pin, int dutycycle);
    
    void SetDutyCycle_Main(int duty_Main, int dir_Main);
    void SetDutyCycle_Bow(int duty_Bow, int dir_Bow);
    void SetDutyCycle_Rudder(int duty_Rudder, int dir_Rudder);
};

#endif
