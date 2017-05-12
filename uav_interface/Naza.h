#ifndef NAZA_H_
#define NAZA_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>
#include "GR12.h"

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
    void writeStickValues();
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


    float _targetPitch, _targetRoll, _targetYaw, _targetGaz;
    float _PWMPitch, _PWMRoll, _PWMYaw, _PWMGaz;


    int deadzone_pwm_counter = 0;
    bool deadzone_pwm_toggle = false;
    
    GR12 *_gr12 = nullptr;

      
    void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency);
    void StorePWMValue(float in, float &target, float &pwm);
};

#endif
