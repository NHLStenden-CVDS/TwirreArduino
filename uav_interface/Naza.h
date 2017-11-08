 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#ifndef NAZA_H_
#define NAZA_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>
#include "GR12.h"

//generates PWM signals for controlling DJI NAZA flight controller. See cpp for more info.
class Naza : public Device
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
     
  private:
    static Naza* _instance;
    float _pitch;
    float _yaw;
    float _roll;
    float _gaz;
    uint32_t _timeout;

    uint8_t _auto_pitch = 1;
    uint8_t _auto_yaw = 1;
    uint8_t _auto_gaz = 1;
    uint8_t _auto_roll = 1;

    float _pitch_deadzone = 0;
    float _yaw_deadzone = 0;
    float _roll_deadzone = 0;
    float _gaz_deadzone = 0;
    
    GR12 *_gr12 = nullptr;

      
    void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency);
};

#endif
