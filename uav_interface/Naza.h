#ifndef NAZA_H_
#define NAZA_H_

#include "Device.h"

class Naza : public Device
{
  public:
    Naza(char* name);
    void Update();
    void ValuesChanged();

  private:
    float _pitch;
    float _yaw;
    float _roll;
    float _gaz;
    uint32_t _timeout;

};

#endif
