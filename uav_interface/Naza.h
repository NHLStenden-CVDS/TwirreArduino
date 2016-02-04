#ifndef NAZA_H_
#define NAZA_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>

class Naza : public Device
{
  public:
    Naza(const char* name);
    void Update();
    void ValuesChanged();

    static Naza* Instance();
    static Naza* Initialize(const char* name);
    uint32_t * getTimeout();
     void writeDefaultStickValues();  
     
  private:
    static Naza* _instance;
    float _pitch;
    float _yaw;
    float _roll;
    float _gaz;
    uint32_t _timeout;
      
    void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency);
};

#endif
